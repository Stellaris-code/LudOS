/*
deltaqueue.hpp

Copyright (c) 31 Yann BOUCHER (yann)

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
#ifndef DELTAQUEUE_HPP
#define DELTAQUEUE_HPP

#include <list.hpp>
#include <functional.hpp>

template <typename T, typename S = size_t>
class DeltaQueue
{
public:
    DeltaQueue(std::function<void(const T&)> pop_callback = {})
     : m_pop_callback(pop_callback)
    {}

    void decrease(S duration);
    void insert(const T& el, S sleep_duration);
    // TODO : make find() O(1) or O(log n)
    bool find(const T& el) const;
    std::vector<T> elements() const;

private:
    std::function<void(const T&)> m_pop_callback;
    std::list<std::pair<T, S>> m_list;
};

template<typename T, typename S>
void DeltaQueue<T, S>::decrease(S duration)
{
    while (duration >= 0 && !m_list.empty())
    {
        S old_value = m_list.front().second;

        if (duration >= m_list.front().second)
        {
            m_list.front().second = 0;
            if (m_pop_callback) m_pop_callback(m_list.front().first);
            m_list.pop_front();
        }
        else
        {
            m_list.front().second -= duration;
        }

        if (old_value > duration) return;
        else duration -= old_value;
    }
}

template<typename T, typename S>
void DeltaQueue<T, S>::insert(const T &el, S sleep_duration)
{
    auto it { m_list.begin() };
    for (auto loop_it = m_list.begin(); loop_it != m_list.end(); std::advance(loop_it, 1))
    {
        if (sleep_duration < loop_it->second)
        {
            it = loop_it;
            break;
        }
        else
        {
            sleep_duration -= loop_it->second;
            it = std::next(loop_it);
        }
    }

    it = m_list.emplace(it, el, sleep_duration);
    if (std::next(it) != m_list.end())
    {
        std::next(it)->second -= it->second;
    }
}

template<typename T, typename S>
bool DeltaQueue<T, S>::find(const T &el) const
{
    for (const auto& pair : m_list)
    {
        if (pair.first == el)
        {
            return true;
        }
    }

    return false;
}

template<typename T, typename S>
std::vector<T> DeltaQueue<T, S>::elements() const
{
    std::vector<T> vec;
    for (const auto& pair : m_list)
    {
        vec.emplace_back(pair.first);
    }

    return vec;
}

#endif // DELTAQUEUE_HPP
