/*
rand.c

Copyright (c) 27 Yann BOUCHER (yann)

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

static int
do_rand(unsigned long *ctx)
{
#ifdef  USE_WEAK_SEEDING
/*
 * Historic implementation compatibility.
 * The random sequences do not vary much with the seed,
 * even with overflowing.
 */
        return ((*ctx = *ctx * 1103515245 + 12345) % ((u_long)RAND_MAX + 1));
#else   /* !USE_WEAK_SEEDING */
/*
 * Compute x = (7^5 * x) mod (2^31 - 1)
 * without overflowing 31 bits:
 *      (2^31 - 1) = 127773 * (7^5) + 2836
 * From "Random number generators: good ones are hard to find",
 * Park and Miller, Communications of the ACM, vol. 31, no. 10,
 * October 1988, p. 1195.
 */
        long hi, lo, x;

        /* Must be in [1, 0x7ffffffe] range at this point. */
        hi = *ctx / 127773;
        lo = *ctx % 127773;
        x = 16807 * lo - 2836 * hi;
        if (x < 0)
                x += 0x7fffffff;
        *ctx = x;
        /* Transform to [0, 0x7ffffffd] range. */
        return (x - 1);
#endif  /* !USE_WEAK_SEEDING */
}


int
rand_r(unsigned int *ctx)
{
        unsigned long val;
        int r;

#ifdef  USE_WEAK_SEEDING
        val = *ctx;
#else
        /* Transform to [1, 0x7ffffffe] range. */
        val = (*ctx % 0x7ffffffe) + 1;
#endif
        r = do_rand(&val);

#ifdef  USE_WEAK_SEEDING
        *ctx = (unsigned int)val;
#else
        *ctx = (unsigned int)(val - 1);
#endif
        return (r);
}


static unsigned long next =
#ifdef  USE_WEAK_SEEDING
    1;
#else
    2;
#endif

int rand()
{
        return (do_rand(&next));
}

void srand(unsigned int seed)
{
        next = seed;
#ifndef USE_WEAK_SEEDING
        /* Transform to [1, 0x7ffffffe] range. */
        next = (next % 0x7ffffffe) + 1;
#endif
}
