#pragma once
#include "Definitions.hpp"

namespace DivaOS::Kernel::Memory::MemoryManager {
    u64 Allocate(u64 size);
    void DeallocateAt(u64 start);
}