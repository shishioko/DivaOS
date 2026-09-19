#pragma once
#include "Definitions.hpp"

/// @brief Contains basic methods to output to the firmware provided VGA text terminal
namespace DivaOS::Loader::Peripherals::Terminal {
    
    /// @brief Writes text to the terminal
    /// @param text the text to be written
    extern void Write(const t8* text) asm("DivaOS.Loader.Peripherals.Terminal.Write_t8p");
    /// @brief Writes a single character to the terminal
    /// @param character the character to be written
    extern void Write(const t8 character) asm("DivaOS.Loader.Peripherals.Terminal.Write_t8");

    /// @brief Writes a line of text to the terminal
    /// @param text the text to be written
    extern inline void WriteLine(const t8* text) asm("DivaOS.Loader.Peripherals.Terminal.WriteLine_t8p");
    
    inline void WriteLine(const t8* text){
        Write(text);
        Write('\n');
    }
}