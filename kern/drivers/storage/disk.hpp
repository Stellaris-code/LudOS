/*
disk.hpp

Copyright (c) 03 Yann BOUCHER (yann)

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
#ifndef DISK_HPP
#define DISK_HPP

#include <string.hpp>
#include <exception.hpp>

#include "utils/vecutils.hpp"
#include "utils/messagebus.hpp"
#include "utils/noncopyable.hpp"

#include <utils/gsl/gsl_span.hpp>

#include "diskcache.hpp"
#include "panic.hpp"

struct DiskFoundEvent
{
    class Disk& disk;
};

struct SyncDisksCache
{
};

class Disk : NonCopyable
{
    friend class DiskSlice;
    friend class DiskCache;

public:
    static void system_init();

    enum Type
    {
        Floppy,
        OpticalDisk,
        HardDrive,
        NetworkDrive,
        MemoryCard,
        RamDrive,
        Other,
        Unknown
    };

public:
    Disk();
    virtual ~Disk();

    virtual size_t disk_size() const = 0;
    virtual size_t sector_size() const = 0;
    virtual std::string drive_name() const = 0;
    virtual void flush_hardware_cache() = 0;
    virtual Type media_type() const = 0;
    virtual bool is_partition() const { return false; };

    std::vector<uint8_t> read(size_t offset, size_t size) const;
    std::vector<uint8_t> read() const;

    void write(size_t offset, gsl::span<const uint8_t> data);

    void enable_caching(bool val);
    bool caching_enabled() const { return m_caching; }
    void flush_cache();

private:
    std::vector<uint8_t> read_cache_sector(size_t sector, size_t count) const;
    void write_cache_sector(size_t sector, gsl::span<const uint8_t> data);

protected:
    virtual std::vector<uint8_t> read_sector(size_t sector, size_t count) const = 0;
    virtual void write_sector(size_t sector, gsl::span<const uint8_t> data) = 0;

public:
    static ref_vector<Disk> disks();

private:
    mutable DiskCache m_cache;
    bool m_caching { false };

protected:
    static inline std::vector<std::unique_ptr<Disk>> m_disks;
};

template <typename Derived>
class DiskImpl : public Disk
{
public:
    DiskImpl() : Disk() { }
    DiskImpl(DiskImpl&&) : Disk() {}

public:
    template <typename... Args>
    static Derived& create_disk(Args&&... args)
    {
        static_assert(std::is_base_of_v<Disk, Derived>);
        Disk::m_disks.emplace_back(std::make_unique<Derived>(std::forward<Args>(args)...));
        MessageBus::send<DiskFoundEvent>({Disk::disks().back()});
        return static_cast<Derived&>(Disk::disks().back().get());
    }
};

class MemoryDisk : public DiskImpl<MemoryDisk>
{
public:
    friend class Disk;

    virtual size_t disk_size() const override { return m_size; }
    virtual size_t sector_size() const override { return 512; }
    virtual std::string drive_name() const override { return m_name; }
    virtual void flush_hardware_cache() override {}
    virtual Type media_type() const override { return Disk::RamDrive; }

protected:
    virtual std::vector<uint8_t> read_sector(size_t sector, size_t count) const override;
    virtual void write_sector(size_t sector, gsl::span<const uint8_t> data) override;

public:
    MemoryDisk(uint8_t* data, size_t size, const std::string& name);
    MemoryDisk(const uint8_t* data, size_t size, const std::string& name);

private:
    size_t m_size { 0 };
    uint8_t* m_data { nullptr };
    std::string m_name;
    bool m_const { false };
};

class DiskSlice : public DiskImpl<DiskSlice>
{
public:
    DiskSlice(Disk& disk, size_t offset, size_t size);

    virtual size_t disk_size() const override { return m_size * sector_size(); }
    virtual size_t sector_size() const override { return m_base_disk.sector_size(); }
    virtual std::string drive_name() const override { return m_base_disk.drive_name() + " - slice"; }
    virtual void flush_hardware_cache() override { m_base_disk.flush_hardware_cache(); }
    virtual Type media_type() const override { return m_base_disk.media_type(); }
    virtual bool is_partition() const override { return true; };

protected:
    virtual std::vector<uint8_t> read_sector(size_t sector, size_t count) const override;
    virtual void write_sector(size_t sector, gsl::span<const uint8_t> data) override;

private:
    Disk& m_base_disk;
    size_t m_offset {};
    size_t m_size {};
};

class DiskException : public std::runtime_error
{
public:
    enum ErrorType
    {
        OK = 0,
        OutOfBounds,
        ReadOnly,
        BadSector,
        NoMedia,
        Aborted,
        Unknown
    };

    std::string to_string(ErrorType type)
    {
        switch (type)
        {
            case OutOfBounds:
                return "Out of bounds access";
            case ReadOnly:
                return "Disk is read only";
            case BadSector:
                return "Bad sector access";
            case NoMedia:
                return "No media";
            case Aborted:
                return "Access aborted";
            case Unknown:
            default:
                return "Unknown error";
        }
    }

    explicit DiskException(const Disk& disk, ErrorType type)
        : std::runtime_error("Disk error : " + to_string(type) + " on disk " + disk.drive_name())
    {
        assert(type != OK);
    }
};

#endif // DISK_HPP
