/*
vfs.cpp

Copyright (c) 17 Yann BOUCHER (yann)

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

#include "vfs.hpp"

#include "utils/logging.hpp"

//#include "i686/pc/devices/ps2keyboard.hpp"

//void vfs::init()
//{
//    descriptors.resize(3); // for std streams

//    node stdin_node;
//    file stdin;
//    stdin_node.filename = "/dev/stdin";
//    stdin.read = [](void* data, size_t size)->size_t
//    {
//        std::vector<uint8_t> buf;

//        Keyboard::char_handlers.emplace_back([&buf, size](uint8_t c)
//        {
//            buf.push_back(c);
//        });

//        while (buf.size() < size);

//        Keyboard::char_handlers.pop_back();

//        for (size_t i { 0 }; i < buf.size(); ++i)
//        {
//            reinterpret_cast<uint8_t*>(data)[i] = buf[i];
//        }

//        return buf.size();
//    };
//    stdin_node.data = stdin;

//    descriptors[0] = stdin_node;

//    node stdout_node;
//    file stdout;
//    stdout_node.filename = "/dev/stdout";
//    stdout.write = [](const void* data, size_t size)->size_t
//    {
//        std::string str(reinterpret_cast<const char*>(data), size);

//        kprintf("%s", str.c_str());

//        return size;
//    };
//    stdout_node.data = stdout;

//    descriptors[1] = stdout_node;

//    node stderr_node;
//    file stderr;
//    stderr_node.filename = "/dev/stderr";
//    stderr.write = [](const void* data, size_t size)->size_t
//    {
//        std::string str(reinterpret_cast<const char*>(data), size);

//        err("%s", str.c_str());

//        return size;
//    };
//    stderr_node.data = stderr;

//    descriptors[2] = stderr_node;

//    log("VFS initialized.\n");
//}
