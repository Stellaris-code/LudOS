// Verbose terminate_handler -*- C++ -*-

// Copyright (C) 2001-2017 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

#include "c++config"

#if _GLIBCXX_HOSTED || VERBOSE_TERMINATE
#include <stdlib.h>
#include "exception"
#include "exception_defines.h"
#include "cxxabi.h"
#include <stdio.h>
#include "utils/logging.hpp"

using namespace std;
using namespace abi;

namespace __gnu_cxx
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

// A replacement for the standard terminate_handler which prints
// more information about the terminating exception (if any) on
// stderr.
void __verbose_terminate_handler()
{
    static bool terminating;
    if (terminating)
    {
        err("terminate called recursively\n");
        abort ();
    }
    terminating = true;

    // Make sure there was an exception; terminate is also called for an
    // attempt to rethrow when there is no suitable exception.
    type_info *t = __cxa_current_exception_type();
    if (t)
    {
        // Note that "name" is the mangled name.
        char const *name = t->name();
        {
            int status = -1;
            char *dem = 0;

            dem = __cxa_demangle(name, 0, 0, &status);

            err("terminate called after throwing an instance of '");
            if (status == 0)
                err("%s", dem);
            else
                err("%s", name);
            err("'\n");

            if (status == 0)
                kfree(dem);
        }

        // If the exception is derived from std::exception, we can
        // give more information.
        __try { __throw_exception_again; }
#if __cpp_exceptions
        __catch(const exception& exc)
        {
            char const *w = exc.what();
            err("  what():  ");
            err("%s", w);
            err("\n");
        }
#endif
        __catch(...) { }
    }
    else
        err("terminate called without an active exception\n");

    abort();
}

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif
