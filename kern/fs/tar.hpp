/*
tar.hpp

Copyright (c) 05 Yann BOUCHER (yann)

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
#ifndef TAR_HPP
#define TAR_HPP

#include "vfs.hpp"

#include <vector.hpp>
#include <type_traits.hpp>
#include <optional.hpp>

#include <string.h>

namespace tar
{

struct tar_node;

class TarFS
{
    friend struct tar_node;
public:
    TarFS(const std::vector<uint8_t>& file);

    tar_node root_dir() const;

private:
    struct Header
    {                              /* byte offset */
        char name[100];               /*   0 */
        char mode[8];                 /* 100 */
        char uid[8];                  /* 108 */
        char gid[8];                  /* 116 */
        char size[12];                /* 124 */
        char mtime[12];               /* 136 */
        char chksum[8];               /* 148 */
        char typeflag;                /* 156 */
        char linkname[100];           /* 157 */
        char magic[6];                /* 257 */
        char version[2];              /* 263 */
        char uname[32];               /* 265 */
        char gname[32];               /* 297 */
        char devmajor[8];             /* 329 */
        char devminor[8];             /* 337 */
        char prefix[155];             /* 345 */
        char padding[12];             /* 500 */
    };
    static_assert(sizeof(Header) == 512);

    std::optional<tar_node> read_header(const Header* hdr) const;

    std::vector<tar_node> read_dir(const uint8_t* addr, size_t size) const;
private:

    bool check_sum(const Header* hdr) const;

    template <typename T>
    size_t read_number(T&& str) const
    {
        using type = std::remove_reference_t<T>;
        static_assert(std::is_array_v<type>, "Must use const char[] !");

        char zstr[std::extent_v<type>+1] = {0};
        strncpy(zstr, str, std::extent_v<type>);
        return strtoull(zstr, nullptr, 8);
    }

private:
   std::vector<uint8_t> m_file;
};

struct tar_node : public vfs::node
{
    tar_node(const TarFS& fs)
        : m_fs(fs)
    {}

    virtual size_t read(void* buf, size_t bytes) const override
    {
        if (is_dir())
        {
            return 0;
        }

        size_t amnt = std::min(bytes, size());
        memcpy(buf, m_data_addr, amnt);

        return amnt;
    }

    virtual std::vector<std::shared_ptr<vfs::node>> readdir_impl() override
    {
        if (!is_dir())
        {
            return {};
        }

        auto nodes = m_fs.read_dir(m_data_addr, size());
        return map<tar_node, std::shared_ptr<node>>(nodes, [](const tar_node& file)->std::shared_ptr<node>
        {
            return std::make_shared<tar_node>(file);
        });
    }

    virtual size_t size() const override { return m_size; }

    const TarFS& m_fs;
    const uint8_t* m_data_addr { nullptr };
    size_t m_size { 0 };
private:
};

}

#endif // TAR_HPP
