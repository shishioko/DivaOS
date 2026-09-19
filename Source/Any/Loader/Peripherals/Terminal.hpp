#pragma once
#include "Definitions.hpp"

namespace DivaOS::Loader::Peripherals::Terminal {
    extern void Write(const t8* text) asm("DivaOS.Loader.Peripherals.Terminal.Write_t8p");
    extern void Write(const t8 character) asm("DivaOS.Loader.Peripherals.Terminal.Write_t8");

    extern inline void WriteLine(const t8* text) asm("DivaOS.Loader.Peripherals.Terminal.WriteLine_t8p");
    inline void WriteLine(const t8* text){
        Write(text);
        Write('\n');
    }
}