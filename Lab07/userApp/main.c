#include <userApp.h>
#include <myOS.h>
void count_memory();
//=========================my Main===========================================
void clear_screen(void)
{
    int row, col;
    unsigned char *ptr = (unsigned char *)VGA_BASE;
    for (row = 0; row < VGA_rows; row++)
    {
	for (col = 0; col < VGA_cols; col++)
	{
	    (*ptr++) = 0;
        (*ptr++) = WHITE;
	}
    }
}
int main(void){

	clear_screen();

	myprintf(0x7,"myMain:HELLO WORLD!\n");	
	osStart();

	//never here   
	for(;;); 	
	return 0;
}


