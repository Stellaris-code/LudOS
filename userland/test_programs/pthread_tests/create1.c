/*
 * create1.c
 *
 *
 * --------------------------------------------------------------------------
 *
 *      Pthreads-embedded (PTE) - POSIX Threads Library for embedded systems
 *      Copyright(C) 2008 Jason Schmidlapp
 *
 *      Contact Email: jschmidlapp@users.sourceforge.net
 *
 *
 *      Based upon Pthreads-win32 - POSIX Threads Library for Win32
 *      Copyright(C) 1998 John E. Bossom
 *      Copyright(C) 1999,2005 Pthreads-win32 contributors
 *
 *      Contact Email: rpj@callisto.canberra.edu.au
 *
 *      The original list of contributors to the Pthreads-win32 project
 *      is contained in the file CONTRIBUTORS.ptw32 included with the
 *      source code distribution. The list can also be seen at the
 *      following World Wide Web location:
 *      http://sources.redhat.com/pthreads-win32/contributors.html
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 *
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library in the file COPYING.LIB;
 *      if not, write to the Free Software Foundation, Inc.,
 *      59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 *
 * --------------------------------------------------------------------------
 *
 * Description:
 * Create a thread and check that it ran.
 *
 * Depends on API functions: None.
 */

#include <stdlib.h>

#include "test.h"

static int washere = 0;

static void * func(void * arg)
{
    washere = 1;
    return 0;
}

int pthread_test_create1()
{
    int bob;

    for (size_t i = 0; i < 1; ++i)
    {
        pthread_t t;

        washere = 0;

        assert(pthread_create(&t, NULL, func, NULL) == 0);

        /* A dirty hack, but we cannot rely on pthread_join in this
     primitive test. */
        //pte_osThreadSleep(2000);
        pte_osYield();

        assert(washere == 1);

        assert(pthread_join(t,NULL) == 0);

    }

    return 0;
}
