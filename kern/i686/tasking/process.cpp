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

extern "C" [[noreturn]] void do_switch_inter(const registers* regs);
extern "C" [[noreturn]] void do_switch_same(const registers* regs);

void Process::push_onto_stack(gsl::span<const uint8_t> data)
{
    auto current_sp = this->data->stack.size() - (user_stack_top - arch_context->regs.esp);
    assert(current_sp - data.size() >= 0);

    for (int i { 0 }; i < data.size(); ++i)
    {
        this->data->stack[current_sp - i - 1] = data[data.size() - i - 1];
    }

    arch_context->regs.esp -= data.size();
}

void Process::pop_stack(size_t size)
{
    arch_context->regs.esp += size;
}

void Process::wake_up(pid_t child, int err_code)
{
    Memory::phys_write(data->waitstatus_phys, &err_code, sizeof(err_code));

    data->waiting_pid.reset();

    status = Active;

    arch_context->regs.eax = child; // set waitpid return value
}

void Process::execute_sighandler(int signal, pid_t returning_pid, const siginfo_t &siginfo)
{
    assert(arch_context);
    data->sig_context.push(ProcessData::SigContext{arch_context, returning_pid});
    arch_context = new ProcessArchContext(*arch_context);
    // keep the same stack for signal handling

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
    uintptr_t siginfo_addr = arch_context->regs.esp;

    push_onto_stack(*data->sig_context.top().cpu_context); // ucontext_t
    uintptr_t ucontext_addr = arch_context->regs.esp;

    push_onto_stack(ucontext_addr);
    push_onto_stack(siginfo_addr);
    push_onto_stack(info);

    set_instruction_pointer(signal_trampoline_page);
    Process::current().unswitch();
    switch_to();
}

void Process::do_user_callback(const std::function<int (const std::vector<uintptr_t> &)> &callback, const std::vector<size_t>& arg_sizes)
{
    // TODO : copy_from_user
    auto& regs = arch_context->regs;
    uintptr_t return_address = *(uintptr_t*)(regs.esp);
    regs.esp += sizeof(uintptr_t);

    uintptr_t esp_copy = regs.esp;

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

    int ret = callback(arguments);
    regs.eax = ret;

    set_instruction_pointer(return_address);
    unswitch(); // TODO : optimizable
    switch_to();
}

void Process::exit_signal()
{
    assert(!data->sig_context.empty());

    free_arch_context();
    arch_context = data->sig_context.top().cpu_context;
    pid_t returning_pid = data->sig_context.top().returning_process;

    arch_context->regs.eax = 0; // Set the return value of kill() to 0, as success
    arch_context->regs.esp = data->sig_context.top().cpu_context->regs.esp; // restore the process' %esp register

    data->sig_context.pop();

    unswitch();
    Process::by_pid(returning_pid)->switch_to();
}

void Process::set_exec_level(Process::ExecLevel lvl)
{
    auto& regs = arch_context->regs;

    auto code_selector = (lvl == Process::User ? gdt::user_code_selector : gdt::kernel_code_selector);
    auto data_selector = (lvl == Process::User ? gdt::user_data_selector : gdt::kernel_data_selector);
    regs.cs = code_selector*0x8 | (lvl == Process::User ? 0x3 : 0x0);
    regs.ds = regs.es = regs.fs = regs.gs = regs.ss =
            data_selector*0x8 | (lvl == Process::User ? 0x3 : 0x0);
    if (lvl == Process::User)
    {
        regs.eflags |= 0b11000000000000; // IOPL = 3
    }
    else
    {
        regs.eflags &= ~0b11000000000000; // IOPL = 0;
        regs.esp = data->kernel_stack_ptr;
    }
}

void Process::arch_init(gsl::span<const uint8_t> code_to_copy, size_t allocated_size)
{
    assert(!arch_context);
    arch_context = new ProcessArchContext;

    data->stack.resize(2*Paging::page_size);
    data->kernel_stack.resize(16*Paging::page_size);
    // kernel esp+4 must be 16-bit aligned on function entry
    data->kernel_stack_ptr = (uintptr_t)(data->kernel_stack.data() + data->kernel_stack.size()) - sizeof(uintptr_t);

    data->code = std::make_shared<aligned_vector<uint8_t, Memory::page_size()>>();
    data->code->resize(std::max<int>(code_to_copy.size(), allocated_size));
    std::copy(code_to_copy.begin(), code_to_copy.end(), data->code->begin());
    std::fill(data->code->begin() + code_to_copy.size(), data->code->end(), 0); // clear the allocated part

    registers regs;
    memset(&regs, 0, sizeof(registers));

    regs.eax = data->args.size();
    regs.ecx = argv_virt_page;
    regs.esp = user_stack_top;
    regs.eflags |= 0b1000000000; // enable IF

    arch_context->regs = regs;

    set_exec_level(Process::User);

    create_mappings();

    set_args(data->args);
}

void Process::set_instruction_pointer(unsigned int value)
{
    arch_context->regs.eip = value;
}

void Process::switch_to()
{
    {
        //assert(status == Active); TODO : check if in sighandler too
        if (m_current_process &&
                (m_current_process->arch_context->regs.cs & 0x3) == 0) // if previous task was in ring 0
        {
            m_current_process->data->kernel_stack_ptr = m_current_process->arch_context->regs.esp;
        }

        map_address_space();
        map_shm();

        m_current_process = this;

        tss.esp0 = data->kernel_stack_ptr;
        arch_context->regs.eflags |= 0b1000000000; // enable IF
    }

    if ((m_current_process->arch_context->regs.cs & 0x3) == 0)
    {
        do_switch_same(&arch_context->regs);
    }
    else
    {
        do_switch_inter(&arch_context->regs);
    }

    __builtin_unreachable();
}

void Process::unswitch()
{
    unmap_address_space();
}

Process *Process::clone(Process &proc, uint32_t flags)
{
    auto new_proc = Process::create(proc.data->args);
    if (!new_proc) return nullptr;

    new_proc->data->code = std::make_shared<aligned_vector<uint8_t, Memory::page_size()>>(*proc.data->code); // noleak ? :(
    new_proc->data->stack = proc.data->stack; // noleak
    new_proc->data->kernel_stack = proc.data->kernel_stack;
    new_proc->data->kernel_stack_ptr = proc.data->kernel_stack_ptr;
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
    new_proc->arch_context = new ProcessArchContext;
    *new_proc->arch_context = *proc.arch_context;

    proc.data->children.emplace_back(new_proc->pid);

    new_proc->create_mappings();

    assert(new_proc);
    return new_proc;
}

void Process::unmap_address_space()
{
#if 0
    Paging::unmap_user_space(); // reloading all the page tables is really costly, not the right solution
#else
    for (const auto& pair : data->mappings)
    {
        Memory::unmap_page((void*)pair.first);
    }
    for (const auto& shm : data->shm_list)
    {
        Memory::unmap(shm.second.v_addr, shm.second.shm->size());
    }
#endif
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
