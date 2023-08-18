#include "memUtil.h"

void *arenaallocator::util::MemSet(void *ptr, int value, size_t n) {
    auto p = (unsigned char *) ptr;
    while (n-- > 0)
        *p++ = value;

    return ptr;
}

void *arenaallocator::util::MemFind(void *buf, unsigned char val, size_t sz) {
    for (size_t i = 0; i < sz; i++) {
        if (((unsigned char *) buf)[i] == val)
            return (void *) (((unsigned char *) buf) + i);
    }
    return nullptr;
}

void *arenaallocator::util::MemCopy(void *des, void *src, size_t sz) {
    auto d = (unsigned char *) des;
    auto s = (unsigned char *) src;
    while (sz--) {
        *d++ = *s++;
    }
    return d;
}

int arenaallocator::util::MemComp(void *ptr1, void *ptr2, size_t n) {
    auto *p1 = (unsigned char *) ptr1;
    auto *p2 = (unsigned char *) ptr2;

    if (ptr1 == nullptr && ptr2 == nullptr) {
        return 0;
    }
    else if (ptr1 == nullptr) {
        return *p2;
    }
    else if (ptr2 == nullptr) {
        return *p1;
    }

    do {
        if (*p1++ != *p2++) {
            return *--p1 - *--p2;
        }
    } while (--n != 0);

    return 0;
}

void *arenaallocator::util::MemConc(void *des, size_t sz, void *src1, size_t sz1, void *src2, size_t sz2) {
    if (sz1 > sz) {
        sz1 = sz;
    }

    if (sz2 > sz - sz1) {
        sz2 = sz - sz1;
    }

    if (des != nullptr) {
        MemCopy(des, src1, sz1);
        MemCopy(((unsigned char *) des) + sz1, src2, sz2);
    }

    return des;
}
