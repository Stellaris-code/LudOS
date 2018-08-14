/*
kbdev.cpp

Copyright (c) 13 Yann BOUCHER (yann)

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

#include "kbdev.hpp"

#include "drivers/kbd/kbd_mappings.hpp"

#include "utils/messagebus.hpp"
#include "utils/logging.hpp"

kbdev_node::kbdev_node(vfs::node *parent)
    : interface_node(parent)
{
    memset(key_state, false, sizeof(key_state));

    msg_hdl = MessageBus::register_handler<kbd::KeyEvent>([this](const kbd::KeyEvent& event)
    {
        if (event.kbd_id == keyboard_id)
        {
            key_state[event.key] = (event.state == kbd::KeyEvent::Pressed);
        }
    });
}

int kbdev_node::get_kbd_state(kbd_state * state) const
{
    memcpy(state->state, this->key_state, sizeof(key_state));

    return EOK;
}
