#pragma once
#include "Definitions.hpp"

#include "Boot/LoaderMemory.hpp"

namespace DivaOS::Kernel
{
    extern "C" void Main() asm("_start");
}

void* operator new(u64 size);
void* operator new[](u64 size);
void operator delete(void* ptr, u64 size) noexcept;
void operator delete[](void* ptr, u64 size) noexcept;
