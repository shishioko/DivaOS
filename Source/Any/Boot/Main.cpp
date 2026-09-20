#include "Boot/Main.hpp"

#include "Boot/Memory/Map.hpp"
#include "Shared/Memory/AddressRange.hpp"

namespace DivaOS::Boot {
    void Main(){
        if (!Peripherals::Terminal::Initialize()){
            Boot::Crash();
        }
        Peripherals::Terminal::WriteLine("Hello, Diva!!!");
        Peripherals::Terminal::WriteLine("I was booted by Limine Bootloader.");
        
        Peripherals::Terminal::WriteLine("<3");

        while (true);
    }

    void Crash(){
        asm volatile(R"(
            cli
            hlt
            jmp $
        )");
        while (true);
    }
    void Crash(const t8* text){
        Peripherals::Terminal::WriteLine(text);
        Crash();
    }
}

void* operator new(u64 size){
    return DivaOS::Boot::Memory::BumpMemory::Acquire(size);
}
void* operator new[](u64 size){
    return DivaOS::Boot::Memory::BumpMemory::Acquire(size);
}
void operator delete(void* ptr, u64 size) noexcept{
    //unsupported
}
void operator delete[](void* ptr, u64 size) noexcept{
    //unsupported
}

void* memset(void* destination, int value, u64 count) {
    u8* pointer = (u8*)destination;
    while (count--) {
        *pointer++ = (u8)value;
    }
    return destination;
}

void* memcpy(void* destination, const void* source, u64 count) {
    u8* destination_data = (u8*)destination;
    const u8* source_data = (const u8*)source;
    while (count--) {
        *destination_data++ = *source_data++;
    }
    return destination;
}