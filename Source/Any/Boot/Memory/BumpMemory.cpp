#include "Boot/Memory/BumpMemory.hpp"

namespace DivaOS::Boot::Memory::BumpMemory {
    namespace{
        /// @brief 
        u64 Offset = 0;
        /// @brief 
        const u64 Size = 16 * 1024 * 1024;
        /// @brief 
        u8 Buffer [Size];
    }

    void* Acquire(const u64 size, const u8 alignment){
        //Compute the padding required to align the memory chunk
        u64 padding = 0;
        if (alignment > 0x0){
            u64 alignmentMask = (0x1 << alignment) - 1;
            void* unaligned = &Buffer[Offset];
            void* aligned = (void*)(((u64)unaligned + alignmentMask) & ~alignmentMask);
            padding = (u64)aligned - (u64)unaligned;
        }
        //Compute the bounds of the padded memory chunk
        u64 start = Offset;
        u64 end = start + padding + size;
        //Ensure the requested size doesn't overflow any boundaries
        if (end > Size || end < start){
            Boot::Crash("Out of Loader Memory!");
        }
        //Offset the start address for future memory chunk acquisitions
        Offset = end;
        return &Buffer[start];
    }
}