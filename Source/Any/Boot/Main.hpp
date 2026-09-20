#pragma once
#include "Definitions.hpp"

#include "Boot/Peripherals/Terminal.hpp"
#include "Boot/Memory/BumpMemory.hpp"

namespace DivaOS::Boot
{
    extern void Main() asm("DivaOS.Boot.Main");
    void Crash();
    void Crash(const t8* text);
}

void* operator new(u64 size);
void* operator new[](u64 size);
void operator delete(void* ptr, u64 size) noexcept;
void operator delete[](void* ptr, u64 size) noexcept;


extern void* memset(void* destination, int value, u64 count) asm("memset");
extern void* memcpy(void* destination, const void* source, u64 count) asm("memcpy");