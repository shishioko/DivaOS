#include "Map.hpp"

namespace DivaOS::Boot::Memory::Map
{
    namespace{
        __attribute__((used, section(".limine_requests")))
        static volatile struct limine_memmap_request MapRequest = {
            .id = LIMINE_MEMMAP_REQUEST_ID,
            .revision = 0,
        };

        Shared::Memory::AddressRange* Cached = null;
    }
    Shared::Memory::AddressRange* Get(){
        //Return the cached pointer if it exists
        if (Cached != null) return Cached;
        //
        if (MapRequest.response == null) Boot::Crash("Memory Map not provided by bootloader!");
        //Organize the areas' boundaries into standalone boundary entries
        u64 rawEntries = MapRequest.response->entry_count * 2;
        void** rawEntriesAddress = new void*[rawEntries];
        bool* rawEntriesUsable = new bool[rawEntries];
        bool* rawEntriesStart = new bool[rawEntries];
        for (u64 i = 0; i < MapRequest.response->entry_count; i++){
            limine_memmap_entry* entry = MapRequest.response->entries[i];
            bool usable = entry->type == LIMINE_MEMMAP_USABLE;
            rawEntriesAddress[i * 2] = (void*)entry->base;
            rawEntriesAddress[i * 2 + 1] = (void*)(entry->base + entry->length);
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
        Shared::Memory::AddressRange* processedEntriesRange = new Shared::Memory::AddressRange[rawEntries];
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
            processedEntriesRange[processedEntries] = Shared::Memory::AddressRange::Null;
        }
        //Set the cached pointer and return
        return Cached = processedEntriesRange;
    }
}
