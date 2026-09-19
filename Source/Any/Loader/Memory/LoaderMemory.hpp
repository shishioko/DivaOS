#pragma once
#include "Definitions.hpp"

namespace DivaOS::Loader::Memory::LoaderMemory {
    extern void* Acquire(const u64 size, const u8 alignment = 0) asm("DivaOS.Loader.Memory.LoaderMemory.Acquire_u64_u1");
}