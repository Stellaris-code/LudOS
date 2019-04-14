/*
 *      HermitCore port of POSIX Threads Library for embedded systems
 *      Copyright(C) 2015 Stefan Lankes, RWTH Aachen Univeristy
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
 */

/*
 * This port is derived from psp_osal.c.
 */

#define LUDOS_USER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <sys/timeb.h>
#include <sys/resource.h>

#include <syscalls/syscall_list.hpp>
#include <tasking/atomic.hpp>

#include "pte_osal.h"
#include "pthread.h"

#include <sched.h>
#include <lud_semaphore.h>

#define tls_addr() \
    ({ int val; \
    asm volatile ("mov %%gs:0, %0"                                      \
    : "=a"(val) \
    :); \
    val; })

/*
 * Data stored on a per-thread basis - allocated in pte_osThreadCreate
 * and freed in pte_osThreadDelete.
 */
typedef struct thread_data
{
    /* Entry point and parameters to thread's main function */
    pte_osThreadEntryPoint entryPoint;
    void* argv;
    void* stack;
    unsigned int stack_size;

    /* id of the thread */
    pid_t pid;

    /* Semaphore to start and to cancel the thread */
    pte_osSemaphoreHandle start_sem;
    pte_osSemaphoreHandle cancel_sem;

    int done;

    bool used;
    int handle_id;
} thread_data;

thread_data* thread_list;
size_t thread_count;
size_t thread_capacity;
size_t tls_key_count;

/****************************************************************************
 *
 * Initialization
 *
 ***************************************************************************/

int pte_kill(pte_osThreadHandle threadId, int sig)
{
    return kill(threadId, sig);
}

int find_free_pid()
{
    // search for a free slot
    for (size_t i = 0; i < thread_capacity; ++i)
    {
        if (!thread_list[i].used)
            return i;
    }

    // FIXME : realloc() isn't thread-safe because it can change addresses currently in use in other threads !!
    // FIXME : find another way to expand the list
    // no free slots available, expand thread_list
    thread_capacity += 16;
    thread_list = realloc(thread_list, sizeof(thread_data) * thread_capacity);
    memset(thread_list + thread_capacity - 16, 0, 16 * sizeof(thread_data));

    return find_free_pid();
}

int alloc_structure(int stack_size)
{
    int id = find_free_pid();
    thread_data* entry = &thread_list[id];

    assert(!entry->used);

    entry->stack = malloc_align(stack_size, 0x10);

    entry->stack_size = stack_size;

    entry->start_sem = NULL;

    pte_osSemaphoreCreate(0, &entry->start_sem);
    pte_osSemaphoreCreate(0, &entry->cancel_sem);

    entry->handle_id = id;
    entry->used = true;

    ++thread_count;

    return id;
}

int get_structure(int pid)
{
    for (size_t i = 0; i < thread_capacity; ++i)
    {
        if (thread_list[i].pid == pid)
        {
            assert(thread_list[i].used);
            return i;
        }
    }

    //printf(" ------------ pthread warning : pid not found for pid %d\n", pid);
    return -1;
}


int delete_structure(int id)
{
    assert(id < thread_capacity);

    thread_data* data = &thread_list[id];
    assert(data->used);
    assert(data->handle_id == id);

    pte_osSemaphoreDelete(data->start_sem);
    pte_osSemaphoreDelete(data->cancel_sem);

    free(data->stack);

    data->used = false;
    data->pid  = -1;

    --thread_count;
}

static int thread_entry_stub(void *argv)
{
    int th_id = get_structure(pte_osThreadGetHandle());
    assert(th_id >= 0);
    //int ret;
    thread_data* handle = &thread_list[th_id];

    if (!handle)
    {
        exit(-PTE_OS_NO_RESOURCES);
    }

    assert(handle->used);
    assert(!handle->done);
    assert(handle->pid == gettid());

    // clear tls
    uint8_t* addr = (uint8_t*)tls_addr();
    memset(addr - 0x1000, 0, 0x1000);

    /* wait for the resume command */
    pte_osSemaphorePend(handle->start_sem, NULL);

    int ret = handle->entryPoint(handle->argv);

    // never reached

    assert(false);

    pte_osThreadExit();

    while(1);
}

static uint8_t self_destruct_stack[0x1000];
static pthread_mutex_t self_destruct_mutex;
// exit and release the resources at the same time
pte_osResult self_destruct()
{
    pthread_mutex_lock(&self_destruct_mutex);

    asm volatile ("mov %0, %%esp"::"r"((uint8_t*)&self_destruct_stack + sizeof(self_destruct_stack)):);

    delete_structure(get_structure(pte_osThreadGetHandle()));

    pthread_mutex_unlock(&self_destruct_mutex);

    exit(0);
}

/****************************************************************************
 *
 * Threads
 *
 ***************************************************************************/

pte_osResult pte_osInit(void)
{
    thread_capacity = 16;
    thread_list = calloc(sizeof(thread_data) * thread_capacity, sizeof(thread_data));

    // clear current thread's tls
    uint8_t* addr = (uint8_t*)tls_addr();
    memset(addr - 0x1000, 0, 0x1000);
    tls_key_count = 0;

    pthread_mutex_init(&self_destruct_mutex, NULL);

    // allocate thread structure for the first thread which is the native OS thread
    int id = alloc_structure(0); // no stack
    if (id < 0)
    {
        printf("pthread os layer structure allocation error\n");
        return PTE_OS_NO_RESOURCES;
    }
    assert(id == 0);
    thread_list[id].pid = gettid();

    return PTE_OS_OK;
}

pte_osResult pte_osThreadCreate(pte_osThreadEntryPoint entryPoint,
                                int stackSize,
                                int initialPriority,
                                void *argv,
                                pte_osThreadHandle* handle_pointer)
{
    /* Make sure that the stack we're going to allocate is big enough */
    if (stackSize < 0x1000)
    {
        stackSize = 0x1000;
    }

    //printf("starting the alloc\n");

    int id = alloc_structure(stackSize);
    if (id < 0)
    {
        printf("pthread os layer structure allocation error\n");
        return PTE_OS_NO_RESOURCES;
    }
    thread_data* handle = &thread_list[id];
    assert(handle->used);

    handle->entryPoint = entryPoint;
    handle->argv = argv;
    handle->done = 0;

    int ret = clone(thread_entry_stub, handle->stack + handle->stack_size, CLONE_VM | CLONE_FILES | CLONE_FS | CLONE_SIGHAND | CLONE_PARENT | CLONE_THREAD | CLONE_IO,
                    argv);

    if (ret < 0)
    {
        printf("clone error : %s\n", strerror(errno));
        delete_structure(id);
        return PTE_OS_NO_RESOURCES;
    }

    *handle_pointer = handle->pid = ret;

    assert(pte_osThreadSetPriority(handle->pid, initialPriority) == PTE_OS_OK);

    return PTE_OS_OK;
}


pte_osResult pte_osThreadStart(pte_osThreadHandle handle)
{
    /* wake up thread */
    int th_id = get_structure(handle);
    if (th_id < 0) // invalid
        return PTE_OS_INVALID_PARAM;
    return pte_osSemaphorePost(thread_list[th_id].start_sem, 1);
}


pte_osResult pte_osThreadDelete(pte_osThreadHandle handle)
{
    assert(handle != gettid());

    /* free ressources */
    int th_id = get_structure(handle);
    if (th_id < 0) // invalid
        return PTE_OS_INVALID_PARAM;

    assert(thread_list[th_id].done);

    delete_structure(thread_list[th_id].handle_id);

    sched_yield();

    return PTE_OS_OK;
}

pte_osResult pte_osThreadExitAndDelete(pte_osThreadHandle handle)
{
    if (handle == gettid())
        return self_destruct();

    int th_id = get_structure(handle);
    if (th_id < 0) // invalid
        return PTE_OS_INVALID_PARAM;

    atomic_store(&thread_list[th_id].done, 1);

    pte_kill(handle, SIGTERM);
    pte_osThreadDelete(handle);

    return PTE_OS_OK;
}

void pte_osThreadExit(void)
{
    int th_id = get_structure(pte_osThreadGetHandle());
    assert(th_id >= 0);

    //printf("exiting slot %d as pid %d\n", th_id, pte_osThreadGetHandle());

    atomic_store(&thread_list[th_id].done, 1);
    exit(0);
}

/*
 * This has to be cancellable, we currently ignore this behavious.
 */
pte_osResult pte_osThreadWaitForEnd(pte_osThreadHandle tid)
{
    int th_id = get_structure(tid);
    if (th_id < 0)
        return PTE_OS_INVALID_PARAM;

    thread_data* data = &thread_list[th_id];
    thread_data* self = NULL;

    int self_th_id = get_structure(gettid());
    if (self_th_id >= 0)
        self = &thread_list[self_th_id];

    pte_osResult result = PTE_OS_OK;

#if 0
    int status;
    ret = waitpid(tid, &status, 0);
    if (ret < 0)
        result = PTE_OS_GENERAL_FAILURE;
#else
    while (1)
    {
        int count;

        if (atomic_load(&data->done))
        {
            return PTE_OS_OK;
        }
        if (self == NULL)
        {
            pte_osYield();
            continue;
        }

        if (lud_sem_getvalue(&self->cancel_sem, &count) == 0)
        {
            if (count > 0)
            {
                result = PTE_OS_INTERRUPTED;
                break;
            }
            else
            {
                /* Nothing found and not timed out yet; let's yield so we're not
                   * in busy loop.
                   */
                pte_osYield();
            }
        }
        else
        {
            result = PTE_OS_GENERAL_FAILURE;
            break;
        }
    }
#endif

    return result;
}

pte_osResult pte_osThreadCancel(pte_osThreadHandle threadHandle)
{
    int th_id = get_structure(threadHandle);
    assert(th_id >= 0);

    pte_osSemaphorePost(thread_list[th_id].cancel_sem, 1);
    return PTE_OS_OK;
}

pte_osResult pte_osThreadCheckCancel(pte_osThreadHandle threadHandle)
{
    int th_id = get_structure(threadHandle);
    assert(th_id >= 0);

    int count;
    lud_sem_getvalue(&thread_list[th_id].cancel_sem, &count);

    if (count)
        return PTE_OS_INTERRUPTED;
    else
        return PTE_OS_OK;
}

pte_osThreadHandle pte_osThreadGetHandle(void)
{
    return gettid();
}

int pte_osThreadGetPriority(pte_osThreadHandle threadHandle)
{
    return getpriority(PRIO_PROCESS, threadHandle);
}

pte_osResult pte_osThreadSetPriority(pte_osThreadHandle threadHandle, int newPriority)
{
    if (setpriority(PRIO_PROCESS, threadHandle, newPriority) < 0)
        return PTE_OS_INVALID_PARAM;

    return PTE_OS_OK;
}

void pte_osThreadSleep(unsigned int msecs)
{
    struct timespec req;
    req.tv_sec  = msecs/1000;
    req.tv_nsec = (msecs%1000) * 1000000;
    nanosleep(&req, NULL);
}

int pte_osThreadGetMinPriority(void)
{
    return OS_MIN_PRIO;
}

int pte_osThreadGetMaxPriority(void)
{
    return OS_MAX_PRIO;
}

int pte_osThreadGetDefaultPriority(void)
{
    return OS_DEFAULT_PRIO;
}

/****************************************************************************
 *
 * Mutexes
 *
 ****************************************************************************/

pte_osResult pte_osMutexCreate(pte_osMutexHandle *pHandle)
{
    if (lud_sem_init(pHandle, 0, 1))
        return PTE_OS_NO_RESOURCES;

    return PTE_OS_OK;
}

pte_osResult pte_osMutexDelete(pte_osMutexHandle handle)
{
    if (lud_sem_destroy(&handle))
        return PTE_OS_GENERAL_FAILURE;

    return PTE_OS_OK;
}

pte_osResult pte_osMutexLock(pte_osMutexHandle handle)
{
    if (lud_sem_wait(&handle))
        return PTE_OS_GENERAL_FAILURE;

    return PTE_OS_OK;
}

pte_osResult pte_osMutexTimedLock(pte_osMutexHandle handle, unsigned int timeoutMsecs)
{
    struct timespec req;
    req.tv_sec  = timeoutMsecs/1000;
    req.tv_nsec = (timeoutMsecs%1000) * 1000000;

    if (lud_sem_timedwait(&handle, &req) < 0)
    {
        if (errno == ETIMEDOUT)
            return PTE_OS_TIMEOUT;
        else
            return PTE_OS_GENERAL_FAILURE;
    }
}


pte_osResult pte_osMutexUnlock(pte_osMutexHandle handle)
{
    if (lud_sem_post(&handle))
        return PTE_OS_GENERAL_FAILURE;

    return PTE_OS_OK;
}

/****************************************************************************
 *
 * Semaphores
 *
 ***************************************************************************/

pte_osResult pte_osSemaphoreCreate(int initialValue, pte_osSemaphoreHandle *pHandle)
{
    if (lud_sem_init(pHandle, 0, initialValue) < 0)
        return PTE_OS_GENERAL_FAILURE;

    return PTE_OS_OK;
}

pte_osResult pte_osSemaphoreDelete(pte_osSemaphoreHandle handle)
{
    if (lud_sem_destroy(&handle) < 0)
        return PTE_OS_GENERAL_FAILURE;

    return PTE_OS_OK;
}

pte_osResult pte_osSemaphorePost(pte_osSemaphoreHandle handle, int count)
{
    int i;

    for (i=0; i<count; i++) {
        if (lud_sem_post(&handle) < 0)
            return PTE_OS_GENERAL_FAILURE;
    }

    return PTE_OS_OK;
}

pte_osResult pte_osSemaphorePend(pte_osSemaphoreHandle handle, unsigned int *pTimeoutMsecs)
{
    if (pTimeoutMsecs && *pTimeoutMsecs)
    {
        struct timespec req;
        req.tv_sec  = *pTimeoutMsecs/1000;
        req.tv_nsec = (*pTimeoutMsecs%1000) * 1000000;

        if (lud_sem_timedwait(&handle, &req) < 0)
        {
            if (errno == ETIMEDOUT)
                return PTE_OS_TIMEOUT;
            else
                return PTE_OS_GENERAL_FAILURE;
        }
    } else if (pTimeoutMsecs && *pTimeoutMsecs == 0) { // try to acquire it immediatly
        if (lud_sem_trywait(&handle) < 0 && errno == EAGAIN)
            return PTE_OS_TIMEOUT;
    }
    else
    {
        if (lud_sem_wait(&handle) < 0)
            return PTE_OS_GENERAL_FAILURE;
    }

    return PTE_OS_OK;
}


/*
 * Pend on a semaphore- and allow the pend to be cancelled.
 */
pte_osResult pte_osSemaphoreCancellablePend(pte_osSemaphoreHandle semHandle, unsigned int *pTimeout)
{
    //return pte_osSemaphorePend(semHandle, pTimeout);

    int th_id = get_structure(pte_osThreadGetHandle());
    assert(th_id >= 0);
    thread_data* data = &thread_list[th_id];
    // copy the sem id, we can't directly use the address of the thread_struct
    // because it may be changed between calls because of a reallocation
    lud_sem_t cancel_sem_id = data->cancel_sem;

    int result;
    unsigned start_millisec;

    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    start_millisec = tp.tv_sec*1000 + tp.tv_nsec / 1000000;

    while (1)
    {
        int status = -1;

        /* Poll semaphore */
        status = lud_sem_trywait(&semHandle);

        if (pTimeout)
        {
            clock_gettime(CLOCK_REALTIME, &tp);
            unsigned current_millisec = tp.tv_sec*1000 + tp.tv_nsec / 1000000;
            /* Timeout expired */
            if (current_millisec - start_millisec > *pTimeout)
            {
                result = PTE_OS_TIMEOUT;
                break;
            }
        }

        if (status == 0)
        {
            /* User semaphore posted to */
            result = PTE_OS_OK;
            break;
        }
        else
        {
            int count;
            if (lud_sem_getvalue(&cancel_sem_id, &count) == 0)
            {
                if (count > 0)
                {
                    result = PTE_OS_INTERRUPTED;
                    break;
                }
            }
            else
            {
                result = PTE_OS_GENERAL_FAILURE;
                break;
            }
        }

        pte_osYield();
    }

    return result;
}

/****************************************************************************
 *
 * Atomic Operations
 *
 ***************************************************************************/

int pte_osAtomicExchange(int *ptarg, int val)
{
    return __atomic_exchange_n(ptarg, val, __ATOMIC_SEQ_CST);
}

int pte_osAtomicCompareExchange(int *pdest, int exchange, int comp)
{
    __atomic_compare_exchange_n(pdest, &comp, exchange, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
    return comp;
}

int pte_osAtomicExchangeAdd(int volatile* pAddend, int value)
{
    return __atomic_fetch_add(pAddend, value, __ATOMIC_SEQ_CST);
}

int pte_osAtomicDecrement(int *pdest)
{
    return __atomic_sub_fetch(pdest, 1, __ATOMIC_SEQ_CST);
}

int pte_osAtomicIncrement(int *pdest)
{
    return __atomic_add_fetch(pdest, 1, __ATOMIC_SEQ_CST);
}

/****************************************************************************
 *
 * Thread Local Storage
 *
 ***************************************************************************/

pte_osResult pte_osTlsSetValue(unsigned int key, void * value)
{
    assert(key < tls_key_count);

    uint32_t* addr = (uint32_t*)tls_addr();
    *(addr - 1 - key) = (uint32_t)value;
    return PTE_OS_OK;
}

void * pte_osTlsGetValue(unsigned int key)
{
    if (key >= tls_key_count)
    {
        printf("invalid : %d / %d, PID %d\n", key, tls_key_count, gettid());
    }

    assert(key < tls_key_count);

    uint32_t* addr = (uint32_t*)tls_addr();

    return (void *)*(addr - 1 - key);
}


pte_osResult pte_osTlsAlloc(unsigned int *pKey)
{
    *pKey = atomic_inc(&tls_key_count);

    assert(tls_key_count < 0x1000/sizeof(unsigned int));

    pte_osTlsSetValue(*pKey, NULL);

    return PTE_OS_OK;
}

pte_osResult pte_osTlsFree(unsigned int index)
{
    pte_osTlsSetValue(index, NULL);

    // NOP currently
    return PTE_OS_OK;
}

/***************************************************************************
 *
 * Miscellaneous
 *
 ***************************************************************************/

int ftime(struct timeb *tb)
{
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);

    tb->time = tp.tv_sec;
    tb->millitm = tp.tv_nsec / 1000000;

    return 0;
}
