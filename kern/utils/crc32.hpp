/*
crc32.hpp

Copyright (c) 12 Yann BOUCHER (yann)

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
#ifndef CRC32_HPP
#define CRC32_HPP

// Stolen from rosettacode

#include <array.hpp>
#include <algorithm.hpp>
#include <numeric.hpp>

// Generates a lookup table for the checksums of all 8-bit values.
inline std::array<uint32_t, 256> generate_crc_lookup_table() noexcept
{
  auto const reversed_polynomial = uint32_t{0xEDB88320uL};

  // This is a function object that calculates the checksum for a value,
  // then increments the value, starting from zero.
  struct byte_checksum
  {
    uint32_t operator()() noexcept
    {
      auto checksum = static_cast<uint32_t>(n++);

      for (auto i = 0; i < 8; ++i)
        checksum = (checksum >> 1) ^ ((checksum & 0x1u) ? reversed_polynomial : 0);

      return checksum;
    }

    unsigned n = 0;
  };

  auto table = std::array<uint32_t, 256>{};
  std::generate(table.begin(), table.end(), byte_checksum{});

  return table;
}

// Calculates the CRC for any sequence of values. (You could use type traits and a
// static assert to ensure the values can be converted to 8 bits.)
template <typename InputIterator>
inline uint32_t crc(InputIterator first, InputIterator last)
{
  // Generate lookup table only on first use then cache it - this is thread-safe.
  static auto const table = generate_crc_lookup_table();

  // Calculate the checksum - make sure to clip to 32 bits, for systems that don't
  // have a true (fast) 32-bit type.
  return uint32_t{0xFFFFFFFFuL} &
    ~std::accumulate(first, last,
      ~uint32_t{0} & uint32_t{0xFFFFFFFFuL},
        [](uint32_t checksum, uint8_t value)
          { return table[(checksum ^ value) & 0xFFu] ^ (checksum >> 8); });
}

#endif // CRC32_HPP
