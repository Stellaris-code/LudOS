/*
interface_list.h

Copyright (c) 09 Yann BOUCHER (yann)

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
#ifndef INTERFACE_LIST_H
#define INTERFACE_LIST_H

#include <stdint.h>

#include <drivers/kbd/keys.h>

#include "utils/user_ptr.hpp"

#define ITEST_ID 0xbeef
typedef struct
{
    int(*test)(const char* str);
} itest;

struct FBDevMode
{
    uint32_t width { 0 };
    uint32_t height { 0 };
    uint32_t depth { 0 };
    uint32_t bytes_per_line { 0 };
    uint8_t  red_mask_size { 0 };
    uint8_t  red_field_pos { 0 };
    uint8_t  green_mask_size { 0 };
    uint8_t  green_field_pos { 0 };
    uint8_t  blue_mask_size { 0 };
    uint8_t  blue_field_pos { 0 };
};

#define IFBDEV_ID 0x0000
typedef struct
{
    // returns the number of valid video modes
    int(*get_video_modes)(FBDevMode* buffer, size_t buffer_count);
    int(*get_current_mode)(FBDevMode* mode);
    int(*switch_mode)(int width, int height, int depth);
    uint8_t*(*get_framebuffer)();
} ifbdev;

typedef struct
{
    enum Key state[Key::KeyCount];
} kbd_state;

#define IKBDEV_ID 0x0001
typedef struct
{
    int(*get_kbd_state)(kbd_state*);
} ikbdev;

#endif // INTERFACE_LIST_H
