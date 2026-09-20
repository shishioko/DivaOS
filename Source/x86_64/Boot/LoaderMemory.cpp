#include "Boot/LoaderMemory.hpp"

namespace DivaOS::Boot::Memory::LoaderMemory {
    /*namespace{
        /// @brief Holds the initial location of the memory to be used by the bootloader
        extern void* Start;
        /// @brief Holds the current location of the memory to be used by the bootloader
        extern void* Offset;
        /// @brief Holds the maximum of the location of the memory to be used by the bootloader
        extern void* End;
    }

    void* Acquire(u64 size, u8 alignment)
    {
        //Compute the padding required to align the memory chunk
        u64 padding = 0;
        if (alignment > 0x0){
            u64 alignmentMask = (0x1 << alignment) - 1;
            void* unaligned = Offset;
            void* aligned = (void*)(((u64)unaligned + alignmentMask) & ~alignmentMask);
            padding = (u64)aligned - (u64)unaligned;
        }
        //Compute the bounds of the padded memory chunk
        void* start = Offset;
        void* end = start + padding + size;
        //Ensure the requested size doesn't overflow any boundaries
        if (end >= End || end < start){
            //Loader::Crash("Out of Loader Memory!");
            //TODO: crash
            while (true);
        }
        //Offset the start address for future memory chunk acquisitions
        Offset = end;
        return start + padding;
    }*/
}