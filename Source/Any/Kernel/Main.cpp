namespace DivaOS {
    extern "C" void Main() asm("_start");
    void Main()
    {
        *( (volatile char *) 0x00000000000B8002 ) = 'G';
        *( (volatile char *) 0x00000000000B8003 ) = 0x04;
        const char* t = "welcome to c";
        return;
    }
}