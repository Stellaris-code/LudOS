/*
main.cpp

Copyright (c) 11 Yann BOUCHER (yann)

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

#include <stdint.h>

#include <syscalls/syscall_list.hpp>

#include <sys/fnctl.h>
#include <sys/interface_list.h>
#include <errno.h>
#include <stdio.h>

int kbd_fd;
ikbdev kbd_interface;
kbd_state key_state;

void update_key_state()
{
    static enum Key previous[KeyCount];
    memcpy(previous, key_state.state, sizeof(key_state.state));

    kbd_interface.get_kbd_state(&key_state);

    for (size_t i = 0; i < KeyCount; ++i)
    {
        if (previous[i] != key_state.state[i])
        {
            if (i >= KeyA && i <= KeyZ)
            {
                fprintf(stderr, "Pressed key %c !\n", 'a' + i - KeyA);
            }
        }
    }
}

int main(int argc, char* argv[])
{
    int fd = open("/proc/interface_test", O_RDONLY, 0);
    if (fd == -1)
    {
        perror("open()");
        return 1;
    }

    itest interface;
    int ret = get_interface(fd, ITEST_ID, &interface);
    if (ret == -1)
    {
        perror("get_interface()");
        return 2;
    }

    fprintf(stderr, "address is : %p\n", interface.test);

    ret = interface.test("working!");

    printf("The call returned %d\n", ret);

    kbd_fd = open("/dev/kbd0", O_RDONLY, 0);
    assert(kbd_fd != -1);
    int kbd_ret = get_interface(kbd_fd, IKBDEV_ID, &kbd_interface);
    assert(kbd_ret != -1);

    int fbdev_fd = open("/dev/fbdev", O_RDWR, 0);
    assert(fbdev_fd != -1);
    ifbdev fbdev;

    int fbret = get_interface(fbdev_fd, IFBDEV_ID, &fbdev);
    assert(fbret != -1);

    //    FBDevMode modes[64];
    //    int count = fbdev.get_video_modes(modes, 64);


    printf("Switching to 640x480x32\n");
    fbdev.switch_mode(640, 480, 32);

    FBDevMode current;
    fbdev.get_current_mode(&current);
    printf("Current mode : %dx%dx%d\n", current.width, current.height, current.depth);

    uint8_t* fb = fbdev.get_framebuffer();
    printf("Address : %p\n", fb);

    while (true)
    {
        for (size_t i { 0 }; i < current.bytes_per_line*current.height/4; ++i)
        {
            ((uint32_t*)fb)[i] = 0x0000ff; // blue
        }
        update_key_state();
    }

    while (true) {}

    return 0;
}
