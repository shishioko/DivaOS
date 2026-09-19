#include "Definitions.hpp"
#include "Terminal.hpp"

namespace DivaOS {
    namespace Loader{
        extern "C" void Main() asm("Main");
        void Main()
        {
            Terminal::WriteLine("Hello, World!");
            for (u64 i = 0; i < 22; i++){
                Terminal::WriteLine("td");
            }
            Terminal::WriteLine("this is a new line");
            
            return;
        }
    }
}