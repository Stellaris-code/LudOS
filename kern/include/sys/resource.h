
#ifndef _UAPI_LINUX_RESOURCE_H
#define _UAPI_LINUX_RESOURCE_H

#include <sys/types.h>

/*
 * Resource control/accounting header file for linux
 */

/*
 * Definition of struct rusage taken from BSD 4.3 Reno
 *
 * We don't support all of these yet, but we might as well have them....
 * Otherwise, each time we add new items, programs which depend on this
 * structure will lose.  This reduces the chances of that happening.
 */
#define	RUSAGE_SELF	0
#define	RUSAGE_CHILDREN	(-1)
#define RUSAGE_BOTH	(-2)		/* sys_wait4() uses this */
#define	RUSAGE_THREAD	1		/* only the calling thread */


#define	PRIO_MIN	(-20)
#define	PRIO_MAX	20

#define	PRIO_PROCESS	0
#define	PRIO_PGRP	1
#define	PRIO_USER	2

/*
 * Limit the stack by to some sane default: root can always
 * increase this limit if needed..  8MB seems reasonable.
 */
#define _STK_LIM	(8*1024*1024)

/*
 * GPG2 wants 64kB of mlocked memory, to make sure pass phrases
 * and other sensitive information are never written to disk.
 */
#define MLOCK_LIMIT	((PAGE_SIZE > 64*1024) ? PAGE_SIZE : 64*1024)


#endif /* _UAPI_LINUX_RESOURCE_H */
