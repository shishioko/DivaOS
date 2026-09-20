#pragma once
#include "Definitions.hpp"

#include "Boot/Terminal.hpp"
#include "Boot/LoaderMemory.hpp"

namespace DivaOS::Boot
{
    extern void Main() asm("DivaOS.Boot.Main");
    void Crash();
    void Crash(t8* text);
}

void* operator new(u64 size);
void* operator new[](u64 size);
void operator delete(void* ptr, u64 size) noexcept;
void operator delete[](void* ptr, u64 size) noexcept;


extern void* memset(void* destination, int value, u64 count) asm("memset");
extern void* memcpy(void* destination, const void* source, u64 count) asm("memcpy");