/*
diskcache.cpp

Copyright (c) 02 Yann BOUCHER (yann)

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

#include "diskcache.hpp"

#include "time/time.hpp"

#include "disk.hpp"
#include "mem/meminfo.hpp"
#include "utils/memutils.hpp"
#include "utils/stlutils.hpp"

#include "panic.hpp"

DiskCache::DiskCache(Disk &disk)
    : m_disk(disk)
{

}

void DiskCache::write_sector(size_t sec, gsl::span<const uint8_t> data)
{
    auto chunks = split(data, m_disk.sector_size());
    for (size_t i { 0 }; i < chunks.size(); ++i)
    {
        add_to_cache(sec + i, data.data() + m_disk.sector_size()*i, true);
    }

    prune_cache();
}

std::vector<uint8_t> DiskCache::read_sector(size_t sec, size_t count)
{
    std::vector<uint8_t> data;
    data.reserve(count*m_disk.sector_size());

    add_span(sec, count);

    for (size_t i { sec }; i < sec + count; ++i)
    {
        merge(data, m_cache.at(i).data);

        const uint64_t ticks = Time::total_ticks();

        m_access_times.erase(m_cache.at(i).access_time);
        m_access_times.emplace(ticks, i);
        m_cache.at(i).access_time = ticks;
    }

    prune_cache();

    return data;
}

void DiskCache::flush()
{
    while (!m_cache.empty())
    {
        remove_entry(m_access_times.begin()->second);
    }
}

void DiskCache::set_ratio(size_t ratio)
{
    m_size_ratio = ratio;
    prune_cache();
}

void DiskCache::add_span(size_t sec, size_t count)
{
#if 1
    for (size_t i { sec }; i < sec + count;)
    {
        if (m_cache.count(i) == 0)
        {
            size_t len { 1 };
            while (m_cache.count(i + len) == 0 && len < count) { ++len; };

#if 0
            auto data = split(m_disk.read_sector(i, len), m_disk.sector_size());

            for (size_t j { 0 }; j < len; ++j)
            {
                add_to_cache(i+j, data[j].data());
            }
#else
            auto data = m_disk.read_sector(i, len);

            for (size_t j { 0 }; j < len; ++j)
            {
                add_to_cache(sec + j, data.data() + j*m_disk.sector_size());
            }
#endif

            i += len;
        }
        else
        {
            ++i;
        }
    }
#else
    for (size_t i { sec }; i < sec + count; ++i)
    {
        add_to_cache(i, m_disk.read_sector(i, 1));
    }
#endif
}

void DiskCache::add_to_cache(size_t sec, const uint8_t *data, bool write)
{
    auto ticks = Time::total_ticks();

    if (m_cache.count(sec) == 0)
    {
        //log_serial("Cache miss\n");

        m_access_times.emplace(ticks, sec);
        m_cache[sec] = CacheEntry{std::vector<uint8_t>(data, data + m_disk.sector_size()), write, ticks};
    }
    else
    {
        m_cache.at(sec).data = std::vector<uint8_t>(data, data + m_disk.sector_size());
        m_cache.at(sec).dirty = write;
        m_access_times.erase(m_cache.at(sec).access_time);
        m_cache.at(sec).access_time = ticks;
        m_access_times.emplace(ticks, sec);
    }
}

void DiskCache::prune_cache()
{
    if (mem_usage_ratio() > m_size_ratio)
    {
        log_serial("Pruning cache\n");
    }

    while (!m_cache.empty() && mem_usage_ratio() > m_size_ratio)
    {
        remove_entry(m_access_times.begin()->second);
    }
}

void DiskCache::remove_entry(size_t id)
{
    if (m_cache.at(id).dirty) m_disk.write_sector(id, m_cache.at(id).data);

    assert(m_access_times.erase(m_cache.at(id).access_time));
    assert(m_cache.erase(id));
}

size_t DiskCache::mem_usage_ratio() const
{
#if 0
    const auto allocated_size = (m_cache.size()*(sizeof(CacheEntry)+sizeof(size_t) +
                                                 m_access_times.size()*(sizeof(uint64_t)*sizeof(uint64_t))));
    return allocated_size*100 / MemoryInfo::total();
#else
    return MemoryInfo::usage_ratio();
#endif
}
