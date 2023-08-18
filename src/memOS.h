#ifndef ARENAALLOCATOR_MEMOS_H_
#define ARENAALLOCATOR_MEMOS_H_

#include <cstddef>

namespace arenaallocator::os {
    void *Alloc(size_t size);

    void Free(void *ptr, size_t size);
}

#endif