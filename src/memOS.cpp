#include "memOS.h"

#include <sys/mman.h>
#include <cstdint>

void *arenaallocator::os::Alloc(size_t size) {
    void *mem = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    // https://man7.org/linux/man-pages/man2/mmap.2.html
    if ((uintptr_t) mem == -1) {
        return nullptr;
    }
    return mem;
}

void arenaallocator::os::Free(void *ptr, size_t size) { 
    munmap(ptr, size);
}
