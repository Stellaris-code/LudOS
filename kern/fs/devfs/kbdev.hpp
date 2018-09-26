/*
kbdev.hpp

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
#ifndef KBDEV_HPP
#define KBDEV_HPP

#include "fs/interface.hpp"

#include <sys/interface_list.h>

#include "utils/kmsgbus.hpp"

#include "utils/user_ptr.hpp"

struct kbdev_node : public vfs::interface_node<kbdev_node, vfs::ientry<ikbdev, IKBDEV_ID>>
{
    kbdev_node(node* parent);

    template<typename Interface>
    void fill_interface(Interface*) const
    {}

    // TODO : map pointers to user_ptr
    int get_kbd_state(kbd_state*) const;

    size_t keyboard_id;
    MessageBus::RAIIHandle msg_hdl;
    bool key_state[KeyCount];
};

template <>
inline void kbdev_node::fill_interface<ikbdev>(ikbdev* interface) const
{
    register_callback(&kbdev_node::get_kbd_state, interface->get_kbd_state);
}

#endif // KBDEV_HPP
