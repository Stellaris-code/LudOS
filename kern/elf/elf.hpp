/*
elf.hpp

Copyright (c) 30 Yann BOUCHER (yann)

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
#ifndef ELF_HPP
#define ELF_HPP

#include <stdint.h>

namespace elf
{

using Elf32_Half  = uint16_t;	// Unsigned half int
using Elf32_Off   = uint32_t;	// Unsigned offset
using Elf32_Addr  = uint32_t;	// Unsigned address
using Elf32_Word  = uint32_t;	// Unsigned int
using Elf32_Sword =  int32_t;	// Signed int

static constexpr size_t elf_nident = 16;

struct Elf32_Ehdr
{
    uint8_t		e_ident[elf_nident];
    Elf32_Half	e_type;
    Elf32_Half	e_machine;
    Elf32_Word	e_version;
    Elf32_Addr	e_entry;
    Elf32_Off	e_phoff;
    Elf32_Off	e_shoff;
    Elf32_Word	e_flags;
    Elf32_Half	e_ehsize;
    Elf32_Half	e_phentsize;
    Elf32_Half	e_phnum;
    Elf32_Half	e_shentsize;
    Elf32_Half	e_shnum;
    Elf32_Half	e_shstrndx;
};

enum Elf_Ident
{
    EI_MAG0		= 0, // 0x7F
    EI_MAG1		= 1, // 'E'
    EI_MAG2		= 2, // 'L'
    EI_MAG3		= 3, // 'F'
    EI_CLASS	= 4, // Architecture (32/64)
    EI_DATA		= 5, // Byte Order
    EI_VERSION	= 6, // ELF Version
    EI_OSABI	= 7, // OS Specific
    EI_ABIVERSION	= 8, // OS Specific
    EI_PAD		= 9  // Padding
};

#define ELFMAG0	0x7F // e_ident[EI_MAG0]
#define ELFMAG1	'E'  // e_ident[EI_MAG1]
#define ELFMAG2	'L'  // e_ident[EI_MAG2]
#define ELFMAG3	'F'  // e_ident[EI_MAG3]

#define ELFDATA2LSB	(1)  // Little Endian
#define ELFCLASS32	(1)  // 32-bit Architecture

enum Elf_Type
{
    ET_NONE		= 0, // Unkown Type
    ET_REL		= 1, // Relocatable File
    ET_EXEC		= 2  // Executable File
};

#define EM_386		(3)  // x86 Machine Type
#define EV_CURRENT	(1)  // ELF Current Version

struct Elf32_Shdr
{
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
};

#define SHN_UNDEF	(0x0000) // Undefined/Not present
#define SHN_ABS 	(0xfff1) // Undefined/Not present

enum ShT_Types
{
    SHT_NULL	= 0,   // Null section
    SHT_PROGBITS	= 1,   // Program information
    SHT_SYMTAB	= 2,   // Symbol table
    SHT_STRTAB	= 3,   // String table
    SHT_RELA	= 4,   // Relocation (w/ addend)
    SHT_NOBITS	= 8,   // Not present in file
    SHT_REL		= 9,   // Relocation (no addend)
};

enum ShT_Attributes
{
    SHF_WRITE	= 0x01, // Writable section
    SHF_ALLOC	= 0x02  // Exists in memory
};


enum Pf_Perms
{
    PF_R = 0x4,
    PF_W = 0x2,
    PF_X = 0x1
};

struct Elf32_Phdr
{
    Elf32_Word	p_type;
    Elf32_Off	p_offset;
    Elf32_Addr	p_vaddr;
    Elf32_Addr	p_paddr;
    Elf32_Word	p_filesz;
    Elf32_Word	p_memsz;
    Elf32_Word	p_flags;
    Elf32_Word	p_align;
};

enum Pt_Types : uint32_t
{
    PT_NULL = 0,
    PT_LOAD = 1,
    PT_DYNAMIC = 2,
    PT_INTERP = 3,
    PT_NOTE = 4,
    PT_SHLIB = 5,
    PT_PHDR = 6,
    PT_TLS = 7,
    PT_LOOS = 0x60000000,
    PT_HIOS = 0x6fffffff,
    PT_LOPROC = 0x70000000,
    PT_HIPROC = 0x7fffffff,
    PT_GNU_EH_FRAME = PT_LOOS + 0x474e550,
    PT_GNU_STACK = PT_LOOS + 0x474e551
};

struct Elf32_Sym
{
    Elf32_Word		st_name;
    Elf32_Addr		st_value;
    Elf32_Word		st_size;
    uint8_t			st_info;
    uint8_t			st_other;
    Elf32_Half		st_shndx;
};

#define ELF32_ST_BIND(INFO)	((INFO) >> 4)
#define ELF32_ST_TYPE(INFO)	((INFO) & 0x0F)

enum StT_Bindings
{
    STB_LOCAL		= 0, // Local scope
    STB_GLOBAL		= 1, // Global scope
    STB_WEAK		= 2  // Weak, (ie. __attribute__((weak)))
};

enum StT_Types
{
    STT_NOTYPE		= 0, // No type
    STT_OBJECT		= 1, // Variables, arrays, etc.
    STT_FUNC		= 2, // Methods or functions
    STT_SECTION     = 3,
    STT_FILE        = 4
};

const char* str_table(const Elf32_Shdr* hdr, size_t strtableidx);
const char* lookup_str(const char* strtable, size_t offset);
const Elf32_Sym* symbol(const Elf32_Shdr* symtab, size_t num);
const Elf32_Shdr* section(const Elf32_Shdr* base, size_t size, size_t type);
const Elf32_Phdr* program_header(const Elf32_Ehdr* base, size_t idx);

bool check_file(const Elf32_Ehdr *hdr);
bool check_supported(const Elf32_Ehdr* hdr);

extern const uint8_t* current_elf_file;

}

#endif // ELF_HPP
