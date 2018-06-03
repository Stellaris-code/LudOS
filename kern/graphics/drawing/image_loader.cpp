/*
image_loader.cpp

Copyright (c) 21 Yann BOUCHER (yann)

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

#include "image_loader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC kmalloc
#define STBI_FREE(x) if (x) kfree(x)
#define STBI_REALLOC krealloc
#define STBI_NO_STDIO
#include "stb/stb_image.h"

#include <kstring/kstring.hpp>

#include "utils/crc32.hpp"

#include "fs/fsutils.hpp"
#include "fs/vfs.hpp"

#include "utils/logging.hpp"

namespace graphics
{

kpp::optional<Bitmap> load_image(const kpp::string &path)
{
    auto node = vfs::find(path);

    if (!node)
    {
        return {};
    }

    auto result = node.value()->read();
    if (!result) return {};

    auto data = std::move(result.value());

    if (data.empty())
    {
        return {};
    }

    int x, y, depth;
    uint8_t* img = stbi_load_from_memory(data.data(), data.size(), &x, &y, &depth, 4);

    if (!img)
    {
        warn("Failed loading '%s' : %s\n", path.c_str(), stbi_failure_reason());
        return {};
    }

    Bitmap bmp(x, y);
    memcpyl(bmp.data(), img, x*y*depth);

    for (size_t i { 0 }; i < static_cast<size_t>(x); ++i)
    {
        for (size_t j { 0 }; j < static_cast<size_t>(y); ++j)
        {
            bmp[{i, j}] = Color(bmp[{i, j}].b, bmp[{i, j}].g, bmp[{i, j}].r, bmp[{i, j}].a);
        }
    }

    stbi_image_free(img);

    return bmp;
}

}
