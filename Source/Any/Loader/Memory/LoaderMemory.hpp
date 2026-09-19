#pragma once
#include "Definitions.hpp"
#include "Loader.hpp"

namespace DivaOS {
    namespace Loader{
        namespace Memory{
            namespace LoaderMemory{
                extern void* Acquire(u64 size, u8 alignment) asm("DivaOS.Loader.Memory.LoaderMemory.Acquire_u64_u1");
            }
        }
    }
}