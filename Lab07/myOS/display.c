/*
display.c
output functions
*/
#include <myOS.h>
//#include <stdio.h>
#include <stdarg.h>
int vsprintf(char *buf, const char *fmt, va_list args);
int curr_row = 0;
int curr_col = 0;

void put_char(char c, char color, int row, int col)
{
    unsigned char *ptr = (unsigned char *)VGA_BASE;
    unsigned short value;

    ptr[(row * 80 + col) * 2] = c;
    ptr[(row * 80 + col) * 2 + 1] = color;
}
/*void str(int num)
{
    char i = 0, j = 0;
    char tempstr[20];
    int tail = 0;
    while (num)
    {
        tail = num % 10; //取10的余数
        tempstr[i++] = (char)tail + '0';
        num /= 10;
    }
    tempstr[i] = 0;
    //invert: 0, 1, 2, ..., i-1
    while (j < i)
    {
        numstr[j] = tempstr[i - j - 1];
        j++;
    }
    numstr[j] = 0; //now j=i
}*/
//int vsprintf(char *buf, const char *fmt, va_list args);//with fixed parameters
int myprintf(char color,const char *fmt, ...)
/*
    print characters without newilne explicitly
*/
{
    char strbuffer[100];//buffer
    char* buffer=strbuffer;
    va_list args;
    int n;
    char c;
    //create buffer:只能调用vspirntf,其参数数量固定.
    va_start(args,fmt);//position after "fmt" -> args
    n = vsprintf(buffer,fmt,args);
    va_end(args);
    //output from buffer:
    c = *buffer;
    while (c != '\0')
    {
        if(c=='\n')//deal with new line
        {
            for(;curr_col<80;curr_col++)
            put_char(0,color,curr_row,curr_col);
            curr_col = 0;
            curr_row=(curr_row+1)%25;
        }
        else
        {
        if (curr_col == 80)
        {
            curr_col = 0;
            curr_row=(curr_row+1)%25;
        }
        put_char(c, color, curr_row, curr_col++);
        }
        c = *(++buffer);
    }
    return n;
}
int println(char color,const char *fmt, ...)
{
    char strbuffer[100];//buffer
    char* buffer=strbuffer;
    va_list args;
    int n;
    char c;
    //create buffer:只能调用vspirntf,其参数数量固定.
    va_start(args,fmt);//position after "fmt" -> args
    n = vsprintf(buffer,fmt,args);
    va_end(args);
    //output from buffer:

    c = *buffer;
    while (c != '\0')
    {
        if (curr_col == 80)
        {  
            curr_col = 0;
            curr_row=(curr_row+1)%25;
        }

        put_char(c, color, curr_row, curr_col++);
        c = *(++buffer);
    }
    if (curr_col > 0)
    {
        for(;curr_col<80;curr_col++)
            put_char(0,color,curr_row,curr_col);
        curr_col = 0;
        curr_row=(curr_row+1)%25;
    }
    return n;
}
void showln(char *msg, char color, int row) //显示一行消息
{
    char c = *msg;
    int ln = 0;
    while (c != '\0')
    {
        put_char(c, WHITE, row, ln++);
        c = *(++msg);
    }
}