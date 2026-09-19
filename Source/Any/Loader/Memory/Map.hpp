#pragma once
#include "Definitions.hpp"
#include "Memory/AddressRange.hpp"

namespace DivaOS::Loader::Memory::Map {
    /// @brief Used to decode the E820 memory map
    /// @note The returned pointer may be cached
    /// @return The decoded memory map
    AddressRange* Get();
}