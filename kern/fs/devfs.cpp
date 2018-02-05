/*
devfs.cpp

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

#include "devfs.hpp"

#include "vfs.hpp"

#include "utils/messagebus.hpp"
#include "drivers/kbd/keyboard.hpp"
#include "utils/nop.hpp"
#include "drivers/storage/disk.hpp"

namespace devfs
{

struct stdout_file : public vfs::node
{
    virtual size_t write(const void* buf, size_t n) override
    {
        auto str = std::string(reinterpret_cast<const char*>(buf), n);
        kprintf("%s", str.c_str());
        return n;
    }
};
struct stderr_file : public vfs::node
{
    virtual size_t write(const void* buf, size_t n) override
    {
        auto str = std::string(reinterpret_cast<const char*>(buf), n);

        err("%s", str.c_str());
        return n;
    }
};

struct stdin_file : public vfs::node
{
    virtual size_t read(void* data, size_t size) const override
    {
        std::vector<uint8_t> buf;

        auto handl = MessageBus::register_handler<kbd::TextEnteredEvent>([&buf](const kbd::TextEnteredEvent& e)
        {
            buf.push_back(e.c);
        });

        while (buf.size() < size) { nop(); };

        MessageBus::remove_handler(handl);

        for (size_t i { 0 }; i < buf.size(); ++i)
        {
            reinterpret_cast<uint8_t*>(data)[i] = buf[i];
        }

        return buf.size();
    }
};

struct disk_file : public vfs::node
{
    disk_file(Disk& disk, const std::string& node_name)
        : m_disk(disk)
    {
        m_name = node_name;
    }

    virtual size_t size() const override { return m_disk.disk_size(); }
    virtual bool is_dir() const override { return false; }
    virtual size_t read(void* buf, size_t size) const override
    {
        auto data = m_disk.read(0, size);
        std::copy(data.begin(), data.end(), (uint8_t*)buf);

        return data.size();
    }
    virtual size_t write(const void* buf, size_t n) override
    {
        uint8_t* ptr = (uint8_t*)buf;
        auto data = std::vector<uint8_t>(ptr, ptr + n);
        m_disk.write(0, data);
    }

    private:
        Disk& m_disk;
};

void init()
{
    auto devfs_root = std::make_shared<vfs::node>(vfs::root.get());
    devfs_root->m_is_dir = true;
    vfs::mount(devfs_root, "/dev");

    auto stdin_node = std::make_shared<stdin_file>();
    stdin_node->rename("stdin");

    vfs::new_descriptor(*stdin_node);
    if (!mount(stdin_node, "/dev"))
    {
        warn("Can't mount '/dev/stdin'\n");
    }

    auto stdout_node = std::make_shared<stdout_file>();
    stdout_node->rename("stdout");

    vfs::new_descriptor(*stdout_node);
    if (!mount(stdout_node, "/dev"))
    {
        warn("Can't mount '/dev/stdout'\n");
    }

    auto stderr_node = std::make_shared<stderr_file>();
    stderr_node->rename("stderr");

    vfs::new_descriptor(*stderr_node);
    if (!mount(stderr_node, "/dev"))
    {
        warn("Can't mount '/dev/stderr'\n");
    }

    // add existing drives
    for (Disk& disk : Disk::disks())
    {
        detail::add_drive(disk);
    }

    MessageBus::register_handler<DiskFoundEvent>([](const DiskFoundEvent& ev)
    {
        detail::add_drive(ev.disk);
    });
}

namespace detail
{

void add_drive(Disk &disk)
{
    std::string prefix;
    static std::array<std::string, Disk::Unknown + 1> suffixes;
    if (suffixes[0].empty()) suffixes.fill("a");

    switch (disk.media_type())
    {
        case Disk::Floppy:
            prefix = "fd"; break;
        case Disk::OpticalDisk:
            prefix = "od"; break;
        case Disk::NetworkDrive:
            prefix = "nd"; break;
        case Disk::MemoryCard:
            prefix = "mmc"; break;
        case Disk::RamDrive:
            prefix = "md"; break;
        case Disk::HardDrive:
        default:
            prefix = "sd"; break;
    }

    auto node = std::make_shared<disk_file>(disk, prefix + suffixes[disk.media_type()]);
    vfs::mount(node, "/dev");

    if (suffixes[disk.media_type()].back() != 'z')
    {
        ++suffixes[disk.media_type()].back();
    }
    else
    {
        suffixes[disk.media_type()] += "a";
    }
}
}

}
