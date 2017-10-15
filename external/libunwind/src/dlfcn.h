/*
 * dlfcn-win32
 * Copyright (c) 2007 Ramiro Polla
 *
 * dlfcn-win32 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * dlfcn-win32 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with dlfcn-win32; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef DLFCN_H
#define DLFCN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(DLFCN_WIN32_EXPORTS)
#   define DLFCN_EXPORT __declspec(dllexport)
#else
#   define DLFCN_EXPORT
#endif

/* POSIX says these are implementation-defined.
 * To simplify use with Windows API, we treat them the same way.
 */

#define RTLD_LAZY   0
#define RTLD_NOW    0

#define RTLD_GLOBAL (1 << 1)
#define RTLD_LOCAL  (1 << 2)

typedef long int Lmid_t ;

/* These two were added in The Open Group Base Specifications Issue 6.
 * Note: All other RTLD_* flags in any dlfcn.h are not standard compliant.
 */

#define RTLD_DEFAULT    0
#define RTLD_NEXT       0

DLFCN_EXPORT void *dlopen ( const char *file, int mode );
DLFCN_EXPORT int   dlclose(void *handle);
DLFCN_EXPORT void *dlsym(void *handle, const char *name);
DLFCN_EXPORT char *dlerror(void);

typedef struct
{
    const char * dli_fname;
    void       * dli_fbase;
    const char * dli_sname;
    void       * dli_saddr;
    int          dli_version;
    int          dli_reserved1;
    long         dli_reserved[4];
} Dl_info;

typedef struct
{
  char *dls_name;                /* Name of library search path directory.  */
  unsigned int dls_flags;        /* Indicates where this directory came from. */
} Dl_serpath;
/* This is the structure that must be passed (by reference) to `dlinfo' for
   the RTLD_DI_SERINFO and RTLD_DI_SERINFOSIZE requests.  */
typedef struct
{
  size_t dls_size;                /* Size in bytes of the whole buffer.  */
  unsigned int dls_cnt;                /* Number of elements in `dls_serpath'.  */
  Dl_serpath dls_serpath[1];        /* Actually longer, dls_cnt elements.  */
} Dl_serinfo;

DLFCN_EXPORT int dladdr(void*, Dl_info*);

#ifdef __cplusplus
}
#endif

#endif /* DLFCN_H */
