#include "Memory/LoaderMemory.hpp"

namespace DivaOS::Loader::Memory::LoaderMemory {
    extern void* Start asm("DivaOS.Loader.Memory.LoaderMemory.Start");
    extern void* Offset asm("DivaOS.Loader.Memory.LoaderMemory.Offset");
    extern void* End asm("DivaOS.Loader.Memory.LoaderMemory.End");
    struct MemoryMapSegment{
        u64* Start;
        u64* End;
        u32 Type;
        MemoryMapSegment* Next;
    };

    void* Acquire(u64 size, u8 alignment)
    {
        u64 padding = 0;
        if (alignment > 0x0){
            u64 alignmentMask = (0x1 << alignment) - 1;
            void* unaligned = Offset;
            void* aligned = (void*)(((u64)unaligned + alignmentMask) & ~alignmentMask);
            padding = (u64)aligned - (u64)unaligned;
        }
        void* start = Offset;
        void* end = start + padding + size;
        if (end >= End || end < start){
            Loader::Crash("Out of Loader Memory!");
        }
        Offset = end;
        return start + padding;
    }
}