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
#include "diskinterface.hpp"

#include "greet.hpp"

#include "messagebus.hpp"
#include "drivers/kbd/kbd_mappings.hpp"
#include "drivers/kbd/text_handler.hpp"
#include "drivers/kbd/keyboard.hpp"
#include "drivers/kbd/led_handler.hpp"

#include "drivers/mouse/mouse.hpp"

#include "powermanagement.hpp"

#include "terminal/terminal.hpp"

#include "utils/logging.hpp"

void arch_indep_init()
{
    //vfs::init();

    DiskInterface::scan();

    auto fs = fat::read_fat_fs(0, 0);
    if (fs.valid)
    {
        log("FAT %zd filesystem found on drive %zd\n", (size_t)fs.type, fs.drive);

        auto entries = fat::root_entries(fs);
        log("%zd\n", entries.size());
        for (const auto& entry : entries)
        {
            auto& file = entry.get_file();
            std::vector<uint8_t> data(file.length);
            file.read(data.data(), data.size());
            data.push_back('\0'); // sentinel value
            log("name : %s\n%s\n", entry.filename.data(), data.data());
        }
    }
    else
    {
        warn ("No FAT fs found\n");
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
                Terminal::scroll_history(+10);
            }
            else if (e.key == kbd::PageDown)
            {
                Terminal::scroll_history(-10);
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
            Terminal::scroll_history(-3);
        }
        else if (e.wheel<0)
        {
            Terminal::scroll_history(6);
        }
    });
}

#endif // ARCH_INDEP_INIT_HPP
