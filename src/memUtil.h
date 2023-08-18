#ifndef ARENAALLOCATOR_MEMUTIL_H_
#define ARENAALLOCATOR_MEMUTIL_H_

#include <cstddef>

namespace arenaallocator::util {
    void *MemSet(void *ptr, int val, size_t n);

    inline void *MemZero(void *ptr, size_t n) {return MemSet(ptr, 0x00, n);}

    void *MemFind(void *buf, unsigned char val, size_t sz);

    void *MemCopy(void *des, void *src, size_t sz);

    void *MemConc(void *des, size_t sz, void *src1, size_t sz1, void *src2, size_t sz2);

    int MemComp(void *ptr1, void *ptr2, size_t n);
}

#endif