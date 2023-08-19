#ifndef ARENAALLOCATOR_MEMORY_H_
#define ARENAALLOCATOR_MEMORY_H_

#include <cstddef>
#include "linkedlist.h"
#include "memArena.h"

#define MIN_ARENAS          32 
#define REALLOC_THRESH      20

namespace arenaallocator {
    class Memory {
        ll::LinkedList<Arena> arenas; 

        ll::LinkedList<Pool> pools[CLASSES];

        std::mutex arenasMUTEX;

        std::mutex poolsMUTEX[CLASSES];

        Arena *FindArena();

        Pool *AllocatePool(size_t cl);

        Pool *GetPool(size_t cl);

        void ReleaseMemory(Pool *pool, size_t cl);

    public:
        void *Alloc(size_t sz) noexcept;

        void *Realloc(void *ptr, size_t sz);

        bool Init();

        void MemFin();

        void Free(void *ptr);
    };

    void *Alloc(size_t sz) noexcept;

    void *Realloc(void *ptr, size_t sz);

    bool MemInit();

    void MemFin();

    void Free(void *ptr);

} 

#endif