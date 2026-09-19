#pragma once
#include "Definitions.hpp"

/// @brief Contains basic methods to manage physical memory within the real mode address range
namespace DivaOS::Loader::Memory::LoaderMemory {
    /**
         * @brief Used to permanently acquire a contiguous physical chunk of memory.
         * @param size The size of the chunk to be acquired
         * @param alignment The amounts of bits the address of the chunk has to be aligned to
         * @return A pointer to the acquired chunk of memory
         */
    extern void* Acquire(const u64 size, const u8 alignment = 0) asm("DivaOS.Loader.Memory.LoaderMemory.Acquire_u64_u8");
}