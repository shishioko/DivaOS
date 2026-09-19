#pragma once
#include "Definitions.hpp"
#include "Loader.hpp"

namespace DivaOS {
    namespace Loader{
        namespace LoaderMemory{
            extern void* Acquire(u64 size, u8 alignment) asm("DivaOS.Loader.LoaderMemory.Acquire_u64_u1");
        }
    }
}