// -*- C++ -*-
//===--------------------------- stdexcept --------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_STDEXCEPT
#define _LIBCPP_STDEXCEPT

/*
    stdexcept synopsis

namespace std
{

class logic_error;
    class domain_error;
    class invalid_argument;
    class length_error;
    class out_of_range;
class runtime_error;
    class range_error;
    class overflow_error;
    class underflow_error;

for each class xxx_error:

class xxx_error : public exception // at least indirectly
{
public:
    explicit xxx_error(const string& what_arg);
    explicit xxx_error(const char*   what_arg);

    virtual const char* what() const noexcept // returns what_arg
};

}  // std

*/

#include "__config.hpp"
#include "exception.hpp"
#include "iosfwd.hpp"  // for string forward decl
#ifdef _LIBCPP_NO_EXCEPTIONS
#include <stdlib.h>
#endif

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

class _LIBCPP_HIDDEN __libcpp_refstring
{
    const char* __imp_;

    bool __uses_refcount() const;
public:
    explicit __libcpp_refstring(const char* __msg);
    __libcpp_refstring(const __libcpp_refstring& __s) _NOEXCEPT;
    __libcpp_refstring& operator=(const __libcpp_refstring& __s) _NOEXCEPT;
    ~__libcpp_refstring();

    const char* c_str() const _NOEXCEPT {return __imp_;}
};

_LIBCPP_END_NAMESPACE_STD

namespace std  // purposefully not using versioning namespace
{

class _LIBCPP_EXCEPTION_ABI logic_error
    : public exception
{
private:
    _VSTD::__libcpp_refstring __imp_;
public:
    explicit logic_error(const string&);
    explicit logic_error(const char*);

    logic_error(const logic_error&) _NOEXCEPT;
    logic_error& operator=(const logic_error&) _NOEXCEPT;

    virtual ~logic_error() _NOEXCEPT;

    virtual const char* what() const _NOEXCEPT;
};

class _LIBCPP_EXCEPTION_ABI runtime_error
    : public exception
{
private:
    _VSTD::__libcpp_refstring __imp_;
public:
    explicit runtime_error(const string&);
    explicit runtime_error(const char*);

    runtime_error(const runtime_error&) _NOEXCEPT;
    runtime_error& operator=(const runtime_error&) _NOEXCEPT;

    virtual ~runtime_error() _NOEXCEPT;

    virtual const char* what() const _NOEXCEPT;
};

class _LIBCPP_EXCEPTION_ABI domain_error
    : public logic_error
{
public:
    _LIBCPP_INLINE_VISIBILITY explicit domain_error(const string& __s) : logic_error(__s) {}
    _LIBCPP_INLINE_VISIBILITY explicit domain_error(const char* __s)   : logic_error(__s) {}

    virtual ~domain_error() _NOEXCEPT;
};

class _LIBCPP_EXCEPTION_ABI invalid_argument
    : public logic_error
{
public:
    _LIBCPP_INLINE_VISIBILITY explicit invalid_argument(const string& __s) : logic_error(__s) {}
    _LIBCPP_INLINE_VISIBILITY explicit invalid_argument(const char* __s)   : logic_error(__s) {}

    virtual ~invalid_argument() _NOEXCEPT;
};

class _LIBCPP_EXCEPTION_ABI length_error
    : public logic_error
{
public:
    _LIBCPP_INLINE_VISIBILITY explicit length_error(const string& __s) : logic_error(__s) {}
    _LIBCPP_INLINE_VISIBILITY explicit length_error(const char* __s)   : logic_error(__s) {}

    virtual ~length_error() _NOEXCEPT;
};

class _LIBCPP_EXCEPTION_ABI out_of_range
    : public logic_error
{
public:
    _LIBCPP_INLINE_VISIBILITY explicit out_of_range(const string& __s) : logic_error(__s) {}
    _LIBCPP_INLINE_VISIBILITY explicit out_of_range(const char* __s)   : logic_error(__s) {}

    virtual ~out_of_range() _NOEXCEPT;
};

class _LIBCPP_EXCEPTION_ABI range_error
    : public runtime_error
{
public:
    _LIBCPP_INLINE_VISIBILITY explicit range_error(const string& __s) : runtime_error(__s) {}
    _LIBCPP_INLINE_VISIBILITY explicit range_error(const char* __s)   : runtime_error(__s) {}

    virtual ~range_error() _NOEXCEPT;
};

class _LIBCPP_EXCEPTION_ABI overflow_error
    : public runtime_error
{
public:
    _LIBCPP_INLINE_VISIBILITY explicit overflow_error(const string& __s) : runtime_error(__s) {}
    _LIBCPP_INLINE_VISIBILITY explicit overflow_error(const char* __s)   : runtime_error(__s) {}

    virtual ~overflow_error() _NOEXCEPT;
};

class _LIBCPP_EXCEPTION_ABI underflow_error
    : public runtime_error
{
public:
    _LIBCPP_INLINE_VISIBILITY explicit underflow_error(const string& __s) : runtime_error(__s) {}
    _LIBCPP_INLINE_VISIBILITY explicit underflow_error(const char* __s)   : runtime_error(__s) {}

    virtual ~underflow_error() _NOEXCEPT;
};

}  // std

_LIBCPP_BEGIN_NAMESPACE_STD

// in the dylib
_LIBCPP_NORETURN _LIBCPP_FUNC_VIS void __throw_runtime_error(const char*);

_LIBCPP_NORETURN inline _LIBCPP_ALWAYS_INLINE
void __throw_logic_error(const char*__msg)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw logic_error(__msg);
#else
    ((void)__msg);
          abort();
#endif
}

_LIBCPP_NORETURN inline _LIBCPP_ALWAYS_INLINE
void __throw_domain_error(const char*__msg)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw domain_error(__msg);
#else
    ((void)__msg);
          abort();
#endif
}

_LIBCPP_NORETURN inline _LIBCPP_ALWAYS_INLINE
void __throw_invalid_argument(const char*__msg)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw invalid_argument(__msg);
#else
    ((void)__msg);
          abort();
#endif
}

_LIBCPP_NORETURN inline _LIBCPP_ALWAYS_INLINE
void __throw_length_error(const char*__msg)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw length_error(__msg);
#else
    ((void)__msg);
          abort();
#endif
}

_LIBCPP_NORETURN inline _LIBCPP_ALWAYS_INLINE
void __throw_out_of_range(const char*__msg)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw out_of_range(__msg);
#else
    ((void)__msg);
          abort();
#endif
}

_LIBCPP_NORETURN inline _LIBCPP_ALWAYS_INLINE
void __throw_range_error(const char*__msg)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw range_error(__msg);
#else
    ((void)__msg);
          abort();
#endif
}

_LIBCPP_NORETURN inline _LIBCPP_ALWAYS_INLINE
void __throw_overflow_error(const char*__msg)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw overflow_error(__msg);
#else
    ((void)__msg);
    abort();
#endif
}

_LIBCPP_NORETURN inline _LIBCPP_ALWAYS_INLINE
void __throw_underflow_error(const char*__msg)
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw underflow_error(__msg);
#else
    ((void)__msg);
    abort();
#endif
}

_LIBCPP_END_NAMESPACE_STD

#endif  // _LIBCPP_STDEXCEPT
