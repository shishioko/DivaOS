#pragma once
#include "Definitions.hpp"

#include <Limine/limine.h>
#include <Flanterm/flanterm_backends/fb.h>

#include "Boot/Main.hpp"

/// @brief Contains basic methods to output to a graphical terminal during bootup
namespace DivaOS::Boot::Peripherals::Terminal {
    
    /// @brief Used to initialize the internal context of the terminaal
    /// @note Must be called before calling any other method
    /// @return Whether the initialization succeded or not
    const bool Initialize();

    /// @brief Writes a single character to the terminal
    /// @param character the character to be written
    void Write(const t8 character);
    /// @brief Writes text to the terminal
    /// @param text the text to be written
    void Write(const t8* text);

    /// @brief Writes a line of text to the terminal
    /// @param text the text to be written
    inline void WriteLine(const t8* text);
    
    inline void WriteLine(const t8* text){
        Write(text);
        Write("\r\n");
    }
}