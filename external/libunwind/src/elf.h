#ifndef _UAPI_LINUX_ELF_H
#define _UAPI_LINUX_ELF_H

#include <stdint.h>

/* 32-bit ELF base types. */
typedef uint32_t	Elf32_Addr;
typedef uint16_t	Elf32_Half;
typedef uint32_t	Elf32_Off;
typedef int32_t	Elf32_Sword;
typedef uint32_t	Elf32_Word;

/* 64-bit ELF base types. */
typedef uint64_t	Elf64_Addr;
typedef uint16_t	Elf64_Half;
typedef int16_t	Elf64_SHalf;
typedef uint64_t	Elf64_Off;
typedef int32_t	Elf64_Sword;
typedef uint32_t	Elf64_Word;
typedef uint64_t	Elf64_Xword;
typedef int64_t	Elf64_Sxword;

#define ELF32_ST_VISIBILITY(o)	((o) & 0x03)

/* For ELF64 the definitions are the same.  */
#define ELF64_ST_VISIBILITY(o)	ELF32_ST_VISIBILITY (o)

/* Symbol visibility specification encoded in the st_other field.  */
#define STV_DEFAULT	0		/* Default symbol visibility rules */
#define STV_INTERNAL	1		/* Processor specific hidden class */
#define STV_HIDDEN	2		/* Sym unavailable in other modules */
#define STV_PROTECTED	3	/* Not preemptible, not exported */


#define STT_GNU_IFUNC	10

#define	R_386_NONE		0	/* relocation type */
#define	R_386_32		1
#define	R_386_PC32		2
#define	R_386_GOT32		3
#define	R_386_PLT32		4
#define	R_386_COPY		5
#define	R_386_GLOB_DAT		6
#define	R_386_JMP_SLOT		7
#define	R_386_RELATIVE		8
#define	R_386_IRELATIVE		88
#define	R_386_GOTOFF		9
#define	R_386_GOTPC		10
#define	R_386_32PLT		11
#define	R_386_TLS_GD_PLT	12
#define	R_386_TLS_LDM_PLT	13
#define	R_386_TLS_TPOFF		14
#define	R_386_TLS_IE		15
#define	R_386_TLS_GOTIE		16
#define	R_386_TLS_LE		17
#define	R_386_TLS_GD		18
#define	R_386_TLS_LDM		19
#define	R_386_16		20
#define	R_386_PC16		21
#define	R_386_8			22
#define	R_386_PC8		23
#define	R_386_UNKNOWN24		24
#define	R_386_UNKNOWN25		25
#define	R_386_UNKNOWN26		26
#define	R_386_UNKNOWN27		27
#define	R_386_UNKNOWN28		28
#define	R_386_UNKNOWN29		29
#define	R_386_UNKNOWN30		30
#define	R_386_UNKNOWN31		31
#define	R_386_TLS_LDO_32	32
#define	R_386_UNKNOWN33		33
#define	R_386_UNKNOWN34		34
#define	R_386_TLS_DTPMOD32	35
#define	R_386_TLS_DTPOFF32	36
#define	R_386_TLS_TPOFF32	90
#define	R_386_UNKNOWN37		37
#define	R_386_SIZE32		38
#define	R_386_NUM	39
#define	R_386_TLS_DESC	89

typedef struct
{
    uint32_t a_type;           /* Entry type */
    union
    {
        uint32_t a_val;                /* Integer value */
        /* We use to have pointer elements added here.  We cannot do that,
            though, since it does not work when using 32-bit definitions
            on 64-bit platforms and vice versa.  */
    } a_un;
} Elf32_auxv_t;

typedef struct
{
    uint64_t a_type;           /* Entry type */
    union
    {
        uint64_t a_val;                /* Integer value */
        /* We use to have pointer elements added here.  We cannot do that,
            though, since it does not work when using 32-bit definitions
            on 64-bit platforms and vice versa.  */
    } a_un;
} Elf64_auxv_t;

/* These constants are for the segment types stored in the image headers */
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_SHLIB   5
#define PT_PHDR    6
#define PT_TLS     7               /* Thread local storage segment */
#define PT_LOOS    0x60000000      /* OS-specific */
#define PT_HIOS    0x6fffffff      /* OS-specific */
#define PT_LOPROC  0x70000000
#define PT_HIPROC  0x7fffffff
#define PT_GNU_EH_FRAME		0x6474e550

#define PT_GNU_STACK	(PT_LOOS + 0x474e551)

/*
 * Extended Numbering
 *
 * If the real number of program header table entries is larger than
 * or equal to PN_XNUM(0xffff), it is set to sh_info field of the
 * section header at index 0, and PN_XNUM is set to e_phnum
 * field. Otherwise, the section header at index 0 is zero
 * initialized, if it exists.
 *
 * Specifications are available in:
 *
 * - Oracle: Linker and Libraries.
 *   Part No: 817–1984–19, August 2011.
 *   http://docs.oracle.com/cd/E18752_01/pdf/817-1984.pdf
 *
 * - System V ABI AMD64 Architecture Processor Supplement
 *   Draft Version 0.99.4,
 *   January 13, 2010.
 *   http://www.cs.washington.edu/education/courses/cse351/12wi/supp-docs/abi.pdf
 */
#define PN_XNUM 0xffff

/* These constants define the different elf file types */
#define ET_NONE   0
#define ET_REL    1
#define ET_EXEC   2
#define ET_DYN    3
#define ET_CORE   4
#define ET_LOPROC 0xff00
#define ET_HIPROC 0xffff

/* This is the info that is needed to parse the dynamic section of the file */
#define DT_NULL		0
#define DT_NEEDED	1
#define DT_PLTRELSZ	2
#define DT_PLTGOT	3
#define DT_HASH		4
#define DT_STRTAB	5
#define DT_SYMTAB	6
#define DT_RELA		7
#define DT_RELASZ	8
#define DT_RELAENT	9
#define DT_STRSZ	10
#define DT_SYMENT	11
#define DT_INIT		12
#define DT_FINI		13
#define DT_SONAME	14
#define DT_RPATH 	15
#define DT_SYMBOLIC	16
#define DT_REL	        17
#define DT_RELSZ	18
#define DT_RELENT	19
#define DT_PLTREL	20
#define DT_DEBUG	21
#define DT_TEXTREL	22
#define DT_JMPREL	23
#define DT_ENCODING	32
#define OLD_DT_LOOS	0x60000000
#define DT_LOOS		0x6000000d
#define DT_HIOS		0x6ffff000
#define DT_VALRNGLO	0x6ffffd00
#define DT_VALRNGHI	0x6ffffdff
#define DT_ADDRRNGLO	0x6ffffe00
#define DT_ADDRRNGHI	0x6ffffeff
#define DT_VERSYM	0x6ffffff0
#define DT_RELACOUNT	0x6ffffff9
#define DT_RELCOUNT	0x6ffffffa
#define DT_FLAGS_1	0x6ffffffb
#define DT_VERDEF	0x6ffffffc
#define	DT_VERDEFNUM	0x6ffffffd
#define DT_VERNEED	0x6ffffffe
#define	DT_VERNEEDNUM	0x6fffffff
#define OLD_DT_HIOS     0x6fffffff
#define DT_LOPROC	0x70000000
#define DT_HIPROC	0x7fffffff

/* This info is needed when parsing the symbol table */
#define STB_LOCAL  0
#define STB_GLOBAL 1
#define STB_WEAK   2

#define STT_NOTYPE  0
#define STT_OBJECT  1
#define STT_FUNC    2
#define STT_SECTION 3
#define STT_FILE    4
#define STT_COMMON  5
#define STT_TLS     6

#define ELF_ST_BIND(x)		((x) >> 4)
#define ELF_ST_TYPE(x)		(((unsigned int) x) & 0xf)
#define ELF32_ST_BIND(x)	ELF_ST_BIND(x)
#define ELF32_ST_TYPE(x)	ELF_ST_TYPE(x)
#define ELF64_ST_BIND(x)	ELF_ST_BIND(x)
#define ELF64_ST_TYPE(x)	ELF_ST_TYPE(x)

typedef struct dynamic{
    Elf32_Sword d_tag;
    union{
        Elf32_Sword	d_val;
        Elf32_Addr	d_ptr;
    } d_un;
} Elf32_Dyn;

typedef struct {
    Elf64_Sxword d_tag;		/* entry tag value */
    union {
        Elf64_Xword d_val;
        Elf64_Addr d_ptr;
    } d_un;
} Elf64_Dyn;

/* The following are used with relocations */
#define ELF32_R_SYM(x) ((x) >> 8)
#define ELF32_R_TYPE(x) ((x) & 0xff)

#define ELF64_R_SYM(i)			((i) >> 32)
#define ELF64_R_TYPE(i)			((i) & 0xffffffff)

typedef struct elf32_rel {
    Elf32_Addr	r_offset;
    Elf32_Word	r_info;
} Elf32_Rel;

typedef struct elf64_rel {
    Elf64_Addr r_offset;	/* Location at which to apply the action */
    Elf64_Xword r_info;	/* index and type of relocation */
} Elf64_Rel;

typedef struct elf32_rela{
    Elf32_Addr	r_offset;
    Elf32_Word	r_info;
    Elf32_Sword	r_addend;
} Elf32_Rela;

typedef struct elf64_rela {
    Elf64_Addr r_offset;	/* Location at which to apply the action */
    Elf64_Xword r_info;	/* index and type of relocation */
    Elf64_Sxword r_addend;	/* Constant addend used to compute value */
} Elf64_Rela;

typedef struct elf32_sym{
    Elf32_Word	st_name;
    Elf32_Addr	st_value;
    Elf32_Word	st_size;
    unsigned char	st_info;
    unsigned char	st_other;
    Elf32_Half	st_shndx;
} Elf32_Sym;

typedef struct elf64_sym {
    Elf64_Word st_name;		/* Symbol name, index in string tbl */
    unsigned char	st_info;	/* Type and binding attributes */
    unsigned char	st_other;	/* No defined meaning, 0 */
    Elf64_Half st_shndx;		/* Associated section index */
    Elf64_Addr st_value;		/* Value of the symbol */
    Elf64_Xword st_size;		/* Associated symbol size */
} Elf64_Sym;


#define EI_NIDENT	16

typedef struct elf32_hdr{
    unsigned char	e_ident[EI_NIDENT];
    Elf32_Half	e_type;
    Elf32_Half	e_machine;
    Elf32_Word	e_version;
    Elf32_Addr	e_entry;  /* Entry point */
    Elf32_Off	e_phoff;
    Elf32_Off	e_shoff;
    Elf32_Word	e_flags;
    Elf32_Half	e_ehsize;
    Elf32_Half	e_phentsize;
    Elf32_Half	e_phnum;
    Elf32_Half	e_shentsize;
    Elf32_Half	e_shnum;
    Elf32_Half	e_shstrndx;
} Elf32_Ehdr;

typedef struct elf64_hdr {
    unsigned char	e_ident[EI_NIDENT];	/* ELF "magic number" */
    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry;		/* Entry point virtual address */
    Elf64_Off e_phoff;		/* Program header table file offset */
    Elf64_Off e_shoff;		/* Section header table file offset */
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
} Elf64_Ehdr;

/* These constants define the permissions on sections in the program
   header, p_flags. */
#define PF_R		0x4
#define PF_W		0x2
#define PF_X		0x1

typedef struct elf32_phdr{
    Elf32_Word	p_type;
    Elf32_Off	p_offset;
    Elf32_Addr	p_vaddr;
    Elf32_Addr	p_paddr;
    Elf32_Word	p_filesz;
    Elf32_Word	p_memsz;
    Elf32_Word	p_flags;
    Elf32_Word	p_align;
} Elf32_Phdr;

typedef struct elf64_phdr {
    Elf64_Word p_type;
    Elf64_Word p_flags;
    Elf64_Off p_offset;		/* Segment file offset */
    Elf64_Addr p_vaddr;		/* Segment virtual address */
    Elf64_Addr p_paddr;		/* Segment physical address */
    Elf64_Xword p_filesz;		/* Segment size in file */
    Elf64_Xword p_memsz;		/* Segment size in memory */
    Elf64_Xword p_align;		/* Segment alignment, file & memory */
} Elf64_Phdr;

/* sh_type */
#define SHT_NULL	0
#define SHT_PROGBITS	1
#define SHT_SYMTAB	2
#define SHT_STRTAB	3
#define SHT_RELA	4
#define SHT_HASH	5
#define SHT_DYNAMIC	6
#define SHT_NOTE	7
#define SHT_NOBITS	8
#define SHT_REL		9
#define SHT_SHLIB	10
#define SHT_DYNSYM	11
#define SHT_NUM		12
#define SHT_LOPROC	0x70000000
#define SHT_HIPROC	0x7fffffff
#define SHT_LOUSER	0x80000000
#define SHT_HIUSER	0xffffffff

/* sh_flags */
#define SHF_WRITE		0x1
#define SHF_ALLOC		0x2
#define SHF_EXECINSTR		0x4
#define SHF_RELA_LIVEPATCH	0x00100000
#define SHF_RO_AFTER_INIT	0x00200000
#define SHF_MASKPROC		0xf0000000

/* special section indexes */
#define SHN_UNDEF	0
#define SHN_LORESERVE	0xff00
#define SHN_LOPROC	0xff00
#define SHN_HIPROC	0xff1f
#define SHN_LIVEPATCH	0xff20
#define SHN_ABS		0xfff1
#define SHN_COMMON	0xfff2
#define SHN_HIRESERVE	0xffff

typedef struct elf32_shdr {
    Elf32_Word	sh_name;
    Elf32_Word	sh_type;
    Elf32_Word	sh_flags;
    Elf32_Addr	sh_addr;
    Elf32_Off	sh_offset;
    Elf32_Word	sh_size;
    Elf32_Word	sh_link;
    Elf32_Word	sh_info;
    Elf32_Word	sh_addralign;
    Elf32_Word	sh_entsize;
} Elf32_Shdr;

typedef struct elf64_shdr {
    Elf64_Word sh_name;		/* Section name, index in string tbl */
    Elf64_Word sh_type;		/* Type of section */
    Elf64_Xword sh_flags;		/* Miscellaneous section attributes */
    Elf64_Addr sh_addr;		/* Section virtual addr at execution */
    Elf64_Off sh_offset;		/* Section file offset */
    Elf64_Xword sh_size;		/* Size of section in bytes */
    Elf64_Word sh_link;		/* Index of another section */
    Elf64_Word sh_info;		/* Additional section information */
    Elf64_Xword sh_addralign;	/* Section alignment */
    Elf64_Xword sh_entsize;	/* Entry size if section holds table */
} Elf64_Shdr;

#define	EI_MAG0		0		/* e_ident[] indexes */
#define	EI_MAG1		1
#define	EI_MAG2		2
#define	EI_MAG3		3
#define	EI_CLASS	4
#define	EI_DATA		5
#define	EI_VERSION	6
#define	EI_OSABI	7
#define	EI_PAD		8

#define	ELFMAG0		0x7f		/* EI_MAG */
#define	ELFMAG1		'E'
#define	ELFMAG2		'L'
#define	ELFMAG3		'F'
#define	ELFMAG		"\177ELF"
#define	SELFMAG		4

#define	ELFCLASSNONE	0		/* EI_CLASS */
#define	ELFCLASS32	1
#define	ELFCLASS64	2
#define	ELFCLASSNUM	3

#define ELFDATANONE	0		/* e_ident[EI_DATA] */
#define ELFDATA2LSB	1
#define ELFDATA2MSB	2

#define EV_NONE		0		/* e_version, EI_VERSION */
#define EV_CURRENT	1
#define EV_NUM		2

#define ELFOSABI_NONE	0
#define ELFOSABI_LINUX	3

#ifndef ELF_OSABI
#define ELF_OSABI ELFOSABI_NONE
#endif

/*
 * Notes used in ET_CORE. Architectures export some of the arch register sets
 * using the corresponding note types via the PTRACE_GETREGSET and
 * PTRACE_SETREGSET requests.
 */
#define NT_PRSTATUS	1
#define NT_PRFPREG	2
#define NT_PRPSINFO	3
#define NT_TASKSTRUCT	4
#define NT_AUXV		6
/*
 * Note to userspace developers: size of NT_SIGINFO note may increase
 * in the future to accomodate more fields, don't assume it is fixed!
 */
#define NT_SIGINFO      0x53494749
#define NT_FILE         0x46494c45
#define NT_PRXFPREG     0x46e62b7f      /* copied from gdb5.1/include/elf/common.h */
#define NT_PPC_VMX	0x100		/* PowerPC Altivec/VMX registers */
#define NT_PPC_SPE	0x101		/* PowerPC SPE/EVR registers */
#define NT_PPC_VSX	0x102		/* PowerPC VSX registers */
#define NT_PPC_TAR	0x103		/* Target Address Register */
#define NT_PPC_PPR	0x104		/* Program Priority Register */
#define NT_PPC_DSCR	0x105		/* Data Stream Control Register */
#define NT_PPC_EBB	0x106		/* Event Based Branch Registers */
#define NT_PPC_PMU	0x107		/* Performance Monitor Registers */
#define NT_PPC_TM_CGPR	0x108		/* TM checkpointed GPR Registers */
#define NT_PPC_TM_CFPR	0x109		/* TM checkpointed FPR Registers */
#define NT_PPC_TM_CVMX	0x10a		/* TM checkpointed VMX Registers */
#define NT_PPC_TM_CVSX	0x10b		/* TM checkpointed VSX Registers */
#define NT_PPC_TM_SPR	0x10c		/* TM Special Purpose Registers */
#define NT_PPC_TM_CTAR	0x10d		/* TM checkpointed Target Address Register */
#define NT_PPC_TM_CPPR	0x10e		/* TM checkpointed Program Priority Register */
#define NT_PPC_TM_CDSCR	0x10f		/* TM checkpointed Data Stream Control Register */
#define NT_386_TLS	0x200		/* i386 TLS slots (struct user_desc) */
#define NT_386_IOPERM	0x201		/* x86 io permission bitmap (1=deny) */
#define NT_X86_XSTATE	0x202		/* x86 extended state using xsave */
#define NT_S390_HIGH_GPRS	0x300	/* s390 upper register halves */
#define NT_S390_TIMER	0x301		/* s390 timer register */
#define NT_S390_TODCMP	0x302		/* s390 TOD clock comparator register */
#define NT_S390_TODPREG	0x303		/* s390 TOD programmable register */
#define NT_S390_CTRS	0x304		/* s390 control registers */
#define NT_S390_PREFIX	0x305		/* s390 prefix register */
#define NT_S390_LAST_BREAK	0x306	/* s390 breaking event address */
#define NT_S390_SYSTEM_CALL	0x307	/* s390 system call restart data */
#define NT_S390_TDB	0x308		/* s390 transaction diagnostic block */
#define NT_S390_VXRS_LOW	0x309	/* s390 vector registers 0-15 upper half */
#define NT_S390_VXRS_HIGH	0x30a	/* s390 vector registers 16-31 */
#define NT_S390_GS_CB	0x30b		/* s390 guarded storage registers */
#define NT_S390_GS_BC	0x30c		/* s390 guarded storage broadcast control block */
#define NT_ARM_VFP	0x400		/* ARM VFP/NEON registers */
#define NT_ARM_TLS	0x401		/* ARM TLS register */
#define NT_ARM_HW_BREAK	0x402		/* ARM hardware breakpoint registers */
#define NT_ARM_HW_WATCH	0x403		/* ARM hardware watchpoint registers */
#define NT_ARM_SYSTEM_CALL	0x404	/* ARM system call number */
#define NT_METAG_CBUF	0x500		/* Metag catch buffer registers */
#define NT_METAG_RPIPE	0x501		/* Metag read pipeline state */
#define NT_METAG_TLS	0x502		/* Metag TLS pointer */
#define NT_ARC_V2	0x600		/* ARCv2 accumulator/extra registers */

#define DT_NULL                0                /* Marks end of dynamic section */
#define DT_NEEDED        1                /* Name of needed library */
#define DT_PLTRELSZ        2                /* Size in bytes of PLT relocs */
#define DT_PLTGOT        3                /* Processor defined value */
#define DT_HASH                4                /* Address of symbol hash table */
#define DT_STRTAB        5                /* Address of string table */
#define DT_SYMTAB        6                /* Address of symbol table */
#define DT_RELA                7                /* Address of Rela relocs */
#define DT_RELASZ        8                /* Total size of Rela relocs */
#define DT_RELAENT        9                /* Size of one Rela reloc */
#define DT_STRSZ        10                /* Size of string table */
#define DT_SYMENT        11                /* Size of one symbol table entry */
#define DT_INIT                12                /* Address of init function */
#define DT_FINI                13                /* Address of termination function */
#define DT_SONAME        14                /* Name of shared object */
#define DT_RPATH        15                /* Library search path (deprecated) */
#define DT_SYMBOLIC        16                /* Start symbol search here */
#define DT_REL                17                /* Address of Rel relocs */
#define DT_RELSZ        18                /* Total size of Rel relocs */
#define DT_RELENT        19                /* Size of one Rel reloc */
#define DT_PLTREL        20                /* Type of reloc in PLT */
#define DT_DEBUG        21                /* For debugging; unspecified */
#define DT_TEXTREL        22                /* Reloc might modify .text */
#define DT_JMPREL        23                /* Address of PLT relocs */
#define        DT_BIND_NOW        24                /* Process relocations of object */
#define        DT_INIT_ARRAY        25                /* Array with addresses of init fct */
#define        DT_FINI_ARRAY        26                /* Array with addresses of fini fct */
#define        DT_INIT_ARRAYSZ        27                /* Size in bytes of DT_INIT_ARRAY */
#define        DT_FINI_ARRAYSZ        28                /* Size in bytes of DT_FINI_ARRAY */
#define DT_RUNPATH        29                /* Library search path */
#define DT_FLAGS        30                /* Flags for the object being loaded */
#define DT_ENCODING        32                /* Start of encoded range */
#define DT_PREINIT_ARRAY 32                /* Array with addresses of preinit fct*/
#define DT_PREINIT_ARRAYSZ 33                /* size in bytes of DT_PREINIT_ARRAY */
#define        DT_NUM                34                /* Number used */
#define DT_LOOS                0x6000000d        /* Start of OS-specific */
#define DT_HIOS                0x6ffff000        /* End of OS-specific */
#define DT_LOPROC        0x70000000        /* Start of processor-specific */
#define DT_HIPROC        0x7fffffff        /* End of processor-specific */
#define        DT_PROCNUM        DT_MIPS_NUM        /* Most used by any processor */
/* DT_* entries which fall between DT_VALRNGHI & DT_VALRNGLO use the
   Dyn.d_un.d_val field of the Elf*_Dyn structure.  This follows Sun's
   approach.  */
#define DT_VALRNGLO        0x6ffffd00
#define DT_GNU_PRELINKED 0x6ffffdf5        /* Prelinking timestamp */
#define DT_GNU_CONFLICTSZ 0x6ffffdf6        /* Size of conflict section */
#define DT_GNU_LIBLISTSZ 0x6ffffdf7        /* Size of library list */
#define DT_CHECKSUM        0x6ffffdf8
#define DT_PLTPADSZ        0x6ffffdf9
#define DT_MOVEENT        0x6ffffdfa
#define DT_MOVESZ        0x6ffffdfb
#define DT_FEATURE_1        0x6ffffdfc        /* Feature selection (DTF_*).  */
#define DT_POSFLAG_1        0x6ffffdfd        /* Flags for DT_* entries, effecting
                                           the following DT_* entry.  */
#define DT_SYMINSZ        0x6ffffdfe        /* Size of syminfo table (in bytes) */
#define DT_SYMINENT        0x6ffffdff        /* Entry size of syminfo */
#define DT_VALRNGHI        0x6ffffdff
#define DT_VALTAGIDX(tag)        (DT_VALRNGHI - (tag))        /* Reverse order! */
#define DT_VALNUM 12
/* DT_* entries which fall between DT_ADDRRNGHI & DT_ADDRRNGLO use the
   Dyn.d_un.d_ptr field of the Elf*_Dyn structure.
   If any adjustment is made to the ELF object after it has been
   built these entries will need to be adjusted.  */
#define DT_ADDRRNGLO        0x6ffffe00
#define DT_GNU_HASH        0x6ffffef5        /* GNU-style hash table.  */
#define DT_TLSDESC_PLT        0x6ffffef6
#define DT_TLSDESC_GOT        0x6ffffef7
#define DT_GNU_CONFLICT        0x6ffffef8        /* Start of conflict section */
#define DT_GNU_LIBLIST        0x6ffffef9        /* Library list */
#define DT_CONFIG        0x6ffffefa        /* Configuration information.  */
#define DT_DEPAUDIT        0x6ffffefb        /* Dependency auditing.  */
#define DT_AUDIT        0x6ffffefc        /* Object auditing.  */
#define        DT_PLTPAD        0x6ffffefd        /* PLT padding.  */
#define        DT_MOVETAB        0x6ffffefe        /* Move table.  */
#define DT_SYMINFO        0x6ffffeff        /* Syminfo table.  */
#define DT_ADDRRNGHI        0x6ffffeff
#define DT_ADDRTAGIDX(tag)        (DT_ADDRRNGHI - (tag))        /* Reverse order! */
#define DT_ADDRNUM 11
/* The versioning entry types.  The next are defined as part of the
   GNU extension.  */
#define DT_VERSYM        0x6ffffff0
#define DT_RELACOUNT        0x6ffffff9
#define DT_RELCOUNT        0x6ffffffa
/* These were chosen by Sun.  */
#define DT_FLAGS_1        0x6ffffffb        /* State flags, see DF_1_* below.  */
#define        DT_VERDEF        0x6ffffffc        /* Address of version definition
                                           table */
#define        DT_VERDEFNUM        0x6ffffffd        /* Number of version definitions */
#define        DT_VERNEED        0x6ffffffe        /* Address of table with needed
                                           versions */
#define        DT_VERNEEDNUM        0x6fffffff        /* Number of needed versions */
#define DT_VERSIONTAGIDX(tag)        (DT_VERNEEDNUM - (tag))        /* Reverse order! */
#define DT_VERSIONTAGNUM 16
/* Sun added these machine-independent extensions in the "processor-specific"
   range.  Be compatible.  */
#define DT_AUXILIARY    0x7ffffffd      /* Shared object to load before self */
#define DT_FILTER       0x7fffffff      /* Shared object to get values from */
#define DT_EXTRATAGIDX(tag)        ((Elf32_Word)-((Elf32_Sword) (tag) <<1>>1)-1)
#define DT_EXTRANUM        3

/* Note header in a PT_NOTE section */
typedef struct elf32_note {
    Elf32_Word	n_namesz;	/* Name size */
    Elf32_Word	n_descsz;	/* Content size */
    Elf32_Word	n_type;		/* Content type */
} Elf32_Nhdr;

/* Note header in a PT_NOTE section */
typedef struct elf64_note {
    Elf64_Word n_namesz;	/* Name size */
    Elf64_Word n_descsz;	/* Content size */
    Elf64_Word n_type;	/* Content type */
} Elf64_Nhdr;

#endif /* _UAPI_LINUX_ELF_H */
