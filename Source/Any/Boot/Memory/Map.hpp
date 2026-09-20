#pragma once

#include <Limine/limine.h>

#include "Boot/Main.hpp"
#include "Shared/Memory/AddressRange.hpp"

namespace DivaOS::Boot::Memory::Map
{
    Shared::Memory::AddressRange* Get();
}
