#pragma once
#include "Definitions.hpp"

namespace DivaOS::Loader::Memory {
    struct E820MemoryMapEntry;
    struct E820MemoryMapEntry{
        const void* Start;
        const u64 Size;
        const u32 Type;
        const u32 Attributes;
        E820MemoryMapEntry() = delete;
    };
}