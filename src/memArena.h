#ifndef ARENAALLOCATOR_MEMARENA_H_
#define ARENAALLOCATOR_MEMARENA_H_

#include <cstddef>
#include <cstdint>

#define POOL_SZ             2048             
#define ARENA_SZ            (512u << 10u)       // 512 KB (1 KiB = 1 KB = 1024 bytes @Apple)
#define POOLS_AVAIL         (ARENA_SZ / POOL_SZ)
#define ALIGN               8                   // Memory aligner
#define MAX_BLOCK_SZ        1024
#define CLASSES             (MAX_BLOCK_SZ / ALIGN)


namespace arenaallocator {
    struct alignas(ALIGN) Arena {
        unsigned short totalPools;
        unsigned short freePools;
        struct Pool *pool;                      // pointer to ll of avail pools
        struct Arena *next;
        struct Arena **prev;
    };

    struct alignas(ALIGN) Pool {
        Arena *arena;                           // pointer to arena that pool is in 
        unsigned short totalBlocks;
        unsigned short freeBlocks;
        unsigned short blocksz;
        void *block;                            // pointer to ll of avail blocks
        struct Pool *next;
        struct Pool **prev;
    };

    inline void *AlignDown(void *ptr, size_t sz) {
        return (void *) (((uintptr_t) ptr) & ~(sz - 1));
    }

    inline void *AlignUp(void *ptr, size_t sz) {
        return (void *) (((uintptr_t) ptr + sz) & ~(sz - 1));
    }

    inline size_t SzToCl(size_t sz) {
        return (((sz + (ALIGN - 1)) & ~((size_t) ALIGN - 1)) / ALIGN) - 1;
    }

    inline size_t ClToSz(size_t cl) {
        return ALIGN + (ALIGN * cl);
    }

    inline bool AddressInArenas(void *ptr) {
        auto *p = (Pool *) AlignDown(ptr, POOL_SZ);
        return p->arena != nullptr
               && (((uintptr_t) ptr - (uintptr_t) AlignDown(p->arena, POOL_SZ)) < POOL_SZ)
               && p->arena->totalPools == POOLS_AVAIL;
    }

    Arena *AllocArena();

    void FreeArena(Arena *arena);

    Pool *AllocPool(Arena *arena, size_t cl);

    void FreePool(Pool *pool);

    void *AllocBlock(Pool *pool);

    void FreeBlock(Pool *pool, void *block);

}

#endif