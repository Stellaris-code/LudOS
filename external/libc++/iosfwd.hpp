// -*- C++ -*-
//===--------------------------- iosfwd -----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_IOSFWD
#define _LIBCPP_IOSFWD


#include "__config.hpp"
#include "../libc/wchar.h"  // for mbstate_t

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

//#include_next <wchar.h>

_LIBCPP_BEGIN_NAMESPACE_STD

class _LIBCPP_TYPE_VIS ios_base;

template<class _CharT>  struct _LIBCPP_TEMPLATE_VIS char_traits;
template<class _Tp>     class _LIBCPP_TEMPLATE_VIS allocator;

template <class _CharT, class _Traits = char_traits<_CharT> >
    class _LIBCPP_TEMPLATE_VIS basic_ios;

template <class _CharT, class _Traits = char_traits<_CharT> >
    class _LIBCPP_TEMPLATE_VIS basic_streambuf;
template <class _CharT, class _Traits = char_traits<_CharT> >
    class _LIBCPP_TEMPLATE_VIS basic_istream;
template <class _CharT, class _Traits = char_traits<_CharT> >
    class _LIBCPP_TEMPLATE_VIS basic_ostream;
template <class _CharT, class _Traits = char_traits<_CharT> >
    class _LIBCPP_TEMPLATE_VIS basic_iostream;

template <class _CharT, class _Traits = char_traits<_CharT>,
          class _Allocator = allocator<_CharT> >
    class _LIBCPP_TEMPLATE_VIS basic_stringbuf;
template <class _CharT, class _Traits = char_traits<_CharT>,
          class _Allocator = allocator<_CharT> >
    class _LIBCPP_TEMPLATE_VIS basic_istringstream;
template <class _CharT, class _Traits = char_traits<_CharT>,
          class _Allocator = allocator<_CharT> >
    class _LIBCPP_TEMPLATE_VIS basic_ostringstream;
template <class _CharT, class _Traits = char_traits<_CharT>,
          class _Allocator = allocator<_CharT> >
    class _LIBCPP_TEMPLATE_VIS basic_stringstream;

template <class _CharT, class _Traits = char_traits<_CharT> >
    class _LIBCPP_TEMPLATE_VIS basic_filebuf;
template <class _CharT, class _Traits = char_traits<_CharT> >
    class _LIBCPP_TEMPLATE_VIS basic_ifstream;
template <class _CharT, class _Traits = char_traits<_CharT> >
    class _LIBCPP_TEMPLATE_VIS basic_ofstream;
template <class _CharT, class _Traits = char_traits<_CharT> >
    class _LIBCPP_TEMPLATE_VIS basic_fstream;

template <class _CharT, class _Traits = char_traits<_CharT> >
    class _LIBCPP_TEMPLATE_VIS istreambuf_iterator;
template <class _CharT, class _Traits = char_traits<_CharT> >
    class _LIBCPP_TEMPLATE_VIS ostreambuf_iterator;

typedef basic_ios<char>              ios;
typedef basic_ios<wchar_t>           wios;

typedef basic_streambuf<char>        streambuf;
typedef basic_istream<char>          istream;
typedef basic_ostream<char>          ostream;
typedef basic_iostream<char>         iostream;

typedef basic_stringbuf<char>        stringbuf;
typedef basic_istringstream<char>    istringstream;
typedef basic_ostringstream<char>    ostringstream;
typedef basic_stringstream<char>     stringstream;

typedef basic_filebuf<char>          filebuf;
typedef basic_ifstream<char>         ifstream;
typedef basic_ofstream<char>         ofstream;
typedef basic_fstream<char>          fstream;

typedef basic_streambuf<wchar_t>     wstreambuf;
typedef basic_istream<wchar_t>       wistream;
typedef basic_ostream<wchar_t>       wostream;
typedef basic_iostream<wchar_t>      wiostream;

typedef basic_stringbuf<wchar_t>     wstringbuf;
typedef basic_istringstream<wchar_t> wistringstream;
typedef basic_ostringstream<wchar_t> wostringstream;
typedef basic_stringstream<wchar_t>  wstringstream;

typedef basic_filebuf<wchar_t>       wfilebuf;
typedef basic_ifstream<wchar_t>      wifstream;
typedef basic_ofstream<wchar_t>      wofstream;
typedef basic_fstream<wchar_t>       wfstream;

template <class _State>             class _LIBCPP_TEMPLATE_VIS fpos;
typedef fpos<mbstate_t>    streampos;
typedef fpos<mbstate_t>    wstreampos;
#ifndef _LIBCPP_HAS_NO_UNICODE_CHARS
typedef fpos<mbstate_t>    u16streampos;
typedef fpos<mbstate_t>    u32streampos;
#endif  // _LIBCPP_HAS_NO_UNICODE_CHARS

#if defined(_NEWLIB_VERSION)
// On newlib, off_t is 'long int'
typedef long int streamoff;         // for char_traits in <string>
#else
typedef long long streamoff;        // for char_traits in <string>
#endif

template <class _CharT,             // for <stdexcept>
          class _Traits = char_traits<_CharT>,
          class _Allocator = allocator<_CharT> >
    class _LIBCPP_TEMPLATE_VIS basic_string;
typedef basic_string<char, char_traits<char>, allocator<char> > string;
typedef basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > wstring;


// Include other forward declarations here
template <class _Tp, class _Alloc = allocator<_Tp> >
class _LIBCPP_TEMPLATE_VIS vector;

_LIBCPP_END_NAMESPACE_STD

#endif  // _LIBCPP_IOSFWD
