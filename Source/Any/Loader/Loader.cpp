#include "Loader.hpp"

namespace DivaOS {
    namespace Loader{
        namespace Loader{
            void Main()
            {
                Terminal::WriteLine("Hello, World!");
                for (u64 i = 0; i < 22; i++){
                    Terminal::WriteLine("td");
                }
                Terminal::WriteLine("this is a new line");

                return;
            }
            void Crash(const t8* text){
                Terminal::Write(text);
                while (true);
            }
        }
    }
}