/*
pthread.h

Copyright (c) 24 Yann BOUCHER (yann)

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
#ifndef PTHREAD_H
#define PTHREAD_H

#include "assert.h"

typedef int pthread_key_t;
typedef int pthread_once_t;
typedef unsigned long int pthread_t;

static inline int sched_yield(void)
{
    return 0;
}

#define __SIZEOF_PTHREAD_ATTR_T 36
#define __SIZEOF_PTHREAD_MUTEX_T 24
#define __SIZEOF_PTHREAD_MUTEXATTR_T 4
#define __SIZEOF_PTHREAD_COND_T 48
#define __SIZEOF_PTHREAD_COND_COMPAT_T 12
#define __SIZEOF_PTHREAD_CONDATTR_T 4
#define __SIZEOF_PTHREAD_RWLOCK_T 32
#define __SIZEOF_PTHREAD_RWLOCKATTR_T 8
#define __SIZEOF_PTHREAD_BARRIER_T 20
#define __SIZEOF_PTHREAD_BARRIERATTR_T 4


/* Detach state.  */
enum
{
  PTHREAD_CREATE_JOINABLE,
#define PTHREAD_CREATE_JOINABLE	PTHREAD_CREATE_JOINABLE
  PTHREAD_CREATE_DETACHED
#define PTHREAD_CREATE_DETACHED	PTHREAD_CREATE_DETACHED
};


/* Mutex types.  */
enum
{
  PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_ADAPTIVE_NP
#if defined __USE_UNIX98 || defined __USE_XOPEN2K8
  ,
  PTHREAD_MUTEX_NORMAL = PTHREAD_MUTEX_TIMED_NP,
  PTHREAD_MUTEX_RECURSIVE = PTHREAD_MUTEX_RECURSIVE_NP,
  PTHREAD_MUTEX_ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK_NP,
  PTHREAD_MUTEX_DEFAULT = PTHREAD_MUTEX_NORMAL
#endif
#ifdef __USE_GNU
  /* For compatibility.  */
  , PTHREAD_MUTEX_FAST_NP = PTHREAD_MUTEX_TIMED_NP
#endif
};


#ifdef __USE_XOPEN2K
/* Robust mutex or not flags.  */
enum
{
  PTHREAD_MUTEX_STALLED,
  PTHREAD_MUTEX_STALLED_NP = PTHREAD_MUTEX_STALLED,
  PTHREAD_MUTEX_ROBUST,
  PTHREAD_MUTEX_ROBUST_NP = PTHREAD_MUTEX_ROBUST
};
#endif


#if defined __USE_POSIX199506 || defined __USE_UNIX98
/* Mutex protocols.  */
enum
{
  PTHREAD_PRIO_NONE,
  PTHREAD_PRIO_INHERIT,
  PTHREAD_PRIO_PROTECT
};
#endif


/* Mutex initializers.  */
#ifdef __PTHREAD_MUTEX_HAVE_PREV
# define PTHREAD_MUTEX_INITIALIZER \
  { { 0, 0, 0, 0, 0, 0, { 0, 0 } } }
# ifdef __USE_GNU
#  define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP \
  { { 0, 0, 0, 0, PTHREAD_MUTEX_RECURSIVE_NP, 0, { 0, 0 } } }
#  define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP \
  { { 0, 0, 0, 0, PTHREAD_MUTEX_ERRORCHECK_NP, 0, { 0, 0 } } }
#  define PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP \
  { { 0, 0, 0, 0, PTHREAD_MUTEX_ADAPTIVE_NP, 0, { 0, 0 } } }
# endif
#else
# define PTHREAD_MUTEX_INITIALIZER \
  { { 0, 0, 0, 0, 0, { 0 } } }
# ifdef __USE_GNU
#  define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP \
  { { 0, 0, 0, PTHREAD_MUTEX_RECURSIVE_NP, 0, { 0 } } }
#  define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP \
  { { 0, 0, 0, PTHREAD_MUTEX_ERRORCHECK_NP, 0, { 0 } } }
#  define PTHREAD_ADAPTIVE_MUTEX_INITIALIZER_NP \
  { { 0, 0, 0, PTHREAD_MUTEX_ADAPTIVE_NP, 0, { 0 } } }
# endif
#endif


/* Read-write lock types.  */
#if defined __USE_UNIX98 || defined __USE_XOPEN2K
enum
{
  PTHREAD_RWLOCK_PREFER_READER_NP,
  PTHREAD_RWLOCK_PREFER_WRITER_NP,
  PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP,
  PTHREAD_RWLOCK_DEFAULT_NP = PTHREAD_RWLOCK_PREFER_READER_NP
};

/* Define __PTHREAD_RWLOCK_INT_FLAGS_SHARED to 1 if pthread_rwlock_t
   has the shared field.  All 64-bit architectures have the shared field
   in pthread_rwlock_t.  */
#ifndef __PTHREAD_RWLOCK_INT_FLAGS_SHARED
# if __WORDSIZE == 64
#  define __PTHREAD_RWLOCK_INT_FLAGS_SHARED 1
# endif
#endif

/* Read-write lock initializers.  */
# define PTHREAD_RWLOCK_INITIALIZER \
  { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
# ifdef __USE_GNU
#  ifdef __PTHREAD_RWLOCK_INT_FLAGS_SHARED
#   define PTHREAD_RWLOCK_WRITER_NONRECURSIVE_INITIALIZER_NP \
  { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,					      \
        PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP } }
#  else
#   if __BYTE_ORDER == __LITTLE_ENDIAN
#    define PTHREAD_RWLOCK_WRITER_NONRECURSIVE_INITIALIZER_NP \
  { { 0, 0, 0, 0, 0, 0, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP, \
      0, 0, 0, 0 } }
#   else
#    define PTHREAD_RWLOCK_WRITER_NONRECURSIVE_INITIALIZER_NP \
  { { 0, 0, 0, 0, 0, 0, 0, 0, 0, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP,\
      0 } }
#   endif
#  endif
# endif
#endif  /* Unix98 or XOpen2K */


/* Scheduler inheritance.  */
enum
{
  PTHREAD_INHERIT_SCHED,
#define PTHREAD_INHERIT_SCHED   PTHREAD_INHERIT_SCHED
  PTHREAD_EXPLICIT_SCHED
#define PTHREAD_EXPLICIT_SCHED  PTHREAD_EXPLICIT_SCHED
};


/* Scope handling.  */
enum
{
  PTHREAD_SCOPE_SYSTEM,
#define PTHREAD_SCOPE_SYSTEM    PTHREAD_SCOPE_SYSTEM
  PTHREAD_SCOPE_PROCESS
#define PTHREAD_SCOPE_PROCESS   PTHREAD_SCOPE_PROCESS
};


/* Process shared or private flag.  */
enum
{
  PTHREAD_PROCESS_PRIVATE,
#define PTHREAD_PROCESS_PRIVATE PTHREAD_PROCESS_PRIVATE
  PTHREAD_PROCESS_SHARED
#define PTHREAD_PROCESS_SHARED  PTHREAD_PROCESS_SHARED
};



/* Conditional variable handling.  */
#define PTHREAD_COND_INITIALIZER { { 0, 0, 0, 0, 0, (void *) 0, 0, 0 } }


/* Cleanup buffers */
struct _pthread_cleanup_buffer
{
  void (*__routine) (void *);             /* Function to call.  */
  void *__arg;                            /* Its argument.  */
  int __canceltype;                       /* Saved cancellation type. */
  struct _pthread_cleanup_buffer *__prev; /* Chaining of cleanup functions.  */
};

/* Cancellation */
enum
{
  PTHREAD_CANCEL_ENABLE,
#define PTHREAD_CANCEL_ENABLE   PTHREAD_CANCEL_ENABLE
  PTHREAD_CANCEL_DISABLE
#define PTHREAD_CANCEL_DISABLE  PTHREAD_CANCEL_DISABLE
};
enum
{
  PTHREAD_CANCEL_DEFERRED,
#define PTHREAD_CANCEL_DEFERRED	PTHREAD_CANCEL_DEFERRED
  PTHREAD_CANCEL_ASYNCHRONOUS
#define PTHREAD_CANCEL_ASYNCHRONOUS	PTHREAD_CANCEL_ASYNCHRONOUS
};
#define PTHREAD_CANCELED ((void *) -1)


/* Single execution handling.  */
#define PTHREAD_ONCE_INIT 0


#ifdef __USE_XOPEN2K
/* Value returned by 'pthread_barrier_wait' for one of the threads after
   the required number of threads have called this function.
   -1 is distinct from 0 and all errno constants */
# define PTHREAD_BARRIER_SERIAL_THREAD -1
#endif


union pthread_attr_t
{
  char __size[__SIZEOF_PTHREAD_ATTR_T];
  long int __align;
};

#ifndef __have_pthread_attr_t
typedef union pthread_attr_t pthread_attr_t;
# define __have_pthread_attr_t	1
#endif

typedef struct __pthread_internal_slist
{
  struct __pthread_internal_slist *__next;
} __pthread_slist_t;

typedef union
{
  struct __pthread_mutex_s
  {
    int __lock;
    unsigned int __count;
    int __owner;
    /* KIND must stay at this position in the structure to maintain
       binary compatibility.  */
    int __kind;
    unsigned int __nusers;
    __extension__ union
    {
      int __spins;
      __pthread_slist_t __list;
    };
  } __data;
  char __size[__SIZEOF_PTHREAD_MUTEX_T];
  long int __align;
} pthread_mutex_t;

typedef union
{
  char __size[__SIZEOF_PTHREAD_MUTEXATTR_T];
  long int __align;
} pthread_mutexattr_t;


/* Data structure for conditional variable handling.  The structure of
   the attribute type is not exposed on purpose.  */
typedef union
{
  struct
  {
    int __lock;
    unsigned int __futex;
    __extension__ unsigned long long int __total_seq;
    __extension__ unsigned long long int __wakeup_seq;
    __extension__ unsigned long long int __woken_seq;
    void *__mutex;
    unsigned int __nwaiters;
    unsigned int __broadcast_seq;
  } __data;
  char __size[__SIZEOF_PTHREAD_COND_T];
  __extension__ long long int __align;
} pthread_cond_t;

typedef union
{
  char __size[__SIZEOF_PTHREAD_CONDATTR_T];
  long int __align;
} pthread_condattr_t;

struct pthread {
    //pthread_state		state;
    pthread_attr_t			attr;

    /* Signal interface */
    int				sigmask;
    int				sigpending;

    /* Time until timeout */
    //struct timespec			wakeup_time;

    /* Cleanup handlers Link List */
    struct pthread_cleanup 	*cleanup;

    /* Join queue for waiting threads */
   // struct pthread_queue	join_queue;

    /* Queue thread is waiting on, (mutexes, cond. etc.) */
    struct pthread_queue	*queue;

    /*
     * Thread implementations are just multiple queue type implemenations,
     * Below are the various link lists currently necessary
     * It is possible for a thread to be on multiple, or even all the
     * queues at once, much care must be taken during queue manipulation.
     *
     * The pthread structure must be locked before you can even look at
     * the link lists.
     */

    struct pthread			*pll;		/* ALL threads, in any state */
    /* struct pthread		*rll;		 Current run queue, before resced */
    struct pthread			*sll;		/* For sleeping threads */
    struct pthread			*next;		/* Standard for mutexes, etc ... */
    /* struct pthread			*fd_next;	 For kernel fd operations */

    int						fd;			/* Used when thread waiting on fd */

    int				lock;

    /* Data that doesn't need to be locked */
    void 					*ret;
    int						error;
    const void				**specific_data;
};


extern void* threadDataTable[64]; extern int freeEntry;
inline int pthread_key_create(pthread_key_t* key, void (*u)(void*)) {
        assert(freeEntry < 64);

        *key = freeEntry;
        freeEntry++;
        return 0;
}

inline int pthread_once(pthread_once_t* control, void (*init)(void)) {
        if (*control == 0) {
                (*init)();
                *control = 1;
        }
        return 0;
}

inline void* pthread_getspecific(pthread_key_t key) {
        return threadDataTable[key];
}

inline int pthread_setspecific(pthread_key_t key, const void* data) {
        threadDataTable[key] = (void*)data;
        return 0;
}

#ifdef __cplusplus
extern "C"
{
#endif

extern int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* u);

extern int pthread_mutex_lock(pthread_mutex_t* mutex);

extern int pthread_mutex_unlock(pthread_mutex_t* mutex);

#ifdef __cplusplus
}
#endif

inline int pthread_cond_wait(pthread_cond_t* v, pthread_mutex_t* u) {
        return 0;
}

inline int pthread_cond_signal(pthread_cond_t* u) {
        return 0;
}

#endif // PTHREAD_H
