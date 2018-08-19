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

#include "fs/fsutils.hpp"
#include "fs/vfs.hpp"

#include <deque.hpp>

#include "drivers/driver.hpp"
#include "drivers/kbd/keyboard.hpp"
#include "drivers/storage/disk.hpp"

#include "tasking/process.hpp"

#include "utils/messagebus.hpp"
#include "utils/nop.hpp"

#include "fbdev.hpp"
#include "kbdev.hpp"

namespace devfs
{

struct stdout_file : public vfs::node
{
    using node::node;

protected:
    [[nodiscard]] virtual kpp::expected<kpp::dummy_t, vfs::FSError> write_impl(size_t offset, gsl::span<const uint8_t> data) override
    {
        auto str = kpp::string((const char*)data.data(), data.size());
        kprintf("%s", str.c_str());
        return {};
    }
};
struct stderr_file : public vfs::node
{
    using node::node;
protected:
    [[nodiscard]] virtual kpp::expected<kpp::dummy_t, vfs::FSError> write_impl(size_t offset, gsl::span<const uint8_t> data) override
    {
        auto str = kpp::string((const char*)data.data(), data.size());
        err("%s", str.c_str());
        return {};
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
    [[nodiscard]] virtual kpp::expected<MemBuffer, vfs::FSError> read_impl(size_t offset, size_t size) const override
    {
        MemBuffer buf;

        while (m_input_buffer.size() < size) { wait_for_interrupts(); };

        for (size_t i { 0 }; i < size; ++i)
        {
            buf.emplace_back(m_input_buffer.front());
            m_input_buffer.pop_front();
        }

        assert(buf.size() == size);

        return std::move(buf);
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
    auto result = vfs::root->create("dev", vfs::node::Directory);
    if (!result) panic("Could not create devfs : %s\n", result.error().to_string());

    root = std::make_shared<devfs_root>(vfs::root.get());
    if (!vfs::mount(root, result.value()))
    {
        panic("Can't mount devfs !\n");
    }

    auto stdin_node = std::make_shared<stdin_file>(root.get());
    stdin_node->rename("stdin");

    root->children.emplace_back(stdin_node);

    auto stdout_node = std::make_shared<stdout_file>(root.get());
    stdout_node->rename("stdout");

    root->children.emplace_back(stdout_node);

    auto stderr_node = std::make_shared<stderr_file>(root.get());
    stderr_node->rename("stderr");

    root->children.emplace_back(stderr_node);

    // TODO : make it a driver
    auto fbdev = std::make_shared<fbdev_node>(root.get());
    fbdev->rename("fbdev");

    root->children.emplace_back(fbdev);

    // add existing drives
    for (Disk& disk : Disk::disks())
    {
        detail::add_drive(disk);
    }

    for (Driver& drv : Driver::list())
    {
        detail::handle_new_driver(drv);
    }

    MessageBus::register_handler<DiskFoundEvent>([](const DiskFoundEvent& ev)
    {
        detail::add_drive(ev.disk);
    });

    MessageBus::register_handler<DriverLoadedEvent>([](const DriverLoadedEvent& ev)
    {
        detail::handle_new_driver(ev.drv);
    });
}

size_t disk_file::size() const { return m_disk.disk_size(); }

kpp::expected<MemBuffer, vfs::FSError> disk_file::read_impl(size_t offset, size_t size) const
{
    auto result = m_disk.read(offset, size);
    if (!result)
    {
        return kpp::make_unexpected(vfs::FSError{vfs::FSError::ReadError, {result.error().type}});
    }
    return std::move(result.value());
}

kpp::expected<kpp::dummy_t, vfs::FSError> disk_file::write_impl(size_t offset, gsl::span<const uint8_t> data)
{
    auto result = m_disk.write(offset, data);
    if (!result)
    {
        return kpp::make_unexpected(vfs::FSError{vfs::FSError::WriteError, {result.error().type}});
    }
    return {};
}

namespace detail
{

kpp::string drive_label(const Disk& disk)
{
    static std::map<kpp::string, kpp::string> label_mappings;
    static std::map<kpp::string, size_t> disk_partitions;
    static kpp::array<kpp::string, Disk::Unknown + 1> suffixes;
    if (suffixes[0].empty()) suffixes.fill("a");

    kpp::string prefix;
    kpp::string name;

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

        name = label_mappings.at(parent_name) + kpp::to_string(++disk_partitions[parent_name]);
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
    kpp::string name = drive_label(disk);

    auto node = std::make_shared<disk_file>(root.get(), disk, name);
    root->children.emplace_back(node);
}

void handle_new_driver(Driver &drv)
{
    if (drv.type() == DriverType::Keyboard)
    {
        static int last_keyboard_id = 0;
        auto kb_node = std::make_shared<kbdev_node>(root.get());
        kb_node->keyboard_id = last_keyboard_id;
        kb_node->rename("kbd" + kpp::to_string(last_keyboard_id++));
        root->children.emplace_back(kb_node);
    }
}

}

}
