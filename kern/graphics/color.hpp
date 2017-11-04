/*
color.hpp

Copyright (c) 29 Yann BOUCHER (yann)

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
#ifndef COLOR_HPP
#define COLOR_HPP

#include <stdint.h>

#include <math.h>

namespace video
{

struct Color
{
    union
    {
        struct
        {
            uint8_t b;
            uint8_t g;
            uint8_t r;
        };
        uint32_t rgb;
    };

    Color() = default;
    Color(uint8_t ir, uint8_t ig, uint8_t ib)
        : r(ir), g(ig), b(ib)
    {}
    Color(uint32_t irgb)
        : rgb(irgb)
    {}
};

inline Color hsvToRgb(double h, double s, double v)
{
    Color color;

    if (s == 0)
    {
        color.r = v * 255;
        color.g = v * 255;
        color.b = v * 255;
    }
    else
    {
        double var_h = h * 6;
        int var_i = var_h;     //Or ... var_i = floor( var_h )
        double var_1 = v * ( 1 - s );
        double var_2 = v * ( 1 - s * ( var_h - var_i ) );
        double var_3 = v * ( 1 - s * ( 1 - ( var_h - var_i ) ) );

        double var_r, var_g, var_b;

        if      ( var_i == 0 ) { var_r = v     ; var_g = var_3 ; var_b = var_1; }
        else if ( var_i == 1 ) { var_r = var_2 ; var_g = v     ; var_b = var_1; }
        else if ( var_i == 2 ) { var_r = var_1 ; var_g = v     ; var_b = var_3; }
        else if ( var_i == 3 ) { var_r = var_1 ; var_g = var_2 ; var_b = v;     }
        else if ( var_i == 4 ) { var_r = var_3 ; var_g = var_1 ; var_b = v;     }
        else                   { var_r = v     ; var_g = var_1 ; var_b = var_2; }

        color.r = var_r * 255;
        color.g = var_g * 255;
        color.b = var_b * 255;
    }

    return color;
}

struct TermEntry
{
    Color fg;
    Color bg;
};

}

#endif // COLOR_HPP
