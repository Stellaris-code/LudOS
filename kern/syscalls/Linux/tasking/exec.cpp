/*
exec.cpp

Copyright (c) 25 Yann BOUCHER (yann)

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

#include "syscalls/syscalls.hpp"

#include "tasking/process.hpp"
#include "tasking/process_data.hpp"
#include "tasking/loaders/process_loader.hpp"
#include "mem/meminfo.hpp"
#include "fs/fsutils.hpp"
#include "fs/pathutils.hpp"
#include "fs/vfs.hpp"
#include "drivers/storage/disk.hpp"
#include "mem/meminfo.hpp"
#include "utils/memutils.hpp"
#include "utils/align.hpp"

#include <errno.h>

extern "C" void enter_ring3(uint32_t esp, uint32_t eip, uint32_t argc, uint32_t argv);

std::vector<kpp::string> args;
Process* process { nullptr };

// TODO : ETXTBSY
// TODO : envp
int sys_execve(user_ptr<const char> path, user_ptr<user_ptr<const char>> argv, user_ptr<user_ptr<const char>> envp)
{
    if (!path.check() || !argv.check() || !envp.check())
    {
        return -EFAULT;
    }

    {
        auto res = vfs::user_find(path.get());
        if (res.target_node == nullptr)
        {
            return -ENOENT;
        }
        if (res.target_node->type() != vfs::node::File)
        {
            return -ENOENT;
        }

        if (!Process::current().check_perms(res.target_node->stat().perms, res.target_node->stat().uid,
                                            res.target_node->stat().gid, Process::AccessRequestPerm::ExecRequest))
        {
            return -EACCES;
        }

        args.clear();
        user_ptr<const char>* str = argv.get();
        if (!str->check()) return -EFAULT;
        while (str->get())
        {
            args.emplace_back(str->get());
            str++;
            if (!str->check()) return -EFAULT;
        }

        if (!Process::check_args_size(args))
        {
            return -E2BIG;
        }

        auto result = res.target_node->read();
        if (!result)
        {
            return -result.error().to_errno();
        }

        MemBuffer data = std::move(result.value());

        if (data.empty())
        {
            return -EIO;
        }

        auto loader = ProcessLoader::get(data);
        if (!loader)
        {
            return -ENOEXEC;
        }

        kpp::string proc_name = path.get();

        process = &Process::current();

        process->unswitch();
        loader->load(*process);

        process->set_args(args);
        process->data->name = filename(proc_name);
    }

    // Force scope deletion, otherwise it will never be called
    process->switch_to();

    __builtin_unreachable();
}
