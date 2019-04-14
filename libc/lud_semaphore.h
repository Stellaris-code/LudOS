/*
semaphore.h

Copyright (c) 23 Yann BOUCHER (yann)

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
#ifndef LUD_SEMAPHORE_H
#define LUD_SEMAPHORE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define SEM_FAILED ((lud_sem_t *)0)

typedef uint32_t    lud_sem_t;

//int    sem_close(sem_t *);
int    lud_sem_destroy(lud_sem_t *);
//int    sem_getvalue(sem_t *__restrict, int *__restrict);
int    lud_sem_init(lud_sem_t *, int, unsigned);
//sem_t *sem_open(const char *, int, ...);
int    lud_sem_post(lud_sem_t *);
//int    sem_timedwait(sem_t *__restrict, const struct timespec *__restrict);
int    lud_sem_trywait(lud_sem_t *);
//int    sem_unlink(const char *);
int    lud_sem_wait(lud_sem_t *);

#ifdef __cplusplus
}
#endif

#endif
