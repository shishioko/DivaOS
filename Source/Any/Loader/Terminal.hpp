#pragma once
#include "Definitions.hpp"

namespace DivaOS{
    namespace Loader{
        namespace Terminal{
            extern void Write(const t8* text) asm("DivaOS.Loader.Terminal.Write_t8p");
            extern void Write(const t8 character) asm("DivaOS.Loader.Terminal.Write_t8");

            inline void WriteLine(const t8* text){
                Write(text);
                Write('\n');
            }
        }
    }
}