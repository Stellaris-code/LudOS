/*
vfs.hpp

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
#ifndef VFS_HPP
#define VFS_HPP

#include <stdint.h>

#include <string.hpp>
#include <vector.hpp>
#include <functional.hpp>
#include <variant.hpp>
#include <optional.hpp>
#include <type_traits.hpp>
#include <unordered_map.hpp>

#include "utils/stlutils.hpp"

class vfs
{
public:
    struct node;

    struct file
    {
        uint32_t perms { 0 };
        uint32_t uid { 0 };
        uint32_t gid { 0 };
        uint32_t flags { 0 };
        uint32_t length { 0 }; // TODO : make these functions
        bool is_dir { false };

        std::vector<node> vfs_children;

        virtual size_t read(void* buf, size_t n) const { return 0; }
        virtual size_t write(const void* buf, size_t n) { return 0; }
        virtual std::vector<node> readdir_impl() const { return {}; }
        virtual node* mkdir(const std::string& str) { return nullptr; }
        virtual node* touch(const std::string& str) { return nullptr; }

        std::vector<node> readdir() const
        {
            return merge(vfs_children, readdir_impl());
        }
    };

    struct symlink
    {
        node* link;
    };

    struct node
    {
        std::string name;
        // TODO : consider unique_ptr ?
        std::variant<std::shared_ptr<file>, symlink> data;

        bool is_dir() const
        {
            return get_file().is_dir;
        }

        template <typename T = file>
        T& get_file()
        {
            if (auto target = std::get_if<std::shared_ptr<file>>(&data))
            {
                if constexpr (std::is_same_v<T, file>)
                {
                    if ((*target) == nullptr) abort();
                    return *(*target);
                }
                else
                {
                    auto ptr = dynamic_cast<T*>(target->get());
                    if (ptr == nullptr) abort();
                    return *ptr;
                }
            }
            else
            {
                return std::get<symlink>(data).link->get_file<T>();
            }
        }

        template <typename T = file>
        const T& get_file() const
        {
            if (auto target = std::get_if<std::shared_ptr<file>>(&data))
            {
                if constexpr (std::is_same_v<T, file>)
                {
                    if ((*target) == nullptr) abort();
                    return *(*target);
                }
                else
                {
                    auto ptr = dynamic_cast<T>(*target);
                    if (ptr == nullptr) abort();
                    return *ptr;
                }
            }
            else
            {
                return std::get<symlink>(data).link->get_file<T>();
            }
        }

        file* operator->()
        {
            return &get_file();
        }
        const file* operator->() const
        {
            return &get_file();
        }
    };

    static size_t new_descriptor(const vfs::node& node);

public:

    static void init();

    static void mount_dev();

    static std::optional<vfs::node> find(const std::string& path);

    static bool mount(const vfs::node& node, const std::string& mountpoint);

    static void traverse(const vfs::node& node, size_t indent = 0);
    static void traverse(const std::string& path);

    static inline std::vector<node> descriptors;

    static inline vfs::node root;
};

#endif // VFS_HPP
