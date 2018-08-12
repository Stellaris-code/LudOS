/*
fbdev.hpp

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
#ifndef FBDEV_HPP
#define FBDEV_HPP

#include "fs/interface.hpp"

#include <sys/interface_list.h>

struct fbdev_node : public vfs::interface_node<fbdev_node, vfs::ientry<ifbdev, IFBDEV_ID>>
{
    using interface_node::interface_node;

    template<typename Interface>
    void fill_interface(Interface*) const
    {}

    int get_video_modes(FBDevMode* buffer, size_t buffer_count) const;
    int get_current_mode(FBDevMode* mode) const;
    int switch_mode(int width, int height, int depth) const;
    uint8_t* get_framebuffer() const;
};

template <>
inline void fbdev_node::fill_interface<ifbdev>(ifbdev* interface) const
{
    register_callback(&fbdev_node::get_video_modes, interface->get_video_modes);
    register_callback(&fbdev_node::get_current_mode, interface->get_current_mode);
    register_callback(&fbdev_node::switch_mode, interface->switch_mode);
    register_callback(&fbdev_node::get_framebuffer, interface->get_framebuffer);
}

#endif // FBDEV_HPP
