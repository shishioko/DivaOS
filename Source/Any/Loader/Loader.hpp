#pragma once
#include "Definitions.hpp"
#include "Peripherals/Terminal.hpp"
#include "Memory/LoaderMemory.hpp"

namespace DivaOS {
    namespace Loader{
        namespace Loader
        {
            extern void Main() asm("Main");
            extern void Crash(const t8* text) asm("DivaOS.Loader.Loader.Crash_t8p");
        }
    }
}