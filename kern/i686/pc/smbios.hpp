/*
smbios.hpp

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
#ifndef SMBIOS_HPP
#define SMBIOS_HPP

#include "utils/logging.hpp"
#include "utils/stdint.h"

struct SMBIOSEntryPoint
{
   char EntryPointString[4];    //This is _SM_
   uint8_t Checksum;              //This value summed with all the values of the table, should be 0 (overflow)
   uint8_t Length;                //Length of the Entry Point Table. Since version 2.1 of SMBIOS, this is 0x1F
   uint8_t MajorVersion;          //Major Version of SMBIOS
   uint8_t MinorVersion;          //Minor Version of SMBIOS
   uint16_t MaxStructureSize;     //Maximum size of a SMBIOS Structure (we will se later)
   uint8_t EntryPointRevision;    //...
   char FormattedArea[5];       //...
   char EntryPointString2[5];   //This is _DMI_
   uint8_t Checksum2;             //Checksum for values from EntryPointString2 to the end of table
   uint16_t TableLength;          //Length of the Table containing all the structures
   uint32_t TableAddress;	     //Address of the Table
   uint16_t NumberOfStructures;   //Number of structures in the table
   uint8_t BCDRevision;           //Unused
} __attribute__((packed));

struct SMBIOSTag
{
    uint8_t type;
    uint8_t length;
    uint16_t handle;
};

struct SMBIOSBIOSInfo
{
    uint8_t type;
    uint8_t length;
    uint16_t handle;
    uint8_t vendor;
    uint8_t version;
    uint16_t start_addr_seg;
    uint8_t release_date;
    uint8_t rom_size;
    uint64_t characteristics;
    uint8_t extra_char_1;
    uint8_t extra_char_2;
    uint8_t major_release;
    uint8_t minor_release;
    uint8_t controller_major_release;
    uint8_t controller_minor_release;
    uint16_t ebios_size;
} __attribute__((packed));

struct SMBIOSCPUInfo
{
    uint8_t type;
    uint8_t length;
    uint16_t handle;
    uint8_t socket;
    uint8_t cpu_type;
    uint8_t family;
    uint8_t manufacturer;
    uint64_t id;
    uint8_t version;
    uint8_t voltage;
    uint16_t ext_clock;
    uint16_t max_speed;
    uint16_t curr_speed;
    uint8_t status;
    /// ...
} __attribute__((packed));

class SMBIOS
{
public:

public:
    static SMBIOSEntryPoint* locate();

    static SMBIOSBIOSInfo* bios_info();
    static SMBIOSCPUInfo* cpu_info();

private:

    static const char *get_string(uint8_t *offset, uint8_t number);
    static uintptr_t find_strings_end(uint8_t *offset);

private:
    static inline SMBIOSEntryPoint* entry_point { nullptr };
};

#endif // SMBIOS_HPP
