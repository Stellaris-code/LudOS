/*
terminaldata.cpp

Copyright (c) 15 Yann BOUCHER (yann)

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

#include "terminaldata.hpp"

#include <algorithm.hpp>

#include "utils/logging.hpp"

void TerminalData::add_line(const std::vector<TermEntry> &entry)
{
    m_buffer.add(entry);
}

std::vector<std::vector<std::reference_wrapper<const TermEntry> > > TerminalData::get_screen(size_t width, size_t height, size_t offset) const
{
    std::vector<std::vector<std::reference_wrapper<const TermEntry>>> vec;

    const size_t bound = std::min(height + offset, m_buffer.size());

    for (size_t i { offset }; i < bound && vec.size() < height; ++i)
    {
        std::vector<std::reference_wrapper<const TermEntry>> line;

        const auto& entry = m_buffer.get_entry(i);
        for (size_t j { 0 }; j < entry.size(); ++j)
        {
            line.emplace_back(entry[j]);
            if (line.size() >= width)
            {
                vec.emplace_back(line);
                line.clear();
            }
        }

        vec.emplace_back(line);
    }

    return vec;
}

void TerminalData::set_entry(size_t x, size_t y, TermEntry entry)
{
    // adjust to needs
    while (y >= m_buffer.size())
    {
        m_buffer.add({});
    }

    auto& line = m_buffer.get_entry(y);

    line.resize(x + 1);
    line[x] = entry;
}

void TerminalData::clear()
{
    m_buffer.clear();
}
