#include <sys/mman.h>
#include <cstdint>
#include "memOS.h"
// https://man7.org/linux/man-pages/man2/mmap.2.html

void *arenaallocator::os::Alloc(size_t sz) {
    void *memory = mmap(nullptr, sz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if ((uintptr_t) memory == -1) {
        return nullptr;
    }
    return memory;
}

void arenaallocator::os::Free(void *ptr, size_t sz) { 
    munmap(ptr, sz);
}
