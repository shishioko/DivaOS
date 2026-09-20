#include "Shared/Memory/AddressRange.hpp"

namespace DivaOS::Shared::Memory {
    const AddressRange AddressRange::Null = AddressRange();

    AddressRange::AddressRange(){
        Start = null;
        End = null;
    }
    AddressRange::AddressRange(const void* start, const void* end){
        Start = start;
        End = end;
    }

    const bool AddressRange::operator==(const AddressRange& b) const {
        return Start == b.Start && End == b.End;
    }
}