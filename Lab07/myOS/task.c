#include <myOS.h>
extern unsigned long *prevTSK_StackPtr;
extern unsigned long *nextTSK_StackPtr;
void idleTskBody();
//Global Data:
myTCB glb_TCB[TASK_NUM + 10]; //malloc space for TCB
myready_queue r_queue;

//Internel functions:
void panic(char* str)
{
	put_char(str,RED,0,0);
	for(;;);
}
void stack_init(unsigned long **stk, void (*task)(void))
{
    *(*stk)-- = (unsigned long)0x08;       // CS 高地址
    *(*stk)-- = (unsigned long)task;       // eip
    *(*stk)-- = (unsigned long)0xAAAAAAAA; // EAX
    *(*stk)-- = (unsigned long)0xCCCCCCCC; // ECX
    *(*stk)-- = (unsigned long)0xDDDDDDDD; // EDX
    *(*stk)-- = (unsigned long)0xBBBBBBBB; // EBX
    *(*stk)-- = (unsigned long)0x44444444; // ESP
    *(*stk)-- = (unsigned long)0x55555555; // EBP
    *(*stk)-- = (unsigned long)0x66666666; // ESI
    *(*stk) = (unsigned long)0x77777777;   // EDI 低地址
}
void tskStart(myTCB* p) //将队尾（一定是刚加进来的）任务设为可运行
{
    p->tsk_state = TSK_READY;
}
void destroyTsk() //释放空间。执行此函数时，要终止的任务一定处于队首
{
    r_queue.TCBhead->tsk_state = 0;
}

void schedule() //runs for a short while every time.
{
    if (r_queue.TCBhead) //not empty
    {
        if (r_queue.TCBhead->tsk_state == 1)
        {
            r_queue.TCBhead->tsk_state = TSK_RUNNING;
            void (*run)(void) = r_queue.TCBhead->taskBody;
            //run(); it's incorrect to call the function
            println(WHITE,"next task sp:0x%x",r_queue.TCBhead->stack_top);
            nextTSK_StackPtr = (unsigned long)(r_queue.TCBhead->stack_top);
            stack_init((unsigned long **)&nextTSK_StackPtr, run);//本实验中，每次schedule都是执行新任务。
            CTX_SW();
        }
    }
    else
    {
        ; //do nothing when no tasks
    }
}
void idleTskBody()
{
    for (;;)
    {
        schedule();
    }
}

//Interfaces:
void osStart(void)
/*
    initialize ready-queue(malloc space)
        create init, idle task.
    initialize memory management data structure
    transfer the control power

    int *p
    int *p[5]=(int*)p[5]
    int (*p)[5]
*/
{
    //examine memory:
    pMemInit();
    int TCBindex = 0;
    //ready-queue initialization:
    r_queue.TCBhead = 0;
    r_queue.TCBtail = 0;
    //create init idle task:
    createTsk(initTskBody);
    createTsk(idleTskBody);
    
    stack_init((unsigned long **)&nextTSK_StackPtr, initTskBody);
    CTX_SW();
}
    unsigned long stack_ptr = 0x1000000L;//bug3 fixed at 5:16 栈设的空间太小
int createTsk(void (*tskBody)(void))
//创建新任务，并将新任务插到队尾
{
    //initialize PCB:
    myTCB *p = glb_TCB;
    //find a spare TCB place:
    for (; p <= glb_TCB + TASK_NUM + 9; p++)
    {
        if (p->tsk_state == 0)
            break; //break if unoccupied
    }
    if (p == glb_TCB + TASK_NUM + 10)
    {
        panic("too many tasks!\n");
    }

    //fill in values:
    p->tsk_state = TSK_CREATED;
    p->taskBody = tskBody;
    p->stack_top = (unsigned long*)stack_ptr;
    println(WHITE,"TASK SP=0x%x",p->stack_top);
    println(WHITE,"globl SP=0x%x",stack_ptr);
    stack_ptr += TSK_STACK_INTERVAL;//bug3 fixed at 5:16 栈设的空间太小
    r_queue.tsk_num++;
    //link:
    if (r_queue.TCBhead == 0) //empty queue
    {
        r_queue.TCBhead = p;
        //r_queue.TCBtail = p;
        p->priv = 0;
        p->next = r_queue.TCBtail;
    }
    else if(r_queue.TCBtail == 0)
    {
        r_queue.TCBtail = p;
        r_queue.TCBhead->next = r_queue.TCBtail;
        p->priv = r_queue.TCBhead;
        p->next =0;
    }
    else
    {
        r_queue.TCBtail->priv->next = p;
        r_queue.TCBtail->priv = p;
        p->priv = r_queue.TCBtail->priv;
        p->next = r_queue.TCBtail;
        //r_queue.TCBtail = p;
    }
    //p->next = 0;
    

    tskStart(p); //只是打开一个开关而已，表示这个任务可以被运行了。真正准不准许运行还得看scheduler的安排
    return 0;
}
void tskEnd(void) //执行此函数时，要终止的任务一定处于队首
{
    destroyTsk();
    prevTSK_StackPtr = r_queue.TCBhead->stack_top;
    //把CTX_SW的参数准备好。不过反正不会回来运行了，保存也没什么卵用。
    //out of ready-queue:
    if (r_queue.TCBhead)
    {
        r_queue.TCBhead = r_queue.TCBhead->next;
        r_queue.tsk_num--;
    }
    else
    {
        panic("trying to end a task when there isn't one.\n");
    }
    if (r_queue.TCBhead == 0)
        r_queue.TCBtail = 0;
    schedule();
}

