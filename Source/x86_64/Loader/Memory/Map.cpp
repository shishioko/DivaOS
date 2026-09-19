#include "Memory/Map.hpp"

#include "Memory/LoaderMemory.hpp"
#include "Memory/E820MemoryMapEntry.hpp"

#include "Peripherals/Terminal.hpp"

namespace DivaOS::Loader::Memory::Map {
    extern u64 E820MemoryMapLength asm("DivaOS.Loader.Memory.Map.Length");
    extern E820MemoryMapEntry* E820MemoryMap asm("DivaOS.Loader.Memory.Map.Start");

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
}