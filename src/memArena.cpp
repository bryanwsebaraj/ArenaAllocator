#include <cassert>
#include <cstring>
#include "memOS.h"
#include "memUtil.h"
#include "memArena.h"

using namespace arenaallocator;

Arena *arenaallocator::AllocArena() {
    Arena *arena;
    void *mem = arenaallocator::os::Alloc(ARENA_SZ);

    if (mem == nullptr) {
        return nullptr;
    }
        
    arena = (Arena *) ((unsigned char *) mem) + (POOL_SZ - sizeof(Arena));
    // ^^ double check this. accounts for arena struct memory overhead
    arena->totalPools = POOLS_AVAIL;
    arena->freePools = POOLS_AVAIL;
    arena->pool = (Pool *) mem;
    util::MemZero(arena->pool, sizeof(Pool));
    arena->pool->arena = arena;
    arena->next = nullptr;
    arena->prev = nullptr;

    return arena;
}

void arenaallocator::FreeArena(Arena *arena) {
    auto *mem = AlignDown(arena, POOL_SZ);
    arenaallocator::os::Free(mem, ARENA_SZ);
}

Pool *arenaallocator::AllocPool(Arena *arena, size_t cl) {
    size_t bytes = POOL_SZ - sizeof(Pool); // accounts for pool struct memory overhead
    auto *pool = arena->pool;
    assert(pool != nullptr);
    arena->freePools--;

    arena->pool = pool->next;           // avail pool shifts over to next avail pool

    // aligns and creates new pool if there are no available pools but space for free pools
    if (arena->pool == nullptr && arena->freePools > 0) {
        auto new_pool = (Pool *) AlignUp(pool, POOL_SZ); 
        new_pool->arena = arena;
        new_pool->totalBlocks = 0;
        new_pool->freeBlocks = 0;
        new_pool->blocksz = 0;
        new_pool->block = nullptr;
        new_pool->next = nullptr;
        new_pool->prev = nullptr;
        arena->pool = new_pool;
    }

    if (pool == AlignDown(arena, POOL_SZ)) {
        bytes -= sizeof(Arena);
    }

    pool->blocksz = (unsigned short) ClToSz(cl);
    pool->totalBlocks = (unsigned short) bytes / pool->blocksz;
    pool->freeBlocks = pool->totalBlocks;
    pool->block = ((unsigned char *) pool) + sizeof(Pool);
    *((uintptr_t *) pool->block) = 0x0;
    pool->next = nullptr;
    pool->prev = nullptr;

    return pool;
}

void arenaallocator::FreePool(Pool *pool) {
    auto *arena = pool->arena;
    pool->next = arena->pool;      
    arena->pool = pool;                 
    arena->freePools++;
    assert(arena->freePools <= arena->totalPools);
}

void *arenaallocator::AllocBlock(Pool *pool) {
    void *block = pool->block;
    assert(block != nullptr);
    pool->freeBlocks--;

    pool->block = (void *) *((uintptr_t *) pool->block);
    if (pool->block == nullptr && pool->freeBlocks > 0) {
        pool->block = ((unsigned char *) block) + pool->blocksz;
        *((uintptr_t *) pool->block) = 0x0;
    }

    return block;
}

void arenaallocator::FreeBlock(Pool *pool, void *block) {
    *((uintptr_t *) block) = (uintptr_t) pool->block;
    pool->block = block;
    pool->freeBlocks++;
    assert(pool->freeBlocks <= pool->totalBlocks);
} 
