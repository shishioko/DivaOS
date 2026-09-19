#include "Loader.hpp"

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