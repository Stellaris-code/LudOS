/*
global_init.cpp

Copyright (c) 12 Yann BOUCHER (yann)

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

#include "global_init.hpp"

#include "fs/vfs.hpp"
#include "fs/devfs/devfs.hpp"
#include "fs/procfs/procfs.hpp"
#include "fs/fs.hpp"
#include "fs/mbr/mbr.hpp"
#include "fs/pathutils.hpp"

#include "drivers/kbd/keyboard.hpp"
#include "drivers/kbd/led_handler.hpp"
#include "drivers/mouse/mouse.hpp"
#include "drivers/sound/beep.hpp"

#include "power/powermanagement.hpp"

#include "terminal/terminal.hpp"
#include "terminal/escape_code_macros.hpp"

#include "elf/symbol_table.hpp"

#include "graphics/video.hpp"
#include "graphics/drawing/display_draw.hpp"
#include "graphics/fonts/psf.hpp"
#include "graphics/text/graphicterm.hpp"

#include "utils/logging.hpp"
#include "utils/messagebus.hpp"

#include "halt.hpp"

#include "shell/shell.hpp"
#include "shell/commands/base/basecommands.hpp"
#include "shell/commands/sys/syscommands.hpp"
#include "shell/commands/fs/fscommands.hpp"
#include "shell/commands/graphics/gfxcommands.hpp"
#include "shell/commands/network/netcommands.hpp"
#include "shell/commands/tasking/taskcommands.hpp"

#include "time/time.hpp"
#include "time/timer.hpp"

#include "syscalls/syscalls.hpp"
#include "tasking/process.hpp"
#include "tasking/scheduler.hpp"

#include "initrd/initrd.hpp"

#include "info/version.hpp"

// TODO : system calls
// TODO : user mode
// TODO : POC calculatrice
// TODO : windowing system avec alloc d'un canvas etc
// TODO : Son
// FIXME : revoir l'architecture dégeulasse de l'ownership des nodes de readdir
// TODO : TinyGL
// TODO : ambilight feature pour le windowing system !
// TODO : refaire une VRAIE classe Terminal... c'est atroce l'implémentation actuelle, une horreur lovecraftienne
// TODO : écran de veille ala windows
// TODO : PAE
// TODO : passer AHCI en PciDriver
// TODO : VirtIO drivers
// TODO : BASIC interpreter
// TODO : cache bu sec/count pair ?
// TODO : vfs sanitize names
// TODO : vfs canonicalize
// TODO : process : free pages and alloc only at execute time
// TODO : task switch : fpu state

// ROADMAP
// : supprimer la libc++ & libcxxabi
// : supprimer les includes inutiles
// : passer le shell et un max de trucs en userspace
// : QueryInterface
// : DWARF ?
// : SVGA-II
// : faire une boucle de traitement de callbacks d'interruption
// : implémenter /dev/input
// : vfs call qui liste des properties particulières et une liste possible d'appels
// : implémenter signaux
// : implémenter expanding stack
// : implémenter sigsegv

/**********************************/
// BUGS
// * BUG : IDE PIO seems to be not working, investigate this
/**********************************/

/**********************************/
// AWFUL HACKS
// * early pic eoi to allow dma during kbd interrupt !
// --> should be removed with tasking
/**********************************/

/**********************************/
// NOTES
// * If someting doesn't work :
// --> SSE and FPU state between interrupts !!
// --> ctors ?
// --> any impl in MessageBus
// * don't forget about fpu state
/**********************************/

void global_init()
{
    int register esp asm("esp");
    if ((esp & 0xF) != 0 && false)
    {
        panic("Stack is not aligned on a 16-byte boundary !");
    }

    beep(200);

    power::init_power_management();

    kbd::install_mapping(kbd::mappings::azerty());
    kbd::TextHandler::init();
    kbd::install_led_handler();

    MessageBus::register_handler<kbd::TextEnteredEvent>([](const kbd::TextEnteredEvent& e)
    {
        term().add_input(e.c);
        term().force_redraw_input();
    });

    MessageBus::register_handler<kbd::KeyEvent>([](const kbd::KeyEvent& e)
    {
        if (e.state == kbd::KeyEvent::Pressed)
        {
            if (e.key == kbd::PageUp)
            {
                term().scroll_history(-10);
            }
            else if (e.key == kbd::PageDown)
            {
                term().scroll_history(+10);
            }
            else if (e.key == kbd::Right)
            {
                term().move_cursor(+1);
            }
            else if (e.key == kbd::Left)
            {
                term().move_cursor(-1);
            }
            else if (e.key == kbd::Delete && Keyboard::ctrl() && Keyboard::alt())
            {
                MessageBus::send(ResetMessage{});
            }
            else if (e.key == kbd::Delete)
            {
                term().forward_delete();
                term().force_redraw_input();
            }
            else if (e.key == kbd::P && Keyboard::ctrl() && Keyboard::alt())
            {
                panic("Panic key pressed\n");
            }
        }
    });

    Mouse::init();

    MessageBus::register_handler<MouseScrollEvent>([](const MouseScrollEvent& e)
    {
        if (e.wheel>0)
        {
            term().scroll_history(+3);
        }
        else if (e.wheel<0)
        {
            term().scroll_history(-3);
        }
    });

    Disk::system_init();

    vfs::init();
    devfs::init();
    procfs::init();

    log(Info, "Available drives : %zd\n", Disk::disks().size());

#if 1
    // Detect disk partitions
    size_t disk_amnt = Disk::disks().size();
    for (size_t disk { 0 }; disk < disk_amnt; ++disk)
    {
        mbr::read_partitions(Disk::disks()[disk]);
    }
#endif

    if (!install_initrd())
    {
        panic("Cannot install initrd!\n");
    }

    MessageBus::send<kbd::KeyEvent>(kbd::KeyEvent{kbd::NumLock, kbd::KeyEvent::Pressed});
    MessageBus::send<kbd::KeyEvent>(kbd::KeyEvent{kbd::NumLock, kbd::KeyEvent::Released});

    Shell sh;
    sh.params.prompt = ESC_BG(13,132,203) "  LudOS " ESC_POP_COLOR ESC_BG(78,154,6) ESC_FG(13,132,203) "▶" ESC_POP_COLOR " :{path}> " ESC_POP_COLOR
            ESC_FG(78,154,6) "▶" ESC_POP_COLOR " ";
    install_base_commands(sh);
    install_sys_commands(sh);
    install_fs_commands(sh);
    install_gfx_commands(sh);
    install_net_commands(sh);
    install_task_commands(sh);

    MemBuffer buf(512*16);
    MemoryDisk::create_disk(buf.data(), buf.size(), "scratch");

    init_syscalls();

    tasking::scheduler_init();

    sh.command("run /initrd/init.sh");

    sh.run();
}
