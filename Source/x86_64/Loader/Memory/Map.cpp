#include "Memory/Map.hpp"

#include "Memory/LoaderMemory.hpp"

#include "Peripherals/Terminal.hpp"

namespace DivaOS::Loader::Memory::Map {
    namespace{
        struct E820MemoryMapEntry{
            const void* Start;
            const u64 Size;
            const u32 Type;
            const u32 Attributes;
            E820MemoryMapEntry() = delete;
        };

        extern u64 E820MemoryMapLength asm("DivaOS.Loader.Memory.Map.Length");
        extern E820MemoryMapEntry* E820MemoryMap asm("DivaOS.Loader.Memory.Map.Start");

        AddressRange* Cached = null;
    }
    AddressRange* Get(){
        if (Cached != null) return Cached;
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
        for (u64 i = 0; i < rawEntries; i++){
            bool alignUp = rawEntriesUsable[i] == rawEntriesStart[i];;
            rawEntriesAddress[i] = (void*)(((u64)rawEntriesAddress[i] + (alignUp ? 0x0FFF : 0x0000)) & ~0x0FFF);
        }
        while (true){
            u64 sorted = 0;
            for (u64 i = 0; i + 1 < rawEntries; i++){
                void* address = rawEntriesAddress[i];
                bool usable = rawEntriesUsable[i];
                bool start = rawEntriesStart[i];
                if (rawEntriesAddress[i + 1] < address){
                    rawEntriesAddress[i] = rawEntriesAddress[i + 1];
                    rawEntriesUsable[i] = rawEntriesUsable[i + 1];
                    rawEntriesStart[i] = rawEntriesStart[i + 1];
                    rawEntriesAddress[i + 1] = address;
                    rawEntriesUsable[i + 1] = usable;
                    rawEntriesStart[i + 1] = start;
                    sorted++;
                }
            }
            if (sorted <= 0) break;
        }
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
                (usable ? currentUsableScore : currentUnusableScore) += start ? 1 : -1;

                s64 currentActualScore = currentUnusableScore > 0 ? 0 : currentUsableScore;
                s64 previousActualScore = previousUnusableScore > 0 ? 0 : previousUsableScore;

                if (previousActualScore > 0 && currentActualScore <= 0){
                    if (processedEntriesRange[processedEntries].Start < address){
                    processedEntriesRange[processedEntries].End = address;
                        processedEntries++;
                    }
                }
                if (previousActualScore <= 0 && currentActualScore > 0){
                    processedEntriesRange[processedEntries].Start = address;
                }
            }
            processedEntriesRange[processedEntries] = AddressRange::Null;
        }
        return Cached = processedEntriesRange;
    }
}