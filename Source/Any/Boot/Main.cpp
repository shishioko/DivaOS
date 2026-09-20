#include "Boot/Main.hpp"

namespace DivaOS::Boot {

    void Main(){
        if (!Terminal::Initialize()){
            Boot::Crash();
        }
        Terminal::WriteLine("Hello, Diva!!!");
        while (true);
    }

    void Crash(t8* text){
        Terminal::WriteLine(text);
        Crash();
    }
    void Crash(){
        asm volatile(R"(
            cli
            hlt
            jmp $
        )");
        while (true);
    }
}

/*void* operator new(u64 size){
    return DivaOS::Boot::LoaderMemory::Acquire(size);
}
void* operator new[](u64 size){
    return DivaOS::Boot::LoaderMemory::Acquire(size);
}
void operator delete(void* ptr, u64 size) noexcept{
    //unsupported
}
void operator delete[](void* ptr, u64 size) noexcept{
    //unsupported
}*/

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