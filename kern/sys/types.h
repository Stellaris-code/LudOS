/*
types.h

Copyright (c) 01 Yann BOUCHER (yann)

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
#ifndef LUDOS_TYPES_H
#define LUDOS_TYPES_H

#include <stdint.h>

typedef int pid_t;

typedef int64_t	blkcnt_t;	/* blocks allocated for file */
typedef int32_t	blksize_t;	/* optimal blocksize for I/O */
typedef	int64_t	clock_t;	/* ticks in CLOCKS_PER_SEC */
typedef	int32_t	clockid_t;	/* CLOCK_* identifiers */
typedef	unsigned long	cpuid_t;	/* CPU id */
typedef	int32_t	dev_t;	/* device number */
typedef	uint32_t	fixpt_t;	/* fixed point number */
typedef	uint64_t	fsblkcnt_t;	/* file system block count */
typedef	uint64_t	fsfilcnt_t;	/* file system file count */
typedef	uint32_t	gid_t;	/* group id */
typedef	uint32_t	id_t;		/* may contain pid, uid or gid */
typedef	uint32_t	in_addr_t;	/* base type for internet address */
typedef	uint16_t	in_port_t;	/* IP port type */
typedef	uint64_t	ino_t;	/* inode number */
typedef	long		key_t;	/* IPC key (for Sys V IPC) */
typedef	uint32_t	mode_t;	/* permissions */
typedef	uint32_t	nlink_t;	/* link count */
typedef	int64_t	off_t;	/* file offset or size */
typedef	int32_t	pid_t;	/* process id */
typedef	uint64_t	rlim_t;	/* resource limit */
typedef	uint8_t	sa_family_t;	/* sockaddr address family type */
typedef	int32_t	segsz_t;	/* segment size */
typedef	uint32_t	socklen_t;	/* length type for network syscalls */
typedef	long		suseconds_t;	/* microseconds (signed) */
typedef	int32_t	swblk_t;	/* swap offset */
typedef	int64_t	time_t;	/* epoch time */
typedef	int32_t	timer_t;	/* POSIX timer identifiers */
typedef	uint32_t	uid_t;	/* user id */
typedef	uint32_t	useconds_t; /* microseconds */

#endif // TYPES_H
