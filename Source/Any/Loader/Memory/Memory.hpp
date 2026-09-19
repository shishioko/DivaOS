#pragma once
#include "Definitions.hpp"

namespace DivaOS {
    namespace Loader{
        namespace Memory{
            namespace Memory{
                void* Acquire(u64 size);
                void Release(void* address);
            }
        }
    }
}