/*
pthreads.c

Copyright (c) 21 Yann BOUCHER (yann)

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

#ifdef LUDOS_USER

#include <stdlib.h>
#include <stdint.h>
#include <syscall.h>
#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include <sched.h>

#include <syscalls/syscall_list.hpp>

#include <sys/wait.h>

#define PTHREAD_STACK_SIZE 0x100000


int pthread_create(pthread_t * thread, pthread_attr_t * attr, void *(*start_routine)(void *), void * arg) {
        char * stack = malloc(PTHREAD_STACK_SIZE);
        uintptr_t stack_top = (uintptr_t)stack + PTHREAD_STACK_SIZE;
        thread->stack = stack;
        thread->id = clone(start_routine, (void*)stack_top, CLONE_VM | CLONE_FILES | CLONE_FS | CLONE_SIGHAND | CLONE_PARENT
                           | CLONE_THREAD | CLONE_IO, arg);
        return 0;
}

int pthread_kill(pthread_t thread, int sig)
{
    return kill(thread.id, sig);
}

void pthread_exit(void * value)
{
    exit((int)value);
}

void pthread_cleanup_push(void (*routine)(void *), void *arg) {
        /* do nothing */
}

void pthread_cleanup_pop(int execute) {
        /* do nothing */
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
        while (__sync_lock_test_and_set(mutex, 0x01)) {
                sched_yield();
        }
        return 0;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex) {
        if (__sync_lock_test_and_set(mutex, 0x01)) {
                return EBUSY;
        }
        return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
        __sync_lock_release(mutex);
        return 0;
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
        *mutex = 0;
        return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex) {
        return 0;
}

int pthread_attr_init(pthread_attr_t *attr) {
        *attr = 0;
        return 0;
}

int pthread_attr_destroy(pthread_attr_t *attr) {
        return 0;
}

int pthread_join(pthread_t thread, void **retval) {
        int status;
        int result = waitpid(thread.id, &status, 0);
        if (retval) {
                *retval = (void*)status;
        }

        if (result < 0)
            return -result;

        return 0;
}

#endif
