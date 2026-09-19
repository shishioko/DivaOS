#pragma once
#include "Definitions.hpp"
namespace DivaOS{
    namespace MemoryManager{
        u64 Allocate(u64 size);
        void DeallocateAt(u64 start);
    }
}