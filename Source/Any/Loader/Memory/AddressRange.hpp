#pragma once
#include "Definitions.hpp"

namespace DivaOS::Loader::Memory {
    struct AddressRange{
        static const AddressRange Null;

        const void* Start;
        const void* End;

        AddressRange(const void* start, const void* end);

        const bool operator==(const AddressRange& b) const;
    };
}