#pragma once
#include "Definitions.hpp"
#include "Peripherals/Terminal.hpp"
#include "Memory/LoaderMemory.hpp"

namespace DivaOS::Loader::Loader {
    extern void Main() asm("Main");
    extern void Crash(const t8* text) asm("DivaOS.Loader.Loader.Crash_t8p");
}

void* operator new(u64 size);
void* operator new[](u64 size);
void operator delete(void* ptr, u64 size) noexcept;
void operator delete[](void* ptr, u64 size) noexcept;