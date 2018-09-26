/*
kbd_mappings.hpp

Copyright (c) 24 Yann BOUCHER (yann)

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
#ifndef KBD_MAPPINGS_HPP
#define KBD_MAPPINGS_HPP

#include "driver_kbd_event.hpp"

#include <array.hpp>
#include <optional.hpp>
#include "utils/kmsgbus.hpp"

#include "keys.h"

namespace kbd
{

struct KeyEvent
{
    size_t kbd_id;
    Key key;
    enum
    {
        Pressed,
        Released
    } state;
};

using Mapping = kpp::array<Key, 256>;

void install_mapping(const Mapping& mapping, size_t kbd_id);

static constexpr size_t max_kbd_count = 256;
static inline kpp::array<kpp::optional<MessageBus::Handle>, max_kbd_count> current_handle;

namespace mappings
{
Mapping azerty();

}

}

#endif // KBD_MAPPINGS_HPP
