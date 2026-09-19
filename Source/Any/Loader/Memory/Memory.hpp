#pragma once
#include "Definitions.hpp"

namespace DivaOS::Loader::Memory::Memory {
    void* Acquire(u64 size);
    void Release(void* address);
}