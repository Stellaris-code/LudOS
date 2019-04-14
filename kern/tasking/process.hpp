/*
process.hpp

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
#ifndef PROCESS_HPP
#define PROCESS_HPP

#include "config.hpp"

#include <vector.hpp>
#include <functional.hpp>

#include "mem/memmap.hpp"

#include "fdinfo.hpp"

#include "utils/gsl/gsl_span.hpp"
#include "utils/noncopyable.hpp"

#include <sys/types.h>
#include <signal.h>

#include "kstring/kstrfwd.hpp"

struct ProcessCreatedEvent
{
    pid_t pid;
};

struct ProcessDestroyedEvent
{
    pid_t pid;
    int err_code;
};

class SharedMemorySegment;
struct ProcessArchContext;
struct ProcessData;

struct PageFault;

class Process : NonCopyable
{
public:
    enum AccessRequestPerm : uint16_t
    {
        ReadRequest,
        WriteRequest,
        ExecRequest
    };
    enum ExecLevel
    {
        User,
        Kernel
    };

    struct status_info
    {
        void (*timeout_action)(const Process* proc, void* arg);
        void*  timeout_action_arg;
    };

    static constexpr pid_t  init_pid = 0;
    static constexpr size_t root_uid = 0;
    static constexpr int    max_priority = 40;
    static constexpr int    min_priority = 1;
    static constexpr size_t tls_pages = 1;
    static constexpr uintptr_t signal_trampoline_page = KERNEL_VIRTUAL_BASE - (1*Memory::page_size());
    static constexpr size_t    user_stack_top         = KERNEL_VIRTUAL_BASE - (1*Memory::page_size());
    static constexpr kpp::array<uintptr_t, 64> default_sighandler_actions
    {{
            SIG_ACTION_TERM, // 0
                    SIG_ACTION_TERM, // SIGHUP
                    SIG_ACTION_TERM, // SIGINT
                    SIG_ACTION_CORE, // SIGQUIT
                    SIG_ACTION_CORE, // SIGILL
                    SIG_ACTION_CORE, // SIGTRAP
                    SIG_ACTION_CORE, // SIGABRT
                    SIG_ACTION_CORE, // SIGBUS
                    SIG_ACTION_CORE, // SIGFPE
                    SIG_ACTION_TERM, // SIGKILL
                    SIG_ACTION_TERM, // SIGUSR1
                    SIG_ACTION_CORE, // SIGSEGV
                    SIG_ACTION_TERM, // SIGUSR2
                    SIG_ACTION_TERM, // SIGPIPE
                    SIG_ACTION_TERM, // SIGALRM
                    SIG_ACTION_TERM, // SIGTERM
                    SIG_ACTION_TERM, // SIGSTKFLT
                    SIG_ACTION_IGN , // SIGCHLD
                    SIG_ACTION_CONT, // SIGCONT
                    SIG_ACTION_STOP, // SIGSTOP
                    SIG_ACTION_STOP, // SIGTSTP
                    SIG_ACTION_STOP, // SIGTTIN
                    SIG_ACTION_STOP, // SIGTTOU
                    SIG_ACTION_IGN , // SIGURG
                    SIG_ACTION_CORE, // SIGXCPU
                    SIG_ACTION_CORE, // SIGXFSZ
                    SIG_ACTION_TERM, // SIGVTALRM
                    SIG_ACTION_TERM, // SIGPROF
                    SIG_ACTION_IGN , // SIGWINCH
                    SIG_ACTION_IGN , // SIGIO
                    SIG_ACTION_IGN , // SIGPWR
                    SIG_ACTION_CORE, // SIGSYS
                    // The rest is filled with zeroes which are equal to SIG_ACTION_TERM
        }};

public:
    static Process* create();
    static Process* create_init_task(void(*procedure)());
    static Process* create_kernel_task(void(*procedure)());
    static Process* create_user_task();
    static Process* clone(Process& proc, uint32_t flags = 0);
    static Process& current()    { return *m_current_process ; }
    static size_t   count()      { return  m_process_count   ; }
    static size_t   highest_pid(){ return  m_processes.size(); }
    static void     task_switch(pid_t pid);
    static void     kill(pid_t pid, int err_code);
    static void     release_zombie(pid_t pid);
    static Process* by_pid(pid_t pid);
    static std::vector<pid_t> process_list();

    static bool check_args_size(const std::vector<kpp::string> &args);

public:
    Process& operator=(Process&&) noexcept = default;
    Process(Process&& other) noexcept = default;

    ~Process();

    void load_user_code(gsl::span<const uint8_t> code_to_copy, size_t allocated_size = 0);
    void set_instruction_pointer(unsigned int value);

    void push_args(const std::vector<kpp::string> &args);

    size_t add_fd(const tasking::FDInfo& info);
    tasking::FDInfo *get_fd(size_t fd);
    void close_fd(size_t fd);

    void wait_for(pid_t pid, int* wstatus);

    // expects that user_regs is a owned pointer !
    // will delete user_regs on call
    void jump_to_user_space();
    void jump_to_user_space(uintptr_t ip);

    void* map_range(uintptr_t phys, size_t len);

    // creates a way to call the kernel by triggering a page fault
    template <typename... Args>
    uintptr_t create_user_callback(const std::function<int(Args...)>& callback);

    uintptr_t create_user_callback_impl(const std::function<int(const std::vector<uintptr_t>&)> &callback, const std::vector<size_t>& arg_sizes);
    void allocate_user_callback_page();

    void raise(pid_t target_pid, int sig, const siginfo_t& siginfo);
    void exit_signal();

    void init_tls();
    void switch_tls();
    void copy_tls(const Process &other);

    bool check_perms(uint16_t perms, uint16_t tgt_uid, uint16_t tgt_gid, uint16_t type);

    uintptr_t allocate_pages(size_t pages, bool map_immediatly = true);
    bool      release_pages(uintptr_t ptr, size_t pages);

private:
    Process();

public:
    pid_t pid { 0 };
    pid_t tgid { 0 };
    pid_t parent { 0 };
    std::unique_ptr<ProcessData> data;
    ProcessArchContext* arch_context { nullptr };
    // data used by scheduler is kept directly in the Process structure
    enum
    {
        Active,
        Paused,
        ChildWait,
        IOWait,
        Sleeping,
        Zombie
    } status = Active;
    status_info status_info;
    int priority { 0 };

private:
    static pid_t find_free_pid();

private:
    void arch_init();
    void init_default_fds();
    void init_sig_handlers();

    uintptr_t stack_pointer() const;
    void expand_stack(size_t size);
    void push_onto_stack(gsl::span<const uint8_t> data);
    void pop_stack(size_t size);

    template <typename T>
    void push_onto_stack(const T& val)
    {
        push_onto_stack(gsl::span{(const uint8_t*)&val, sizeof(T)});
    }
    template <typename T>
    void pop_stack()
    {
        pop_stack(sizeof(T));
    }

    void execute_sighandler(int signal, pid_t returning_pid, const siginfo_t& siginfo);

    bool user_callback_fault_handler(const PageFault& fault);
    void do_user_callback(const std::function<int(const std::vector<uintptr_t>&)>& callback, const std::vector<size_t> &arg_sizes);

    void switch_to();
    void unswitch();

    static void switch_mappings(Process& prev, Process& next);

    void map_code();
    void map_stack();
    void map_shm();

    void create_mappings();
    void release_mappings();

    void map_address_space();
    void unmap_address_space();

    void free_arch_context();
    void cleanup();
    void wake_up(pid_t awakener, int err_code);
    void copy_allocated_pages(Process& target);

private:
    static inline Process* m_current_process { nullptr };
    static inline std::vector<std::unique_ptr<Process>> m_processes;
    static inline size_t m_process_count { 0 };
};

extern "C" void test_task();

#include "process.tpp"

#endif // PROCESS_HPP
