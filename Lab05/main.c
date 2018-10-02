/*
    Without any C headers!
*/
char prompt[100] = "Hello World from main function!\n";
char color; //blue
char *video_mem;
char test;
int main()
{
    color = 0x0B;
    video_mem = 0xb8000;
    char *p = prompt;
    //static int test;
    for (; *p; p++)
    {
        *(video_mem++) = *p;
        *(video_mem++) = color;
    }
    while(1);
    return 0;
}