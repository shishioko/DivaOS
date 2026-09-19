#pragma once
#include "Definitions.hpp"

namespace DivaOS::Loader::Memory::PhysicalMemory {
    void* Acquire(const u64 size, const u8 alignment = 0);
    void Release(const void* address);
}