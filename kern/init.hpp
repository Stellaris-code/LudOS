/*
arch_indep_init.hpp

Copyright (c) 27 Yann BOUCHER (yann)

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
#ifndef ARCH_INDEP_INIT_HPP
#define ARCH_INDEP_INIT_HPP

#include "fs/fat.hpp"
#include "fs/vfs.hpp"
#include "fs/mbr.hpp"
#include "fs/pathutils.hpp"

#include "misc/greet.hpp"

#include "drivers/kbd/kbd_mappings.hpp"
#include "drivers/kbd/text_handler.hpp"
#include "drivers/kbd/keyboard.hpp"
#include "drivers/kbd/led_handler.hpp"
#include "drivers/mouse/mouse.hpp"
#include "drivers/diskinterface.hpp"

#include "power/powermanagement.hpp"

#include "terminal/terminal.hpp"

#include "utils/logging.hpp"
#include "utils/messagebus.hpp"

#include "time/time.hpp"

inline void init()
{
    vfs::init();

    log("Available drives : %zd\n", DiskInterface::drive_count());

    for (size_t disk { 0 }; disk < DiskInterface::drive_count(); ++disk)
    {
        log("Disk : %zd\n", disk);
        for (auto partition : mbr::read_partitions(disk))
        {
            log("Partition %zd\n", partition.partition_number);
            auto fs = fat::read_fat_fs(disk, partition.relative_sector);
            if (fs.valid)
            {
                log("FAT %zd filesystem found on drive %zd, partition %d\n", (size_t)fs.type, fs.drive, partition.partition_number);

                vfs::mount(fat::root_dir(fs), "/boot");

                vfs::mount_dev();

                vfs::traverse("/");

                std::optional<vfs::node> file = vfs::find("/boot/test.txt");

                std::string str = __TIME__ "\n";

                file->get_file().write(str.data(), str.size());

                std::vector<uint8_t> vec;
                vec.resize(file->get_file().length);

                file->get_file().read(vec.data(), vec.size());

                for (auto c : vec)
                {
                    putchar(c);
                }
            }
            else
            {
                warn ("No FAT fs found on drive %zd, partition %d\n", fs.drive, partition.partition_number);
            }

        }
    }

    greet();

    kbd::install_mapping(kbd::mappings::azerty());
    kbd::TextHandler::init();
    kbd::install_led_handler();

    MessageBus::register_handler<kbd::TextEnteredEvent>([](const kbd::TextEnteredEvent& e)
    {
        putchar(e.c);
    });

    MessageBus::register_handler<kbd::KeyEvent>([](const kbd::KeyEvent& e)
    {
        if (e.state == kbd::KeyEvent::Pressed)
        {
            if (e.key == kbd::PageUp)
            {
                term->scroll_history(+10);
            }
            else if (e.key == kbd::PageDown)
            {
                term->scroll_history(-10);
            }
            else if (e.key == kbd::Delete && Keyboard::ctrl() && Keyboard::alt())
            {
                reset();
            }
        }
    });

    Mouse::init();

    MessageBus::register_handler<MouseScrollEvent>([](const MouseScrollEvent& e)
    {
        if (e.wheel>0)
        {
            term->scroll_history(-3);
        }
        else if (e.wheel<0)
        {
            term->scroll_history(6);
        }
    });
}

#endif // ARCH_INDEP_INIT_HPP
