/* init.c */
#include "../myOS/myOS.h"

char *message1 = "********************************\n\0";
int row=0,col=0;	

void myTSK0(void){  
  char *message2 = "*     TSK0: HELLO WORLD!       *\n\0";
	long i = 100000;
	while(i-->=0) myprintf(WHITE,message2);
	
	tskEnd();   //the task is end
}

void myTSK1(void){
  char *message2 = "*     TSK1: Now it's my turn!!       *\n\0";
	long i = 100000;
	while(i-->=0) myprintf(WHITE,message2);
	
	tskEnd();   //the task is end
}

void myTSK2(void){
  char *message2 = "*     TSK2: Go away!!!       *\n\0";
	long i = 100000;
	while(i-->=0) myprintf(WHITE,message2);
	tskEnd();  //the task is end
}

void initTskBody(void){
  char *message2 = "*        INIT  INIT !          *\n\0";

	createTsk(myTSK0);
	createTsk(myTSK1);
	createTsk(myTSK2);
  tskEnd();  // init end
}
