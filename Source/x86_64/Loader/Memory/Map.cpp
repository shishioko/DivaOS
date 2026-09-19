#include "Memory/Map.hpp"

#include "Memory/LoaderMemory.hpp"
#include "Memory/E820MemoryMapEntry.hpp"

#include "Peripherals/Terminal.hpp"

namespace DivaOS::Loader::Memory::Map {
    extern u64 E820MemoryMapLength asm("DivaOS.Loader.Memory.Map.Length");
    extern E820MemoryMapEntry* E820MemoryMap asm("DivaOS.Loader.Memory.Map.Start");
    t8* convert_u64_to_hex_string(u64 value);
    t8* convert_u8_to_hex_string(u8 value);
    AddressRange* Get(){
        u64 raw_entries = E820MemoryMapLength * 2;
        void** raw_entries_address = (void**)LoaderMemory::Acquire(sizeof(void*) * raw_entries);
        bool* raw_entries_usable = (bool*)LoaderMemory::Acquire(sizeof(bool) * raw_entries);
        bool* raw_entries_start = (bool*)LoaderMemory::Acquire(sizeof(bool) * raw_entries);
        //Peripherals::Terminal::Write('0' + (const t8)E820MemoryMapLength);
        for (u64 i = 0; i < E820MemoryMapLength; i++){
            E820MemoryMapEntry entry = E820MemoryMap[i];
            bool usable = entry.Type == 1;
            raw_entries_address[i * 2] = (void*)entry.Start;
            raw_entries_address[i * 2 + 1] = (void*)(entry.Start + entry.Size);
            raw_entries_usable[i * 2] = usable;
            raw_entries_usable[i * 2 + 1] = usable;
            raw_entries_start[i * 2] = true;
            raw_entries_start[i * 2 + 1] = false;
        }
        for (u64 i = 0; i < raw_entries; i++){
            bool alignUp = raw_entries_usable[i] == raw_entries_start[i];;
            raw_entries_address[i] = (void*)(((u64)raw_entries_address[i] + (alignUp ? 0x0FFF : 0x0000)) & ~0x0FFF);
        }
        while (true){
            u64 sorted = 0;
            for (u64 i = 0; i + 1 < raw_entries; i++){
                void* address = raw_entries_address[i];
                bool usable = raw_entries_usable[i];
                bool start = raw_entries_start[i];
                if (raw_entries_address[i + 1] < address){
                    raw_entries_address[i] = raw_entries_address[i + 1];
                    raw_entries_usable[i] = raw_entries_usable[i + 1];
                    raw_entries_start[i] = raw_entries_start[i + 1];
                    raw_entries_address[i + 1] = address;
                    raw_entries_usable[i + 1] = usable;
                    raw_entries_start[i + 1] = start;
                    sorted++;
                }
            }
            if (sorted <= 0) break;
        }
        AddressRange* processed_entries_range = (AddressRange*)LoaderMemory::Acquire(sizeof(AddressRange) * raw_entries);
        u64 processed_entries = 0;
        {
            s64 current_usable_score = 0;
            s64 current_unusable_score = 0;
            for (u64 i = 0; i < raw_entries; i++){
                s64 previous_usable_score = current_usable_score;
                s64 previous_unusable_score = current_unusable_score;
                void* address = raw_entries_address[i];
                bool usable = raw_entries_usable[i];
                bool start = raw_entries_start[i];
                (usable ? current_usable_score : current_unusable_score) += start ? 1 : -1;

                s64 current_actual_score = current_unusable_score > 0 ? 0 : current_usable_score;
                s64 previous_actual_score = previous_unusable_score > 0 ? 0 : previous_usable_score;

                if (previous_actual_score > 0 && current_actual_score <= 0){
                    if (processed_entries_range[processed_entries].Start < address){
                    processed_entries_range[processed_entries].End = address;
                        processed_entries++;
                    }
                }
                if (previous_actual_score <= 0 && current_actual_score > 0){
                    processed_entries_range[processed_entries].Start = address;
                }
            }
            processed_entries_range[processed_entries] = AddressRange::Null;
        }
        return processed_entries_range;
    }

    t8* convert_u64_to_hex_string(u64 value) {
        const u64 buffer_size = 18; // 2 for "0x" + 16 for hex digits
        
        // Acquire memory allocation using the specified method
        void* allocated_ptr = LoaderMemory::Acquire(buffer_size);
        if (!allocated_ptr) {
            return nullptr; // Handle allocation failure safely
        }
        
        t8* buffer = static_cast<t8*>(allocated_ptr);
        
        // 1. Write the hexadecimal prefix
        buffer[0] = '0';
        buffer[1] = 'x';
        
        // Look-up table for hex characters to avoid branching/math
        const char hex_chars[] = "0123456789abcdef";
        
        // 2. Process each nibble (4 bits) from most significant to least significant.
        // Iterating backwards from index 17 down to 2 ensures correct endianness rendering.
        for (int i = 17; i >= 2; --i) {
            buffer[i] = static_cast<t8>(hex_chars[value & 0xF]);
            value >>= 4; // Shift right by 4 bits to process the next nibble
        }
        
        return buffer;
    }
    t8* convert_u8_to_hex_string(u8 value) {
        const u64 buffer_size = 4; // 2 for "0x" + 2 for hex digits
        
        // Acquire memory allocation using the specified method
        void* allocated_ptr = LoaderMemory::Acquire(buffer_size);
        if (!allocated_ptr) {
            return nullptr; // Handle allocation failure safely
        }
        
        t8* buffer = static_cast<t8*>(allocated_ptr);
        
        // 1. Write the hexadecimal prefix
        buffer[0] = '0';
        buffer[1] = 'x';
        
        // Look-up table for hex characters to avoid branching/math
        const char hex_chars[] = "0123456789abcdef";
        
        // 2. Process each nibble (4 bits) from most significant to least significant.
        // Iterating backwards from index 3 down to 2 ensures correct padding and layout.
        for (int i = 3; i >= 2; --i) {
            buffer[i] = static_cast<t8>(hex_chars[value & 0xF]);
            value >>= 4; // Shift right by 4 bits to process the next nibble
        }
        
        return buffer;
    }
}