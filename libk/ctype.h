#ifndef _ASCII_CTYPE_H
#define _ASCII_CTYPE_H

#include <stdbool.h>
#include <stdint.h>

#ifndef __cplusplus
typedef uint32_t char32_t;
#endif

#define _ASCIIRANGE(c, start, len) (((c) - (start)) < (len))
static inline bool islower(char32_t c)  { return _ASCIIRANGE(c, 0x61, 26); }
static inline bool isupper(char32_t c)  { return _ASCIIRANGE(c, 0x41, 26); }
static inline bool isdigit(char32_t c)  { return _ASCIIRANGE(c, 0x30, 10); }
static inline bool isbdigit(char32_t c) { return _ASCIIRANGE(c, 0x30,  2); }
static inline bool isodigit(char32_t c) { return _ASCIIRANGE(c, 0x30,  8); }
static inline bool isxdigit(char32_t c) { return _ASCIIRANGE(c | 32, 0x61, 6) || isdigit(c); }
static inline bool isalpha(char32_t c)  { return _ASCIIRANGE(c | 32, 0x61, 26); }
static inline bool isprint(char32_t c)  { return _ASCIIRANGE(c, 0x20, 95) || _ASCIIRANGE(c, 128, 127); }
static inline bool isgraph(char32_t c)  { return _ASCIIRANGE(c, 0x21, 94); }
static inline bool isspace(char32_t c)  { return _ASCIIRANGE(c, 0x9, 5) || c == 0x20; }
static inline bool isalnum(char32_t c)  { return isalpha(c) || isdigit(c); }
static inline bool ispunct(char32_t c)  { return !isalnum(c) && isgraph(c); }
static inline bool isblank(char32_t c)  { return c == 0x20 || c ==  0x9; }
static inline bool iscntrl(char32_t c)  { return c  < 0x20 || c == 0x7f; }
static inline char32_t tolower(char32_t c) { return isupper(c) ? c + 0x20 : c; }
static inline char32_t toupper(char32_t c) { return islower(c) ? c - 0x20 : c; }
#undef _ASCIIRANGE

#endif
