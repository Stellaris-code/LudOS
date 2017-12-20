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

#include "fs/fat.hpp"
#include "fs/tar.hpp"
#include "fs/vfs.hpp"
#include "fs/mbr.hpp"
#include "fs/pathutils.hpp"

#include "drivers/kbd/kbd_mappings.hpp"
#include "drivers/kbd/text_handler.hpp"
#include "drivers/kbd/keyboard.hpp"
#include "drivers/kbd/led_handler.hpp"
#include "drivers/mouse/mouse.hpp"
#include "drivers/storage/diskinterface.hpp"

#include "power/powermanagement.hpp"

#include "terminal/terminal.hpp"
#include "terminal/escape_code_macros.hpp"

#include "elf/symbol_table.hpp"

#include "graphics/video.hpp"
#include "graphics/drawing/display_draw.hpp"
#include "graphics/fonts/psf.hpp"
#include "graphics/text/graphicterm.hpp"
#include "graphics/drawing/image_loader.hpp"

#include "utils/logging.hpp"
#include "utils/messagebus.hpp"
#include "utils/memutils.hpp"
#include "utils/virt_machine_detect.hpp"
#include "utils/debug.hpp"
#include "utils/crc32.hpp"
#include "halt.hpp"

#include "shell/shell.hpp"
#include "shell/commands/basecommands.hpp"
#include "shell/commands/syscommands.hpp"
#include "shell/commands/fscommands.hpp"
#include "shell/commands/gfxcommands.hpp"

#include "time/time.hpp"
#include "time/timer.hpp"

#include "initrd/initrd.hpp"

// TODO : FAT32 write
// TODO : system calls
// TODO : user mode
// TODO : POC calculatrice
// TODO : windowing system avec alloc d'un canvas etc
// TODO : Son
// TODO : Passer en IDE PCI : IDE UDMA
// FIXME : revoir l'architecture dégeulasse de l'ownership des nodes de readdir
// TODO : TinyGL
// TODO : ambilight feature pour le windowing system !
// TODO : refaire une VRAIE classe Terminal... c'est atroce l'implémentation actuelle, une horreur lovecraftienne
// TODO : classe virtuelle FS ave constructor/destructor pour notifier de la création/desctruction d'une partition
// TODO : pas de vfs_children, on peut faire autrement (devfs, ...)
// TODO : écran de veille ala windows
// TODO : Ext2
// TODO : PAE
// TODO : don't forget about fpu state
// NOTE : un static int[256] peut tout foutre en l'air, attention aux static
// TODO : AddressSanitizer

void global_init()
{
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
            else if (e.key == kbd::Delete && Keyboard::ctrl() && Keyboard::alt())
            {
                reset();
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

    vfs::init();
    vfs::mount_dev();

    log(Info, "Available drives : %zd\n", DiskInterface::drive_count());

#if 1
    for (size_t disk { 0 }; disk < DiskInterface::drive_count(); ++disk)
    {
        log(Info, "Disk : %zd\n", disk);
        for (auto partition : mbr::read_partitions(disk))
        {
            log(Info, "Partition %d\n", partition.partition_number);
            auto fs = fat::read_fat_fs(disk, partition.relative_sector, true);
            auto wrapper = fat::RAIIWrapper(fs);
            if (fs.valid)
            {
                log(Info, "FAT %zd filesystem found on drive %zd, partition %d\n", fs.type, fs.drive, partition.partition_number);

                static auto root = fat::root_dir(fs);

                vfs::mount(root, "/boot");
            }
        }
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

    sh.command("run /initrd/init.sh");

    sh.run();
}
