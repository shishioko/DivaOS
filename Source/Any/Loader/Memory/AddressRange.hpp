#pragma once
#include "Definitions.hpp"

namespace DivaOS::Loader::Memory {
    /// @brief Describes a memory address range
    struct AddressRange{
        /// @brief Used to indicate the absence of a ``AddressRange``, such as for the termination of lists
        static const AddressRange Null;

        /// @brief The start address of the ``AddressRange``
        const void* Start;
        /// @brief The exclusive end address of the ``AddressRange``
        const void* End;

        /// @brief Constructs a new instance of ``AddressRange```
        AddressRange();
        /// @brief Constructs a new instance of ``AddressRange``
        /// @param start The start address of the ``AddressRange``
        /// @param end The exclusive end address of the ``AddressRange``
        AddressRange(const void* start, const void* end);

        const bool operator==(const AddressRange& b) const;
    };
}