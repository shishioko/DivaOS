#pragma once
#include "Definitions.hpp"

namespace DivaOS::Loader::Memory::PhysicalMemory {
    void* Acquire(const u64 size);
    void Release(const void* address);
}