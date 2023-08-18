#ifndef ARENAALLOCATOR_MEMARENA_H_
#define ARENAALLOCATOR_MEMARENA_H_

#include <cstddef>
#include <cstdint>

#define POOL_SZ             2048             
#define ARENA_SZ            (512u << 10u)    // 512 KB (1 KiB = 1 KB = 1024 bytes @Apple)
#define POOLS_AVAIL         (ARENA_SZ / POOL_SZ)
#define QUANTUM             8 // Memory alignment
#define MAX_BLOCK_SZ        1024
#define CLASSES             (MAX_BLOCK_SZ / QUANTUM)


namespace arenaallocator {
    struct alignas(QUANTUM) Arena {
        unsigned int pools;
        unsigned int free;
        struct Pool *pool;
        struct Arena *next;
        struct Arena **prev;
    };

    struct alignas(QUANTUM) Pool {
        Arena *arena;
        unsigned short numBlocks;
        unsigned short free;
        unsigned short blocksz;
        void *block; //          Pointer to linked-list of available blocks
        struct Pool *next;
        struct Pool **prev;
    };


    Arena *AllocArena();

    Pool *AllocPool(Arena *arena, size_t cl);

    void FreeArena(Arena *arena);

    void FreePool(Pool *pool);

    void *AllocBlock(Pool *pool);

    void FreeBlock(Pool *pool, void *block);

}

#endif