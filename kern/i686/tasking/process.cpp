/*
process.cpp

Copyright (c) 05 Yann BOUCHER (yann)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "tasking/process.hpp"
#include "tasking/process_data.hpp"
#include "i686/tasking/process.hpp"

#include <vector.hpp>

#include <sys/wait.h>

#include "i686/gdt/gdt.hpp"
#include "i686/tasking/tss.hpp"
#include "i686/interrupts/interrupts.hpp"
#include "i686/mem/physallocator.hpp"
#include "utils/aligned_vector.hpp"

#include "fs/fsutils.hpp"
#include "fs/vfs.hpp"

#include "utils/membuffer.hpp"
#include "utils/align.hpp"

struct SignalTrampolineInfo
{
    int32_t   signal;
    uintptr_t handler;
    uint32_t  flags;
    uint32_t  padding;
};
static_assert(sizeof(SignalTrampolineInfo) == 16);

extern "C" [[noreturn]] void userspace_jump(const registers* regs);

void Process::load_user_code(gsl::span<const uint8_t> code_to_copy, size_t allocated_size)
{
    data->code = std::make_shared<aligned_vector<uint8_t, Memory::page_size()>>();
    data->code->resize(std::max<int>(code_to_copy.size(), allocated_size), 0);
    std::copy(code_to_copy.begin(), code_to_copy.end(), data->code->begin());
    std::fill(data->code->data() + code_to_copy.size(), data->code->data() + data->code->size(), 0); // fill bss section

    data->stack.clear();
    data->stack.resize(2*Paging::page_size);
    auto* regs = arch_context->user_regs;

    memset(regs, 0, sizeof(registers));

    regs->esp = user_stack_top;
    regs->eflags |= 0b11001000000000; // enable IF and IOPL=3, DF clear
    regs->eip = 0xdeadbeef;
    regs->cs = gdt::user_code_selector*0x8 | 0x3;
    regs->ds = regs->es = regs->fs = regs->gs = regs->ss = gdt::user_data_selector*0x8 | 0x3;
    // TODO : have %edx set to the sysv ABI convention's

    arch_context->fpu_state = FPU::make(); // init the FPU state

    if (m_current_process == this)
        unmap_address_space();

    release_mappings();
    create_mappings();

    if (m_current_process == this)
        map_address_space();
}

void Process::expand_stack(size_t size)
{
    arch_context->user_regs->esp -= size;
}

void Process::push_onto_stack(gsl::span<const uint8_t> data)
{
    auto current_sp = this->data->stack.size() - (user_stack_top - arch_context->user_regs->esp);
    assert(current_sp - data.size() >= 0);

    for (int i { 0 }; i < data.size(); ++i)
    {
        this->data->stack[current_sp - i - 1] = data[data.size() - i - 1];
    }

    expand_stack(data.size());
}

void Process::pop_stack(size_t size)
{
    arch_context->user_regs->esp += size;
}

void Process::wake_up(pid_t child, int err_code)
{
    Memory::phys_write(data->waitstatus_phys, &err_code, sizeof(err_code));

    data->waiting_pid.reset();

    status = Active;

    if (arch_context->user_regs)
        arch_context->user_regs->eax = child; // set waitpid return value
}

void Process::execute_sighandler(int signal, pid_t returning_pid, const siginfo_t &siginfo)
{
    assert(arch_context);

    data->sig_context.push(ProcessData::SigContext{arch_context, returning_pid});
    arch_context = new ProcessArchContext(*arch_context);
    // keep the same stack for signal handling

    registers* reg_copy = new registers{*data->sig_context.top().cpu_context->user_regs};
    arch_context->user_regs = data->sig_context.top().cpu_context->user_regs;
    data->sig_context.top().cpu_context->user_regs = reg_copy;

    auto sig = data->sig_handlers->at(signal);

    if (sig.sa_flags & ~(SA_SIGINFO))
    {
        warn("Unsupported signal flag : 0x%x\n", sig.sa_flags);
    }

    SignalTrampolineInfo info;

    info.signal  = signal;
    info.flags   = sig.sa_flags;
    info.handler = (uintptr_t)sig.sa_handler;

    push_onto_stack(siginfo); // siginfo_t
    uintptr_t siginfo_addr = arch_context->user_regs->esp;

    push_onto_stack(*data->sig_context.top().cpu_context); // ucontext_t
    uintptr_t ucontext_addr = arch_context->user_regs->esp;

    push_onto_stack(ucontext_addr);
    push_onto_stack(siginfo_addr);
    push_onto_stack(info);

    set_instruction_pointer(signal_trampoline_page);
}

void Process::exit_signal()
{
    assert(!data->sig_context.empty());

    // restore the saved copy
    registers reg_copy = *data->sig_context.top().cpu_context->user_regs;
    registers* reg_ptr = arch_context->user_regs;

    free_arch_context();
    arch_context = data->sig_context.top().cpu_context;
    pid_t returning_pid = data->sig_context.top().returning_process;

    delete arch_context->user_regs;
    arch_context->user_regs = reg_ptr;
    *arch_context->user_regs = reg_copy; // copy the saved register frame

    arch_context->user_regs->eax = 0; // Set the return value of kill() to 0, as success

    data->sig_context.pop();

    if (pid != returning_pid) task_switch(returning_pid);
}

bool Process::user_callback_fault_handler(const PageFault &fault)
{
    if (fault.level != PageFault::User) return false;
    if (fault.type != PageFault::Read && fault.type != PageFault::Execute) return false;
    if (!data->user_callbacks->list.count(fault.address)) return false;

    auto entry = data->user_callbacks->list.at(fault.address);
    do_user_callback(entry.callback, entry.arg_sizes);

    return true;
}

void Process::do_user_callback(const std::function<int (const std::vector<uintptr_t> &)> &callback, const std::vector<size_t>& arg_sizes)
{
    // TODO : copy_from_user

    // will be released by the jump_to_userspace call
    auto* regs = new registers{*arch_context->user_regs};
    uintptr_t return_address = *(uintptr_t*)(regs->esp);

    uintptr_t esp_copy = regs->esp;

    std::vector<uintptr_t> arguments;
    for (auto len : arg_sizes)
    {
        if (len == 2)
        {
            arguments.push_back(*(uint16_t*)(esp_copy));
            esp_copy += 2;
        }
        else if (len == 4)
        {
            arguments.push_back(*(uint32_t*)(esp_copy));
            esp_copy += 4;
        }
        else if (len == 8)
        {
            arguments.push_back(*(uint64_t*)(esp_copy));
            esp_copy += 8;
        }
        else
        {
            assert(!!"Invalid argument size");
        }
    }

    for (auto arg : arguments)
    {
        kprintf("arg : 0x%x\n", arg);
    }

    int ret = callback(arguments);
    regs->eax = ret;

    arch_context->user_regs = regs;

    jump_to_user_space(return_address);
}

Process *Process::create_kernel_task(void (*procedure)())
{
    auto proc = Process::create();
    proc->arch_context->init_regs->eip = (uintptr_t)procedure; // set eip to the procedure's entry point

    return proc;
}

Process *Process::create_user_task()
{
    auto proc = create_kernel_task([]
    {
        Process::current().jump_to_user_space();
    });

    proc->arch_context->user_regs = new registers;

    return proc;
}

extern "C" int kernel_stack_bottom;
void Process::arch_init()
{
    assert(!arch_context);
    arch_context = new ProcessArchContext;
    arch_context->user_regs = nullptr;
    arch_context->fpu_state = FPU::make();

    data->kernel_stack = (uint8_t*)Memory::vmalloc(ProcessData::kernel_stack_size/Memory::page_size(), Memory::Write|Memory::Read);

    // kernel esp+4 must be 16-bit aligned on function entry
    arch_context->esp = (uintptr_t)(data->kernel_stack + ProcessData::kernel_stack_size) - sizeof(uintptr_t);

    *(uintptr_t*)(arch_context->esp-=4) = 0xdeadbeef; // eip
    *(uintptr_t*)(arch_context->esp-=4) = 0b1000000000; // eflags with IF
    *(uintptr_t*)(arch_context->esp-=4) = 0xdeadbeef; // eax
    *(uintptr_t*)(arch_context->esp-=4) = 0xdeadbeef; // ecx
    *(uintptr_t*)(arch_context->esp-=4) = 0xdeadbeef; // edx
    *(uintptr_t*)(arch_context->esp-=4) = 0xdeadbeef; // ebx
    *(uintptr_t*)(arch_context->esp-=4) = 0xcafebabe; // dummy_esp
    *(uintptr_t*)(arch_context->esp-=4) = 0xdeadbeef; // ebp
    *(uintptr_t*)(arch_context->esp-=4) = 0xdeadbeef; // esi
    *(uintptr_t*)(arch_context->esp-=4) = 0xdeadbeef; // edi
}

extern "C" void task_switch(uintptr_t* esp_from, uintptr_t* esp_to);

void Process::task_switch(pid_t pid)
{
    assert(pid != m_current_process->pid);
    auto next = by_pid(pid);
    assert(next);
    auto prev = m_current_process;

    prev->arch_context->fpu_state = FPU::save();

    prev->unswitch();
    next->switch_to();

    FPU::load(next->arch_context->fpu_state);

    m_current_process = next;

    assert(next->arch_context->init_regs->dummy_esp == 0xcafebabe); // check stack integrity
    ::task_switch(&prev->arch_context->esp, &next->arch_context->esp);
}

void Process::set_instruction_pointer(unsigned int value)
{
    arch_context->user_regs->eip = value;
}
uintptr_t Process::stack_pointer() const
{
    return arch_context->user_regs->esp;
}

void Process::jump_to_user_space(uintptr_t ip)
{
    assert(this == m_current_process);

    registers regs_copy = *arch_context->user_regs;

    delete arch_context->user_regs;

    regs_copy.eip = ip;
    regs_copy.eflags |= 0b1000000000; // enable IF

    asm volatile ("pushl %0\n"
                  "jmp userspace_jump\n"
                  ::"r"(&regs_copy));

    __builtin_unreachable();
}

void Process::jump_to_user_space()
{
    jump_to_user_space(arch_context->user_regs->eip);
}

Process *Process::clone(Process &proc, uint32_t flags)
{
    auto new_proc = Process::create();
    if (!new_proc) return nullptr;

    new_proc->data->code = std::make_shared<aligned_vector<uint8_t, Memory::page_size()>>(*proc.data->code); // noleak ? :(
    new_proc->data->stack = proc.data->stack; // noleak
    assert(new_proc->data->stack.data() != proc.data->stack.data()); // check separate

    new_proc->data->name = proc.data->name + "_child"; // noleak
    new_proc->data->uid = proc.data->uid;
    new_proc->data->gid = proc.data->gid;
    new_proc->parent = proc.pid;

    new_proc->data->pwd  = vfs::find(proc.data->pwd->path()).value(); assert(new_proc->data->pwd);
    new_proc->data->root = vfs::find(proc.data->root->path()).value(); assert(new_proc->data->root);

    new_proc->data->fd_table = std::make_shared<std::vector<tasking::FDInfo>>(*proc.data->fd_table); // noleak
    proc.copy_allocated_pages(*new_proc); // noleak

    // TODO : refactor this
    new_proc->data->user_callbacks = std::make_shared<tasking::UserCallbacks>(*proc.data->user_callbacks);

    new_proc->data->args = proc.data->args; // noleak
    new_proc->data->shm_list = proc.data->shm_list;
    new_proc->data->sig_handlers = std::make_shared<kpp::array<struct sigaction, SIGRTMAX>>(*proc.data->sig_handlers);

    if (proc.arch_context->user_regs != nullptr)
        new_proc->arch_context->user_regs = new registers{*proc.arch_context->user_regs};

    proc.data->children.emplace_back(new_proc->pid);

    new_proc->create_mappings();

    return new_proc;
}
// TODO : remove when using std::unique_ptr
void Process::free_arch_context()
{
    delete arch_context;
    arch_context = nullptr;
}

void Process::cleanup()
{
    release_mappings();

    free_arch_context();
}
