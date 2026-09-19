#include "Peripherals/Terminal.hpp"

#include "Loader.hpp"

namespace DivaOS::Loader::Peripherals::Terminal {
    namespace
    {
        /// @brief The encoded character format in the BIOS provided VGA text mode framebuffer
        struct VGACharacter{
            /// @brief The displayed ASCII character
            t8 Character;
            /// @brief The displayed character's color and animation (0-3 foreground, 4-6 background, 7 blinking)
            u8 ColorMask;

            /// @brief Constructs a new instance of ``VGACharacter``
            /// @param character The displayed ASCII character
            /// @param colorMask The displayed character's color and animation (0-3 foreground, 4-6 background, 7 blinking)
            VGACharacter(const t8 character, const u8 colorMask = 0x0F){
                Character = character;
                ColorMask = colorMask;
            }

            void operator=(const VGACharacter& b) v {
                this->Character = b.Character;
                this->ColorMask = b.ColorMask;
            }
            void operator=(const v VGACharacter& b) v {
                this->Character = b.Character;
                this->ColorMask = b.ColorMask;
            }
        };
        
        /// @brief The BIOS provided VGA text mode framebuffer
        v VGACharacter* VGA = (v VGACharacter*) 0x00000000000B8000;
        /// @brief The width of the displayed framebuffer
        const u16 Width = 80;
        /// @brief The height of the displayed framebuffer
        const u16 Height = 25;
        /// @brief The vertical cursor position
        u16 Y = 0;
        /// @brief The horizontal cursor position
        u16 X = 0;

        /// @brief Used to copy the screen upwards.
        /// @param y The lines to move the screen upwards
        static const void ScrollUp(const u16 y);
        /// @brief Used to calculate the pointer to a specific character by position on the BIOS provided VGA terminal
        /// @param x The vertical position
        /// @param y The horizontal position
        /// @return A pointer to a ``VGACharacter`` to modify the character at that position
        static inline v VGACharacter* CalculateAddress(u16 x, u16 y);

        static const void ScrollUp(const u16 y){
            Y -= y;
            //Iterate the lines to move
            for (u16 line = y; line < Height; line++){
                v VGACharacter* destination = CalculateAddress(0, line - y);
                v VGACharacter* source = CalculateAddress(0, line);
                //Iterate the characters of the line to move
                for (u16 i = 0; i < Width; i++){
                    //Copy the character to its new location
                    destination[i] = source[i];
                    //Clear the previous location of the characters
                    source[i] = VGACharacter(' ', 0x00);
                }
            }
            //Ensure all lower lines are cleared
            for (u16 line = 0; line < y && line < Height; line++){
                v VGACharacter* vga = CalculateAddress(0, Height - line - 1);
                //Iterate the characters of the line to clear
                for (u16 i = 0; i < Width; i++){
                    vga[i] = VGACharacter(' ', 0x00);
                }
            }
        }
        static inline v VGACharacter* CalculateAddress(const u16 x, const u16 y){
            if (x > Width || y > Height) {
                Loader::Crash("Tried to access framebuffer out of bounds");
            }
            return &VGA[y * Width + x];
        }
    }
    
    void Write(const t8* text){
        //Iterate the characters until encountering a null terminator
        for (u64 i = 0; text[i] != '\0'; i++) {
            //Write the individual character to the terminal
            u8 c = text[i];
            Write(c);
        }
    }
    void Write(const t8 character){
        switch (character){
            //Notification character
            case '\a':{
                //unsupported
                break;
            }
            //Backspace
            case '\b':{
                if (X != 0){
                    X--;
                }
                else
                {
                    if (Y != 0) Y--;
                    X = Width - 1;
                }
                break;
            }
            //Carriage return
            case '\r':{
                X = 0;
                break;
            }
            //Newline
            case '\n':{
                X = 0;
                Y++;
                //Scroll up if the screen is full
                if (Y == Height){
                    ScrollUp(1);
                }
                break;
            }
            //Tab
            case '\t':{
                do
                {
                    Write(' ');
                }
                while (X % 4 != 0);
                break;
            }
            //Any other character
            default:{
                //Print the character
                v VGACharacter* vga = CalculateAddress(X, Y);
                vga[0] = VGACharacter(character);
                //Update the cursor position
                X++;
                //Wrap the line if the line is full
                if (X == Width){
                    X = 0;
                    Y++;
                }
                //Scroll up if the screen is full
                if (Y == Height){
                    ScrollUp(1);
                }
                break;
            }
        }
    }
}