#include <cassert>
#include <mutex>
#include "memUtil.h"
#include "mem.h"

using namespace arenaallocator;

Memory def_alloc;

struct MemBlock {
    size_t sz;
    size_t offset;
};

Arena *Memory::FindArena() {
    Arena *arena = this->arenas.FindFree();

    if (arena == nullptr) {
        arena = AllocArena();
        this->arenas.Insert(arena);
    }

    return arena;
}

bool Memory::Init() {
    for (int i = 0; i < MIN_ARENAS; i++) {
        auto *arena = AllocArena();

        if (arena == nullptr) {
            if (i > 0) {
                this->MemFin();
            }
            return false;
        }
        this->arenas.Insert(arena);
    }

    return true;
}

Pool *Memory::AllocatePool(size_t cl) {
    std::scoped_lock alck(this->arenasMUTEX);
    auto *arena = this->FindArena();
    if (arena == nullptr) {
        return nullptr;
    }

    return AllocPool(arena, cl);
}

Pool *Memory::GetPool(size_t cl) {
    Pool *pool = this->pools[cl].FindFree();

    if (pool == nullptr) {
        if ((pool = this->AllocatePool(cl)) == nullptr) {
            return nullptr;
        }
        this->pools[cl].Insert(pool);
    }

    return pool;
}

void *Memory::Alloc(size_t sz) noexcept {
    size_t cl = SzToCl(sz);
    assert(sz > 0);

    if (sz <= MAX_BLOCK_SZ) {
        std::scoped_lock lck(this->poolsMUTEX[cl]);
        auto *pool = GetPool(cl);
        if (pool == nullptr) {
            return nullptr;
        }
        return AllocBlock(pool);
    }

    unsigned char *ptr;
    if ((ptr = (unsigned char *) malloc(sz + sizeof(MemBlock) + ALIGN)) == nullptr) {
        return nullptr;
    }

    auto tmp = (uintptr_t) (ptr + sizeof(MemBlock));
    auto ptr_a = (unsigned char *) (tmp + (ALIGN - (tmp % ALIGN)));
    auto *memBlock = (MemBlock *) ptr_a - sizeof(MemBlock);
    memBlock->sz = sz;
    memBlock->offset = ptr_a - ptr;

    return ptr_a;
}

void Memory::MemFin() {
    Arena *arena;
    while ((arena = this->arenas.Pop()) != nullptr) {
        FreeArena(arena);
    }
}

void Memory::Free(void *ptr) {
    if (ptr == nullptr) {
        return;
    }

    auto *pool = (Pool *) AlignDown(ptr, POOL_SZ);
    if (AddressInArenas(ptr)) {
        size_t cl = SzToCl(pool->blocksz);
        std::scoped_lock lck(this->poolsMUTEX[cl]);
        FreeBlock(pool, ptr);
        this->ReleaseMemory(pool, cl);
        return;
    }
    const auto *memBlock = (MemBlock *) ((unsigned char *) ptr - sizeof(MemBlock));
    free(((unsigned char *) ptr) - memBlock->offset);
}

void *Memory::Realloc(void *ptr, size_t sz) {
    void *tmp;
    size_t src_sz;

    if (ptr == nullptr) {
        return this->Alloc(sz);
    }

    const auto *pool = (Pool *) AlignDown(ptr, POOL_SZ);
    if (AddressInArenas(ptr)) {
        size_t actual = SzToCl(pool->blocksz);
        size_t desired = SzToCl(sz);
        src_sz = pool->blocksz;
        if (actual > desired && (actual - desired < REALLOC_THRESH)) {
            return ptr;
        }
    } else {
        const auto *memBlock = (MemBlock *) (((unsigned char *) ptr) - sizeof(MemBlock));
        src_sz = memBlock->sz;
        if (sz > MAX_BLOCK_SZ && src_sz >= sz) {
            return ptr;
        }
    }

    if ((tmp = Alloc(sz)) == nullptr) {
        return nullptr;
    }
    
    util::MemCopy(tmp, ptr, src_sz > sz ? sz : src_sz);
    this->Free(ptr);
    return tmp;
}

void Memory::ReleaseMemory(Pool *pool, size_t cl) {
    Arena *arena = pool->arena;

    if (pool->freeBlocks == pool->totalBlocks) {
        std::scoped_lock lck(this->arenasMUTEX);
        this->pools[cl].Remove(pool);

        FreePool(pool);

        if (arena->freePools != arena->totalPools) {
            this->arenas.Sort(arena);
        } else if (this->arenas.Count() > MIN_ARENAS) {
            this->arenas.Remove(arena);
            FreeArena(arena);
        }
        return;
    }

    this->pools[cl].Sort(pool);
}



void *arenaallocator::Alloc(size_t sz) noexcept {
    return def_alloc.Alloc(sz);
}

void arenaallocator::Free(void *ptr) {
    return def_alloc.Free(ptr);
}

void *arenaallocator::Realloc(void *ptr, size_t sz) {
    return def_alloc.Realloc(ptr, sz);
}

bool arenaallocator::MemInit() {
    return def_alloc.Init();
}

void arenaallocator::MemFin() {
    def_alloc.MemFin();
}