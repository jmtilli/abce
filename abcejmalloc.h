#ifndef _ABCE_JMALLOC_H_
#define _ABCE_JMALLOC_H_

#include <stddef.h>

void *abce_jmalloc(size_t sz);

void abce_jmfree(void *ptr, size_t sz);

void *abce_jmrealloc(void *oldptr, size_t oldsz, size_t newsz);

size_t abce_jmgetarenabytes(void);

#endif
