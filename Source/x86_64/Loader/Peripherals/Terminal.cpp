#include "Peripherals/Terminal.hpp"

namespace DivaOS::Loader::Peripherals::Terminal {
    namespace
    {
        struct VGACharacter{
            t8 Character;
            u8 ColorMask;

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
        
        v VGACharacter* VGA = (v VGACharacter*) 0x00000000000B8000;
        const u16 Width = 80;
        const u16 Height = 25;
        u16 Y = 0;
        u16 X = 0;
    }

    static const void ScrollUp(u16 y);
    static inline v VGACharacter* CalculateAddress(u16 x, u16 y);

    void Write(const t8* text){
        for (u64 i = 0; text[i] != '\0'; i++) {
            u8 c = text[i];
            Write(c);
        }
    }
    void Write(const t8 character){
        switch (character){
            case '\a':{
                //unsupported
                break;
            }
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
            case '\r':{
                X = 0;
                break;
            }
            case '\n':{
                X = 0;
                Y++;
                if (Y == Height){
                    ScrollUp(1);
                }
                break;
            }
            case '\t':{
                do
                {
                    Write(' ');
                }
                while (X % 4 != 0);
                break;
            }
            default:{
                v VGACharacter* vga = CalculateAddress(X, Y);
                vga[0] = VGACharacter(character);
                X++;
                if (X == Width){
                    X = 0;
                    Y++;
                }
                if (Y == Height){
                    ScrollUp(1);
                }
            }
        }
    }

    static const void ScrollUp(u16 y){
        Y -= y;
        for (u16 line = y; line < Height; line++){
            v VGACharacter* destination = CalculateAddress(0, line - y);
            v VGACharacter* source = CalculateAddress(0, line);
            for (u16 i = 0; i < Width; i++){
                destination[i] = source[i];
                source[i] = VGACharacter(' ', 0x00);
            }
        }
    }
    static inline v VGACharacter* CalculateAddress(u16 x, u16 y){
        return &VGA[y * Width + x];
    }
}