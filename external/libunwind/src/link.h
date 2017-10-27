/* Data structure for communication from the run-time dynamic linker for
   loaded ELF shared objects.
   Copyright (C) 1995-1999, 2000, 2001 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#ifndef	_LINK_H
#define	_LINK_H	1

#include "config.h"
#include "features.h"
#include "elf.h"
#include "dlfcn.h"

#define DL_FIXUP_VALUE_TYPE ElfW(Addr)

#define DT_THISPROCNUM 0

#if __WORDSIZE == 64
struct link_map_machine
{
    Elf64_Addr plt; /* Address of .plt + 0x16 */
    Elf64_Addr gotplt; /* Address of .got + 0x18 */
    void *tlsdesc_table; /* Address of TLS descriptor hash table.  */
};
#else
struct link_map_machine
{
    Elf32_Addr plt; /* Address of .plt + 0x16 */
    Elf32_Addr gotplt; /* Address of .got + 0x0c */
    void *tlsdesc_table; /* Address of TLS descriptor hash table.  */
};
#endif

#define __RTLD_DLOPEN	0x80000000
#define __RTLD_SPROF	0x40000000
#define __RTLD_OPENEXEC	0x20000000
#define __RTLD_CALLMAP	0x10000000
#define __RTLD_AUDIT	0x08000000
#define __RTLD_SECURE	0x04000000 /* Apply additional security checks.  */
#define __RTLD_NOIFUNC	0x02000000 /* Suppress calling ifunc functions.  */

#define __LM_ID_CALLER	-2

#ifndef NO_TLS_OFFSET
# define NO_TLS_OFFSET        0
#endif
#ifndef FORCED_DYNAMIC_TLS_OFFSET
# if NO_TLS_OFFSET == 0
#  define FORCED_DYNAMIC_TLS_OFFSET -1
# elif NO_TLS_OFFSET == -1
#  define FORCED_DYNAMIC_TLS_OFFSET -2
# else
#  error "FORCED_DYNAMIC_TLS_OFFSET is not defined"
# endif
#endif

#define LM_ID_BASE      0     /* application link-map list */
#define LM_ID_LDSO      1     /* runtime linker link-map list */

/* We use this macro to refer to ELF types independent of the native wordsize.
   `ElfW(TYPE)' is used in place of `Elf32_TYPE' or `Elf64_TYPE'.  */
#define ElfW(type)	_ElfW (Elf, __ELF_NATIVE_CLASS, type)
#define _ElfW(e,w,t)	_ElfW_1 (e, w, _##t)
#define _ElfW_1(e,w,t)	e##w##t

#include "bits/elfclass.h"		/* Defines __ELF_NATIVE_CLASS.  */

/* Rendezvous structure used by the run-time dynamic linker to communicate
   details of shared object loading to the debugger.  If the executable's
   dynamic section has a DT_DEBUG element, the run-time linker sets that
   element's value to the address where this structure can be found.  */

struct r_debug
{
    int r_version;		/* Version number for this protocol.  */

    struct link_map *r_map;	/* Head of the chain of loaded objects.  */

    /* This is the address of a function internal to the run-time linker,
       that will always be called when the linker begins to map in a
       library or unmap it, and again when the mapping change is complete.
       The debugger can set a breakpoint at this address if it wants to
       notice shared object mapping changes.  */
    ElfW(Addr) r_brk;
    enum
    {
        /* This state value describes the mapping change taking place when
       the `r_brk' address is called.  */
        RT_CONSISTENT,		/* Mapping change is complete.  */
        RT_ADD,			/* Beginning to add a new object.  */
        RT_DELETE		/* Beginning to remove an object mapping.  */
    } r_state;

    ElfW(Addr) r_ldbase;	/* Base address the linker is loaded at.  */
};

/* This is the instance of that structure used by the dynamic linker.  */
extern struct r_debug _r_debug;

/* This symbol refers to the "dynamic structure" in the `.dynamic' section
   of whatever module refers to `_DYNAMIC'.  So, to find its own
   `struct r_debug', a program could do:
     for (dyn = _DYNAMIC; dyn->d_tag != DT_NULL; ++dyn)
       if (dyn->d_tag == DT_DEBUG)
     r_debug = (struct r_debug *) dyn->d_un.d_ptr;
   */
extern ElfW(Dyn) _DYNAMIC[];

/* Structure describing a loaded shared object.  The `l_next' and `l_prev'
   members form a chain of all the shared objects loaded at startup.

   These data structures exist in space used by the run-time dynamic linker;
   modifying them may have disastrous results.  */


struct link_map
{
    /* These first few members are part of the protocol with the debugger.
       This is the same format used in SVR4.  */

    ElfW(Addr) l_addr;		/* Base address shared object is loaded at.  */
    char *l_name;		/* Absolute file name object was found in.  */
    ElfW(Dyn) *l_ld;		/* Dynamic section of the shared object.  */
    struct link_map *l_next, *l_prev; /* Chain of loaded objects.  */

    ElfW(Addr) l_map_start, l_map_end;
    /* End of the executable part of the mapping.  */
    ElfW(Addr) l_text_end;

    /* All following members are internal to the dynamic linker.
       They may change without notice.  */

    /* This is an element which is only ever different from a pointer to
       the very same copy of this type for ld.so when it is used in more
       than one namespace.  */


    ElfW(Dyn) *l_info[DT_NUM + DT_THISPROCNUM + DT_VERSIONTAGNUM
    + DT_EXTRANUM + DT_VALNUM + DT_ADDRNUM];

    struct link_map *l_real;
    const ElfW(Phdr) *l_phdr;	/* Pointer to program header table in core.  */
    ElfW(Addr) l_entry;		/* Entry point location.  */
    ElfW(Half) l_phnum; /* Number of program header entries.  */

    unsigned int l_direct_opencount; /* Reference count for dlopen/dlclose.  */
    enum                        /* Where this object came from.  */
    {
        lt_executable,                /* The main executable program.  */
        lt_library,                /* Library needed by main executable.  */
        lt_loaded                /* Extra run-time loaded shared object.  */
    } l_type:2;
    unsigned int l_relocated:1;        /* Nonzero if object's relocations done.  */
    unsigned int l_init_called:1; /* Nonzero if DT_INIT function called.  */
    unsigned int l_global:1;        /* Nonzero if object in _dl_global_scope.  */
    unsigned int l_reserved:2;        /* Reserved for internal use.  */
    unsigned int l_phdr_allocated:1; /* Nonzero if the data structure pointed
                                        to by `l_phdr' is allocated.  */
    unsigned int l_soname_added:1; /* Nonzero if the SONAME is for sure in
                                      the l_libname list.  */
    unsigned int l_faked:1;        /* Nonzero if this is a faked descriptor
                                   without associated file.  */
    unsigned int l_need_tls_init:1; /* Nonzero if GL(dl_init_static_tls)
                                       should be called on this link map
                                       when relocation finishes.  */
    unsigned int l_auditing:1;        /* Nonzero if the DSO is used in auditing.  */
    unsigned int l_audit_any_plt:1; /* Nonzero if at least one audit module
                                       is interested in the PLT interception.*/
    unsigned int l_removed:1;        /* Nozero if the object cannot be used anymore
                                   since it is removed.  */
    unsigned int l_contiguous:1; /* Nonzero if inter-segment holes are
                                    mprotected or if no holes are present at
                                    all.  */
    unsigned int l_symbolic_in_local_scope:1; /* Nonzero if l_local_scope
                                                 during LD_TRACE_PRELINKING=1
                                                 contains any DT_SYMBOLIC
                                                 libraries.  */
    unsigned int l_free_initfini:1; /* Nonzero if l_initfini can be
                                       freed, ie. not allocated with
                                       the dummy malloc in ld.so.  */

    /* Number of the namespace this link map belongs to.  */
    Lmid_t l_ns;
    Elf_Symndx l_nbuckets;
    Elf32_Word l_gnu_bitmask_idxbits;
    Elf32_Word l_gnu_shift;
    struct r_scope_elem *l_scope_mem[4];
    /* Size of array allocated for 'l_scope'.  */
    size_t l_scope_max;
    /* This is an array defining the lookup scope for this link map.
       There are initially at most three different scope lists.  */
    struct r_scope_elem **l_scope;

    unsigned long long int l_serial;

    int *l_versyms;



    struct link_map **l_initfini;
    /* The init and fini list generated at startup, saved when the
       object is also loaded dynamically.  */
    struct link_map **l_orig_initfini;
    struct link_map_reldeps
    {
        unsigned int act;
        struct link_map *list[];
    } *l_reldeps;
    unsigned int l_reldepsmax;

    /* Nonzero if the DSO is used.  */
    unsigned int l_used;

    /* Various flag words.  */
    ElfW(Word) l_feature_1;
    ElfW(Word) l_flags_1;
    ElfW(Word) l_flags;

    /* A similar array, this time only with the local scope.  This is
       used occasionally.  */
    struct r_scope_elem *l_local_scope[2];
    const ElfW(Addr) *l_gnu_bitmask;
    union
    {
        const Elf32_Word *l_gnu_buckets;
        const Elf_Symndx *l_chain;
    };
    union
    {
        const Elf32_Word *l_gnu_chain_zero;
        const Elf_Symndx *l_buckets;
    };


    struct
    {
        const ElfW(Sym) *sym;
        int type_class;
        struct link_map *value;
        const ElfW(Sym) *ret;
    } l_lookup_cache;

    void *l_tls_initimage;
    /* Size of the initialization image.  */
    size_t l_tls_initimage_size;
    /* Size of the TLS block.  */
    size_t l_tls_blocksize;
    /* Alignment requirement of the TLS block.  */
    size_t l_tls_align;
    /* Offset of first byte module alignment.  */
    size_t l_tls_firstbyte_offset;

    /* For objects present at startup time: offset in the static TLS block.  */
    ptrdiff_t l_tls_offset;
    /* Index of the module in the dtv array.  */
    size_t l_tls_modid;
    /* Number of thread_local objects constructed by this DSO.  This is
       atomically accessed and modified and is not always protected by the load
       lock.  See also: CONCURRENCY NOTES in cxa_thread_atexit_impl.c.  */
    size_t l_tls_dtor_count;

    struct link_map_machine l_mach;

    struct reloc_result
    {
        DL_FIXUP_VALUE_TYPE addr;
        struct link_map *bound;
        unsigned int boundndx;
        uint32_t enterexit;
        unsigned int flags;
    } *l_reloc_result;

    struct r_found_version *l_versions;
    unsigned int l_nversions;

    ElfW(Addr) l_relro_addr;
    size_t l_relro_size;
};

struct r_scope_elem
{
    /* Array of maps for the scope.  */
    struct link_map **r_list;
    /* Number of entries in the scope.  */
    unsigned int r_nlist;
};


#ifdef __USE_GNU

struct dl_phdr_info
{
    ElfW(Addr) dlpi_addr;
    const char *dlpi_name;
    const ElfW(Phdr) *dlpi_phdr;
    ElfW(Half) dlpi_phnum;

    /* Note: Following members were introduced after the first
       version of this structure was available.  Check the SIZE
       argument passed to the dl_iterate_phdr callback to determine
       whether or not each later member is available.  */

    /* Incremented when a new object may have been added.  */
    unsigned long long int dlpi_adds;
    /* Incremented when an object may have been removed.  */
    unsigned long long int dlpi_subs;

    /* If there is a PT_TLS segment, its module ID as used in
       TLS relocations, else zero.  */
    size_t dlpi_tls_modid;

    /* The address of the calling thread's instance of this module's
       PT_TLS segment, if it has one and it has been allocated
       in the calling thread, otherwise a null pointer.  */
    void *dlpi_tls_data;

    ElfW(Addr) l_relro_addr;
    size_t l_relro_size;

};

#ifdef __cplusplus
extern "C"
{
#endif

extern int dl_iterate_phdr (int (*callback) (struct dl_phdr_info *info,
                                             unsigned long size, void *data),
                            void *data);

#ifdef __cplusplus
}
#endif

#endif

#endif /* link.h */
