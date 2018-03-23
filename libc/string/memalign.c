#include "string.h"

#include "stdlib.h"
#include "errno.h"
#include "liballoc/liballoc.h"

int posix_memalign(void **res, size_t align, size_t len)
{
#ifndef LUDOS_USER
    *res = kmalloc_align(len, align);
#else
    *res = malloc_align(len, align);
#endif
    return 0;
}
