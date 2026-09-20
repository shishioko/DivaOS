#pragma once
#include "Definitions.hpp"

/// @brief Contains basic methods to output to a graphical terminal during bootup
namespace DivaOS::Boot::Terminal {
    
    /// @brief Writes text to the terminal
    /// @param text the text to be written
    extern void Write(const t8* text);
    /// @brief Writes a single character to the terminal
    /// @param character the character to be written
    extern void Write(const t8 character);

    /// @brief Writes a line of text to the terminal
    /// @param text the text to be written
    extern inline void WriteLine(const t8* text);
    
    inline void WriteLine(const t8* text){
        Write(text);
        Write('\n');
    }
}