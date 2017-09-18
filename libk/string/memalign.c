#include "string.h"

#include "stdlib.h"
#include "errno.h"

int posix_memalign(void **res, size_t align, size_t len)
{
        unsigned char *mem, *cnew, *end;
        size_t header, footer;

        if ((align & -align) != align) return EINVAL;
        if (len > SIZE_MAX - align) return ENOMEM;

        if (align <= 4*sizeof(size_t)) {
                if (!(mem = kmalloc(len)))
                        return errno;
                *res = mem;
                return 0;
        }

        if (!(mem = kmalloc(len + align-1)))
                return errno;

        header = ((size_t *)mem)[-1];
        end = mem + (header & -8);
        footer = ((size_t *)end)[-2];
        cnew = (void *)((uintptr_t)mem + align-1 & -align);

        if (!(header & 7)) {
                ((size_t *)cnew)[-2] = ((size_t *)mem)[-2] + (cnew-mem);
                ((size_t *)cnew)[-1] = ((size_t *)mem)[-1] - (cnew-mem);
                *res = cnew;
                return 0;
        }

        ((size_t *)mem)[-1] = header&7 | cnew-mem;
        ((size_t *)cnew)[-2] = footer&7 | cnew-mem;
        ((size_t *)cnew)[-1] = header&7 | end-cnew;
        ((size_t *)end)[-2] = footer&7 | end-cnew;

        if (cnew != mem) kfree(mem);
        *res = cnew;
        return 0;
}
