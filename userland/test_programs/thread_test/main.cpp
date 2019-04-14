/*
main.cpp

Copyright (c) 28 Yann BOUCHER (yann)

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


#if 0

#include <pthread.h>
#include <stdio.h>

#include <syscalls/syscall_list.hpp>

#define set_tls(val) \
    asm volatile ("movl %%gs:0, %%ecx\n" \
                  "movl %0, -256(%%ecx)"                                      \
                         : \
    : "a" (val) : "ecx");

#define get_tls() \
    ({ int val; \
    asm volatile ("movl %%gs:0, %%ecx\n"                                      \
                  "movl -256(%%ecx), %0" \
                         : "=a"(val) \
    ::"ecx"); \
    val; })

int x = 0, y = 0;

/* this function is run by the second thread */
void *inc_x(void *x_void_ptr)
{
    printf("tls is : 0x%x\n", get_tls());

    set_tls(0xdeadbeef);

    /* increment x to 100 */
    int *x_ptr = (int *)x_void_ptr;
    while(++(*x_ptr) < 5)
    {
        printf("x increment finished : %d %d (%p %p); tls : 0x%x\n", x, y, &x, &y, get_tls());
    };
    /* the function must return something - NULL will do */
    return (void*)42;
}

int main()
{

    //int x = 0, y = 0;

    /* show the initial values of x and y */
    printf("x: %d, y: %d; &x %p\n", x, y, &x);
    /* this variable is our reference to the second thread */
    pthread_t inc_x_thread;

    /* create a second thread which executes inc_x(&x) */
    if(pthread_create(&inc_x_thread, NULL, inc_x, (void*)&x)) {

        fprintf(stderr, "Error creating thread\n");
        return 1;

    }

    set_tls(0xcafebabe);
    /* increment y to 100 in the first thread */
    while(++y < 5)
    {
        printf("y increment finished : %d %d (%p %p), tls : 0x%x\n", x, y, &x, &y, get_tls());
    };

    /* wait for the second thread to finish */
    if(pthread_join(inc_x_thread, NULL)) {

        fprintf(stderr, "Error joining thread\n");
        return 2;

    }

    /* show the results - x is now 100 thanks to the second thread */
    printf("x: %d, y: %d, tls : 0x%x\n", x, y, get_tls());

    return 0;

}

#else


// C program to demonstrate working of Semaphores
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>

#include <syscalls/syscall_list.hpp>

sem_t mutex;

void* thread(void* arg)
{
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);

    tp.tv_sec  += 1;
    tp.tv_nsec += 500000000;

    //wait
    sem_timedwait(&mutex, &tp);
    printf("\nEntered..\n");

    sched_yield();

    //critical section
    //sleep(4);

    //signal
    printf("\nJust Exiting...\n");
}


int main()
{
    sem_init(&mutex, 0, 0);

    pthread_t t1;

    pthread_create(&t1,NULL,thread,NULL);

    pthread_join(t1,NULL);
    sem_destroy(&mutex);
    return 0;
}


#endif
