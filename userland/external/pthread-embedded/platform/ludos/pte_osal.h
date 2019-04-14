#ifndef _OS_SUPPORT_H_
#define _OS_SUPPORT_H_

#include <stddef.h>
#include <stdint.h>

#include <sys/types.h>

typedef struct thread_data thread_data;

typedef pid_t pte_osThreadHandle;
typedef unsigned int pte_osSemaphoreHandle;
typedef unsigned int pte_osMutexHandle;

#define OS_MAX_SIMUL_THREADS 1024
#define OS_DEFAULT_PRIO 20
#define OS_MIN_PRIO 1
#define OS_MAX_PRIO 40

#include "pte_generic_osal.h"

#endif // _OS_SUPPORT_H
