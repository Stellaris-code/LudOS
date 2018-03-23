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

#include "fs/vfs.hpp"

#include <deque.hpp>

#include "utils/messagebus.hpp"
#include "drivers/kbd/keyboard.hpp"
#include "utils/nop.hpp"
#include "drivers/storage/disk.hpp"
#include "tasking/process.hpp"

namespace devfs
{

struct stdout_file : public vfs::node
{
    using node::node;

protected:
    [[nodiscard]] virtual bool write_impl(size_t offset, gsl::span<const uint8_t> data) override
    {
        auto str = std::string((const char*)data.data(), data.size());
        kprintf("%s", str.c_str());
        return true;
    }
};
struct stderr_file : public vfs::node
{
    using node::node;
protected:
    [[nodiscard]] virtual bool write_impl(size_t offset, gsl::span<const uint8_t> data) override
    {
        auto str = std::string((const char*)data.data(), data.size());
        err("%s", str.c_str());
        return true;
    }
};

struct stdin_file : public vfs::node
{
    using node::node;

public:
    stdin_file(node* parent = nullptr)
        : node(parent)
    {
        m_handl = MessageBus::register_handler<kbd::TextEnteredEvent>([this](const kbd::TextEnteredEvent& e)
        {
            if (Process::enabled())
            {
                m_input_buffer.push_back(e.c);
            }
        });
    }

protected:
    [[nodiscard]] virtual MemBuffer read_impl(size_t offset, size_t size) const override
    {
        MemBuffer buf;

        while (m_input_buffer.size() < size) { wait_for_interrupts(); };

        for (size_t i { 0 }; i < size; ++i)
        {
            buf.emplace_back(m_input_buffer.front());
            m_input_buffer.pop_front();
        }

        assert(buf.size() == size);

        return buf;
    }

private:
    MessageBus::RAIIHandle m_handl;
    mutable std::deque<uint8_t> m_input_buffer;
};

struct devfs_root : public vfs::node
{
    using node::node;
public:
    virtual Type type() const override { return Directory; }
    virtual std::vector<std::shared_ptr<node>> readdir_impl() override { return children; }

    std::vector<std::shared_ptr<node>> children;
};

std::shared_ptr<devfs_root> root;

void init()
{
    auto dir = vfs::root->create("dev", vfs::node::Directory);

    root = std::make_shared<devfs_root>(vfs::root.get());
    vfs::mount(root, dir);

    auto stdin_node = std::make_shared<stdin_file>(root.get());
    stdin_node->rename("stdin");

    root->children.emplace_back(stdin_node);

    auto stdout_node = std::make_shared<stdout_file>(root.get());
    stdout_node->rename("stdout");

    root->children.emplace_back(stdout_node);

    auto stderr_node = std::make_shared<stderr_file>(root.get());
    stderr_node->rename("stderr");

    root->children.emplace_back(stderr_node);

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

std::string drive_label(const Disk& disk)
{
    static std::map<std::string, std::string> label_mappings;
    static std::map<std::string, size_t> disk_partitions;
    static std::array<std::string, Disk::Unknown + 1> suffixes;
    if (suffixes[0].empty()) suffixes.fill("a");

    std::string prefix;
    std::string name;

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

    if (disk.is_partition())
    {
        // TODO : GUID
        auto parent_name = static_cast<const DiskSlice*>(&disk)->parent().drive_name();

        name = label_mappings.at(parent_name) + std::to_string(++disk_partitions[parent_name]);
    }
    else
    {
        name = prefix + suffixes[disk.media_type()];

        label_mappings[disk.drive_name()] = name;
    }

    if (suffixes[disk.media_type()].back() != 'z')
    {
        ++suffixes[disk.media_type()].back();
    }
    else
    {
        suffixes[disk.media_type()] += "a";
    }

    return name;
}

void add_drive(Disk &disk)
{
    std::string name = drive_label(disk);

    auto node = std::make_shared<disk_file>(disk, name);
    root->children.emplace_back(node);
}
}

size_t disk_file::size() const { return m_disk.disk_size(); }

MemBuffer disk_file::read_impl(size_t offset, size_t size) const
{
    return m_disk.read(offset, size);
}

bool disk_file::write_impl(size_t offset, gsl::span<const uint8_t> data)
{
    m_disk.write(offset, data);
    return true;
}

}
