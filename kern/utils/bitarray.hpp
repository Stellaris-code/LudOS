/*
bitarray.hpp

Copyright (c) 09 Yann BOUCHER (yann)

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
#ifndef BITARRAY_HPP
#define BITARRAY_HPP

#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

class bitarray_base
{
protected:
    template <typename IntType = uint64_t>
    class bitref
    {
    public:
        bitref(IntType &val, IntType mask)
            : m_val(val), m_mask(mask)
        {
        }

        const bitref &operator =(bool val) {
            if (val)
            {
                m_val |= m_mask;
            }
            else
            {
                m_val &= ~m_mask;
            }
            return *this;
        }
        const bitref &operator =(const bitref &br)
        {
            return this->operator =(bool(br));
        }
        operator bool() const
        {
            return (m_val & m_mask) != 0;
        }

    private:
        IntType &m_val;
        IntType m_mask;
    };
};

template <size_t Size, typename IntType = uint64_t>
class bitarray : public bitarray_base
{
private:
    static constexpr size_t numvals = ((Size + sizeof(IntType)*CHAR_BIT-1) / (sizeof(IntType)*CHAR_BIT));
public:
    bitarray()
    {
        memset(m_vals, 0, numvals*sizeof(IntType));
    }

    bool operator [](size_t bitnum) const
    {
        assert(bitnum < Size);

        const size_t bytenum = bitnum / (sizeof(IntType)*CHAR_BIT);
        bitnum = bitnum % (sizeof(IntType)*CHAR_BIT);
        return (m_vals[bytenum] & (IntType(1) << bitnum)) != 0;
    }
    bitref<IntType> operator[](size_t bitnum)
    {
        assert(bitnum < Size);

        const size_t bytenum = bitnum / (sizeof(IntType)*CHAR_BIT);
        bitnum = bitnum % (sizeof(IntType)*CHAR_BIT);
        IntType mask = IntType(1) << bitnum;
        return bitref<IntType>(m_vals[bytenum], mask);
    }

private:
    IntType m_vals[numvals] = {0};
};

#endif // BITARRAY_HPP
