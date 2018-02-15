/*
diskcache.hpp

Copyright (c) 01 Yann BOUCHER (yann)

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
#ifndef DISKCACHE_HPP
#define DISKCACHE_HPP

#include <vector.hpp>
#include <unordered_map.hpp>
#include <map.hpp>

#include <utils/gsl/gsl_span.hpp>

#include "utils/membuffer.hpp"

class Disk;

class DiskCache
{
public:
    DiskCache(Disk& disk);

    static inline size_t max_cache_size = 4096*1000;

public:
    void write_sector(size_t sec, gsl::span<const uint8_t> data);
    MemBuffer read_sector(size_t sec, size_t count);
    void flush();

    void set_ratio(size_t ratio);
    size_t ratio() const { return m_size_ratio; }

private:
    void add_span(size_t sec, size_t count);
    void add_to_cache(size_t sec, const uint8_t *data, bool write = false);
    void prune_cache();
    void remove_entry(size_t id);

    size_t mem_usage_ratio() const;

public:
    Disk& m_disk;

public:
    size_t m_size_ratio { 30 };

    struct CacheEntry
    {
        MemBuffer data {};
        bool dirty { false };
        uint64_t access_time;
    };
    std::map<size_t, DiskCache::CacheEntry> m_cache;
    std::map<uint64_t, size_t, std::greater<>> m_access_times;
};

#endif // DISKCACHE_HPP
