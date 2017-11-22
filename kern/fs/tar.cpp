/*
tar.cpp

Copyright (c) 05 Yann BOUCHER (yann)

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

#include "tar.hpp"

#include <stdlib.h>

#include "pathutils.hpp"
#include "utils/memutils.hpp"

#define TMAGIC   "ustar"        /* ustar and a null */
#define TMAGLEN  6
#define TVERSION "00"           /* 00 and no null */
#define TVERSLEN 2

/* Values used in typeflag field.  */
#define REGTYPE  '0'            /* regular file */
#define AREGTYPE '\0'           /* regular file */
#define LNKTYPE  '1'            /* link */
#define SYMTYPE  '2'            /* reserved */
#define CHRTYPE  '3'            /* character special */
#define BLKTYPE  '4'            /* block special */
#define DIRTYPE  '5'            /* directory */
#define FIFOTYPE '6'            /* FIFO special */
#define CONTTYPE '7'            /* reserved */

#define XHDTYPE  'x'            /* Extended header referring to the
    next file in the archive */
#define XGLTYPE  'g'            /* Global extended header */

/* Bits used in the mode field, values in octal.  */
#define TSUID    04000          /* set UID on execution */
#define TSGID    02000          /* set GID on execution */
#define TSVTX    01000          /* reserved */
/* file permissions */
#define TUREAD   00400          /* read by owner */
#define TUWRITE  00200          /* write by owner */
#define TUEXEC   00100          /* execute/search by owner */
#define TGREAD   00040          /* read by group */
#define TGWRITE  00020          /* write by group */
#define TGEXEC   00010          /* execute/search by group */
#define TOREAD   00004          /* read by other */
#define TOWRITE  00002          /* write by other */
#define TOEXEC   00001          /* execute/search by other */

namespace tar
{

TarFS::TarFS(std::vector<uint8_t> file)
    : m_file(file)
{
}

tar_node TarFS::root_dir() const
{
    tar_node root(*this);
    root.m_is_dir = true;
    root.m_name = "";
    root.m_data_addr = m_file.data() + sizeof(Header);
    root.m_size = m_file.size();

    return root;
}

std::optional<tar_node> TarFS::read_header(const Header *hdr) const
{
    if (hdr->name[0] == '\0')
    {
        return {};
    }
    if (!check_sum(hdr))
    {
        warn("Bad TAR chksum\n");
        return {};
    }

    tar_node node(*this);

    switch (hdr->typeflag)
    {
        case REGTYPE:
        case AREGTYPE :
            node.m_is_dir = false;
            break;
        case DIRTYPE:
            node.m_is_dir = true;
            break;
        case LNKTYPE:
        case SYMTYPE:
            // TODO : Do these
            return {};
        case CHRTYPE:
        case BLKTYPE:
        case FIFOTYPE:
        case CONTTYPE:
            return {};
    }

    node.m_data_addr = reinterpret_cast<const uint8_t*>(hdr) + sizeof(Header);
    node.m_size = read_number(hdr->size);
    node.m_perms = read_number(hdr->mode);
    node.m_uid = read_number(hdr->uid);
    node.m_gid = read_number(hdr->gid);
    node.m_name = std::string(hdr->name, 101); node.m_name.back() = '\0';
    node.m_name = filename(trim(node.m_name));

    return node;
}

std::vector<tar_node> TarFS::read_dir(const uint8_t *addr, size_t size) const
{
    std::vector<tar_node> nodes;

    const uint8_t* ptr = addr;

    while (ptr < addr + size)
    {
        const Header* hdr = reinterpret_cast<const Header*>(ptr);
        auto node = read_header(hdr);

        if (node)
        {
            nodes.emplace_back(*node);
        }
        else
        {
            return nodes;
        }

        size_t jump = read_number(hdr->size);
        if (jump % sizeof(Header))
        {
            jump += sizeof(Header) - (jump % sizeof(Header));
        }
        ptr += sizeof(Header) + jump;

        // Handle end-of-file 512 bytes blocks
        if (!read_header(reinterpret_cast<const Header*>(ptr)))
        {
            ptr += sizeof(Header);
        }
    }

    return nodes;
}

bool TarFS::check_sum(const TarFS::Header *hdr) const
{
    size_t sum { 0 };
    Header hdr_copy = *hdr;
    strncpy(hdr_copy.chksum, "        ", 8);

    for (size_t i { 0 }; i < sizeof(Header); ++i)
    {
        sum += reinterpret_cast<const uint8_t*>(&hdr_copy)[i];
    }
    return read_number(hdr->chksum) == sum;
}

}
