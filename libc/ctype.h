#ifndef ASCII_CTYPE_H
#define ASCII_CTYPE_H

#define ASCIIRANGE(c, start, len) (char32_t((c) - (start)) < char32_t(len))
inline bool islower(char32_t c)  { return ASCIIRANGE(c, 0x61, 26); }
inline bool isupper(char32_t c)  { return ASCIIRANGE(c, 0x41, 26); }
inline bool isdigit(char32_t c)  { return ASCIIRANGE(c, 0x30, 10); }
inline bool isbdigit(char32_t c) { return ASCIIRANGE(c, 0x30,  2); }
inline bool isodigit(char32_t c) { return ASCIIRANGE(c, 0x30,  8); }
inline bool isxdigit(char32_t c) { return ASCIIRANGE(c | 32, 0x61, 6) || isdigit(c); }
inline bool isalpha(char32_t c)  { return ASCIIRANGE(c | 32, 0x61, 26); }
inline bool isprint(char32_t c)  { return ASCIIRANGE(c, 0x20, 95) || ASCIIRANGE(c, 128, 127); }
inline bool isgraph(char32_t c)  { return ASCIIRANGE(c, 0x21, 94); }
inline bool isspace(char32_t c)  { return ASCIIRANGE(c, 0x9, 5) || c == 0x20; }
inline bool isalnum(char32_t c)  { return isalpha(c) || isdigit(c); }
inline bool ispunct(char32_t c)  { return !isalnum(c) && isgraph(c); }
inline bool isblank(char32_t c)  { return c == 0x20 || c ==  0x9; }
inline bool iscntrl(char32_t c)  { return c  < 0x20 || c == 0x7f; }
inline char32_t tolower(char32_t c) { return isupper(c) ? c + 0x20 : c; }
inline char32_t toupper(char32_t c) { return islower(c) ? c - 0x20 : c; }
#undef ASCIIRANGE

#endif
