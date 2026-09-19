extern "C" void Main(void)
{
    *( (volatile int *) 0x00000000000B8000 ) = 'G';
    *( (volatile int *) 0x00000000000B8001 ) = 0x04;
    const char* t = "welcome to c";
	repeat: 	goto repeat;
    return;
}