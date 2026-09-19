#include "Memory/Map.hpp"

#include "Memory/LoaderMemory.hpp"

#include "Peripherals/Terminal.hpp"

namespace DivaOS::Loader::Memory::Map {
    namespace{
        /// @brief Contains data about an address range fetched from the E820 memory map in real mode
        struct E820MemoryMapEntry{
            /// @brief Start of the address range
            const void* Start;
            /// @brief size of the address range
            const u64 Size;
            /// @brief Type of the address range
            /// @ref https://uefi.org/htmlspecs/ACPI_Spec_6_4_html/15_System_Address_Map_Interfaces/Sys_Address_Map_Interfaces.html
            const u32 Type;
            /// @brief 
            /// @ref https://uefi.org/htmlspecs/ACPI_Spec_6_4_html/15_System_Address_Map_Interfaces/int-15h-e820h---query-system-address-map.html
            const u32 Attributes;
            E820MemoryMapEntry() = delete;
        };

        /// @brief Holds the amount of entries in the fetched memory map
        extern u64 E820MemoryMapLength asm("DivaOS.Loader.Memory.Map.Length");
        /// @brief Holds the pointer to the first entry of the fetched memory map
        extern E820MemoryMapEntry* E820MemoryMap asm("DivaOS.Loader.Memory.Map.Start");

        AddressRange* Cached = null;
    }
    AddressRange* Get(){
        //Return the cached pointer if it exists
        if (Cached != null) return Cached;

        //Organize the areas' boundaries into standalone boundary entries
        u64 rawEntries = E820MemoryMapLength * 2;
        void** rawEntriesAddress = (void**)LoaderMemory::Acquire(sizeof(void*) * rawEntries);
        bool* rawEntriesUsable = (bool*)LoaderMemory::Acquire(sizeof(bool) * rawEntries);
        bool* rawEntriesStart = (bool*)LoaderMemory::Acquire(sizeof(bool) * rawEntries);
        for (u64 i = 0; i < E820MemoryMapLength; i++){
            E820MemoryMapEntry entry = E820MemoryMap[i];
            bool usable = entry.Type == 1;
            rawEntriesAddress[i * 2] = (void*)entry.Start;
            rawEntriesAddress[i * 2 + 1] = (void*)(entry.Start + entry.Size);
            rawEntriesUsable[i * 2] = usable;
            rawEntriesUsable[i * 2 + 1] = usable;
            rawEntriesStart[i * 2] = true;
            rawEntriesStart[i * 2 + 1] = false;
        }
        //Align the entries' addresses to page boundaries
        for (u64 i = 0; i < rawEntries; i++){
            bool alignUp = rawEntriesUsable[i] == rawEntriesStart[i];;
            rawEntriesAddress[i] = (void*)(((u64)rawEntriesAddress[i] + (alignUp ? 0x0FFF : 0x0000)) & ~0x0FFF);
        }
        //Bubble sort the entries
        while (true){
            u64 sorted = 0;
            //Iterate the entries in pairs
            for (u64 i = 0; i + 1 < rawEntries; i++){
                void* address = rawEntriesAddress[i];
                bool usable = rawEntriesUsable[i];
                bool start = rawEntriesStart[i];
                //Swap the entries if they are not sorted
                if (rawEntriesAddress[i + 1] < address){
                    rawEntriesAddress[i] = rawEntriesAddress[i + 1];
                    rawEntriesUsable[i] = rawEntriesUsable[i + 1];
                    rawEntriesStart[i] = rawEntriesStart[i + 1];
                    rawEntriesAddress[i + 1] = address;
                    rawEntriesUsable[i + 1] = usable;
                    rawEntriesStart[i + 1] = start;
                    //Note that there are still entries that were sorted in this iteration
                    sorted++;
                }
            }
            //Exit if all entries were sorted, continue otherwise
            if (sorted <= 0) break;
        }
        //Process which entries form usable areas
        AddressRange* processedEntriesRange = (AddressRange*)LoaderMemory::Acquire(sizeof(AddressRange) * rawEntries);
        u64 processedEntries = 0;
        {
            s64 currentUsableScore = 0;
            s64 currentUnusableScore = 0;
            for (u64 i = 0; i < rawEntries; i++){
                s64 previousUsableScore = currentUsableScore;
                s64 previousUnusableScore = currentUnusableScore;
                void* address = rawEntriesAddress[i];
                bool usable = rawEntriesUsable[i];
                bool start = rawEntriesStart[i];
                //Track the current depth into usable and unusable entries
                (usable ? currentUsableScore : currentUnusableScore) += start ? 1 : -1;

                //Determine if the range following the current entry is usable
                s64 currentActualScore = currentUnusableScore > 0 ? 0 : currentUsableScore;
                s64 previousActualScore = previousUnusableScore > 0 ? 0 : previousUsableScore;

                //Save the area if the following range is not usable
                if (previousActualScore > 0 && currentActualScore <= 0){
                    if (processedEntriesRange[processedEntries].Start < address){
                    processedEntriesRange[processedEntries].End = address;
                        processedEntries++;
                    }
                }
                //Start an area if the following area is usable
                if (previousActualScore <= 0 && currentActualScore > 0){
                    processedEntriesRange[processedEntries].Start = address;
                }
            }
            //Set the last entry to terminate the list
            processedEntriesRange[processedEntries] = AddressRange::Null;
        }
        //Set the cached pointer and return
        return Cached = processedEntriesRange;
    }
}