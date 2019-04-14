/*
 * once2.c
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
 * Create several static pthread_once objects and channel several threads
 * through each.
 *
 * Depends on API functions:
 *	pthread_once()
 *	pthread_create()
 */

#include "test.h"

#define NUM_THREADS OS_MAX_SIMUL_THREADS / 5 /* Targeting each once control */
#define NUM_ONCE    5


static pthread_once_t o = PTHREAD_ONCE_INIT;
static pthread_once_t once[NUM_ONCE];

typedef struct
  {
    int i;
    pte_osMutexHandle cs;
  } sharedInt_t;

static sharedInt_t numOnce = {0, 0};
static sharedInt_t numThreads = {0, 0};

static void
myfunc(void)
{

  pte_osMutexLock(numOnce.cs);

  numOnce.i++;

  pte_osMutexUnlock(numOnce.cs);

  /* Simulate slow once routine so that following threads pile up behind it */
  pte_osThreadSleep(100);
}

static void *
mythread(void * arg)
{

  assert(pthread_once(&once[(int) arg], myfunc) == 0);

  pte_osMutexLock(numThreads.cs);
  numThreads.i++;
  pte_osMutexUnlock(numThreads.cs);

  return 0;
}

static pthread_t t[NUM_THREADS][NUM_ONCE];

int pthread_test_once2()
{
  int i, j;
  int result;

  numOnce.i = 0;
  numThreads.i = 0;

  pte_osMutexCreate(&numThreads.cs);
  pte_osMutexCreate(&numOnce.cs);

  for (j = 0; j < NUM_ONCE; j++)
    {
      once[j] = o;

      for (i = 0; i < NUM_THREADS; i++)
        assert(pthread_create(&t[i][j], NULL, mythread, (void *) j) == 0);
    }

  for (j = 0; j < NUM_ONCE; j++)
    for (i = 0; i < NUM_THREADS; i++)
      if ((result = pthread_join(t[i][j], NULL)) != 0)
        {
          assert(0); //Join failed for [thread,once]
        }

  /* Stop some compilers from generating warning */
  result = result;

  pte_osThreadSleep(1000);

  assert(numOnce.i == NUM_ONCE);
  assert(numThreads.i == NUM_THREADS * NUM_ONCE);

  pte_osMutexDelete(numOnce.cs);
  pte_osMutexDelete(numThreads.cs);

  return 0;
}
