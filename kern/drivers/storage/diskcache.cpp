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

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> DiskCache::write_sector(size_t sec, gsl::span<const uint8_t> data)
{
    auto chunks = split(data, m_disk.sector_size());
    for (size_t i { 0 }; i < chunks.size(); ++i)
    {
        add_to_cache(sec + i, chunks[i], true);
    }

    return prune_cache();
}

[[nodiscard]]
kpp::expected<MemBuffer, DiskError> DiskCache::read_sector(size_t sec, size_t count)
{
    MemBuffer data;
    data.reserve(count*m_disk.sector_size());

    auto result = add_span(sec, count);
    if (!result) return kpp::make_unexpected(result.error());

    for (size_t i { sec }; i < sec + count; ++i)
    {
        merge(data, m_cache.at(i).data);

        const uint64_t ticks = Time::total_ticks();

        m_access_times.erase(m_cache.at(i).access_time);
        m_access_times.emplace(ticks, i);
        m_cache.at(i).access_time = ticks;
    }

    auto prune_result = prune_cache();
    if (!prune_result)
        return kpp::make_unexpected(prune_result.error());

    return std::move(data);
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> DiskCache::flush()
{
    while (!m_cache.empty())
    {
        auto result = remove_entry(m_access_times.begin()->second);
        if (!result)
            return kpp::make_unexpected(result.error());
    }
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> DiskCache::set_ratio(size_t ratio)
{
    m_size_ratio = ratio;
    return prune_cache();
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> DiskCache::add_span(size_t sec, size_t count)
{
#if 1
    for (size_t i { sec }; i < sec + count;)
    {
        if (m_cache.count(i) == 0)
        {
            size_t len { 1 };
            while (m_cache.count(i + len) == 0 && len < count) { ++len; };

#if 1
            auto result = m_disk.read_sector(i, len);
            if (!result) return kpp::make_unexpected(result.error());
            auto data = split(result.value(), m_disk.sector_size());

            for (size_t j { 0 }; j < len; ++j)
            {
                add_to_cache(i+j, data[j]);
            }
#else
            auto data = m_disk.read_sector(i, len);

            for (size_t j { 0 }; j < len; ++j)
            {
                add_to_cache(sec + j, gsl::span<const uint8_t>(data.data() + j*m_disk.sector_size(),
                                                               data.data() + (j+1)*m_disk.sector_size()));
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

    return {};
}

void DiskCache::add_to_cache(size_t sec, gsl::span<const uint8_t> data, bool write)
{
    auto ticks = Time::total_ticks();

    if (m_cache.count(sec) == 0)
    {
        //log_serial("Cache miss\n");

        m_access_times.emplace(ticks, sec);
        m_cache[sec] = CacheEntry{MemBuffer(data.begin(), data.end()), write, ticks};
    }
    else
    {
        m_cache.at(sec).data = MemBuffer(data.begin(), data.end());
        m_cache.at(sec).dirty = write;

        assert(m_access_times.count(m_cache.at(sec).access_time));
        m_access_times.erase(m_cache.at(sec).access_time);
        m_cache.at(sec).access_time = ticks;
        m_access_times.emplace(ticks, sec);
    }
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> DiskCache::prune_cache()
{
    if (mem_usage_ratio() > m_size_ratio)
    {
        log_serial("Pruning cache\n");
    }

    while (!m_cache.empty() && mem_usage_ratio() > m_size_ratio)
    {
        auto result = remove_entry(m_access_times.begin()->second);
        if (!result) return kpp::make_unexpected(result.error());
    }
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> DiskCache::remove_entry(size_t id)
{
    if (m_cache.at(id).dirty)
    {
       auto result = m_disk.write_sector(id, m_cache.at(id).data);
       if (!result) return kpp::make_unexpected(result.error());
    }

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
