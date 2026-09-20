#include "Loader.hpp"

#include "Memory/Map.hpp"

namespace DivaOS::Loader::Loader {
    void Main()
    {
        Peripherals::Terminal::WriteLine("Hello, World!");
        for (u64 i = 0; i < 22; i++){
            Peripherals::Terminal::WriteLine("td");
        }
        Peripherals::Terminal::WriteLine("this is a new line");

        return;
    }
    void Crash(const t8* text){
        Peripherals::Terminal::Write(text);
        while (true);
    }
}

void* operator new(u64 size){
    return DivaOS::Loader::Memory::LoaderMemory::Acquire(size);
}
void* operator new[](u64 size){
    return DivaOS::Loader::Memory::LoaderMemory::Acquire(size);
}
void operator delete(void* ptr, u64 size) noexcept{
    //unsupported
}
void operator delete[](void* ptr, u64 size) noexcept{
    //unsupported
}