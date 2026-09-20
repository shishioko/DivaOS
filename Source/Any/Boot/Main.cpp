#include "Boot/Main.hpp"

namespace DivaOS::Kernel {
    void Main()
    {
        __asm__ __volatile__(
            "mov rax, 0x3183cafe01234567\n"
            "1:\n"
            "jmp 1b\n"
            :
            :
            : "rax"
        );
        *( (volatile char *) 0x00000000000B8002 ) = 'G';
        *( (volatile char *) 0x00000000000B8003 ) = 0x04;
        const char* t = "welcome to c";
        return;
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