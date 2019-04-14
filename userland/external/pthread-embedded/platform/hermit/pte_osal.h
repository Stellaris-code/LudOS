#ifndef _OS_SUPPORT_H_
#define _OS_SUPPORT_H_

#include <stddef.h>
#include <stdint.h>

typedef void* pte_osThreadHandle;
typedef void* pte_osSemaphoreHandle;
typedef void* pte_osMutexHandle;

#define OS_MAX_SIMUL_THREADS 32
#define OS_DEFAULT_PRIO 8
#define OS_MIN_PRIO 1
#define OS_MAX_PRIO 31

#include "pte_generic_osal.h"

#endif // _OS_SUPPORT_H
