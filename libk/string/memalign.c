#include "string.h"

#include "stdlib.h"
#include "errno.h"
#include "external/liballoc/liballoc.h"

int posix_memalign(void **res, size_t align, size_t len)
{
        *res = kmalloc_align(len, align);
        return 0;
}
