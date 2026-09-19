#include "Memory/AddressRange.hpp"

namespace DivaOS::Loader::Memory {
    const AddressRange AddressRange::Null = AddressRange(null, null);

    AddressRange::AddressRange(const void* start, const void* end){
        this->Start = start;
        End = end;
    }

    const bool AddressRange::operator==(const AddressRange& b) const {
        return Start == b.Start && End == b.End;
    }
}