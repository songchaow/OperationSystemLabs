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
//用了**，为使nextTSK_StackPtr值能通过形参改变
{
    //add initial eflags:
    //when popped, user task will be running. so interrupt should be enabled
    *(*stk)-- = (unsigned long)514;//EFLAGS 高地址 【IF]+10
    *(*stk)-- = (unsigned long)0x08;       // CS 
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
void destroyTsk(myTCB* p) //释放空间。执行此函数时，要终止的任务一定处于队首
{
    p->tsk_state = 0;
}
myTCB* currTsk=glb_TCB;
void schedule() //runs for a short while every time.
//wrong: called when the time slice is used up. the current task is NOT finished!   
{
    asm("cli");//can be deleted
    if(currTsk&&currTsk->tsk_state==TSK_RUNNING)
    //called after Save_CTX. only to determine nextTSK_StackPtr
    //not empty and the current task is interrupted
    //suspend the current task and choose a new task to execute
    {
        currTsk->tsk_state = TSK_WAIT;
        /*
        //context switch
        Save_CTX();//need to adjust when debugging
        */
        currTsk->stack_top = prevTSK_StackPtr;
        //choose the next task to be executed
        if((currTsk->next!=r_queue.TCBtail)&&currTsk->next)//there is a next task and it's not the idle task
            currTsk = currTsk->next;
        else
            currTsk = r_queue.TCBhead;
        //currTsk:state READY or WAITING
        
        /*if(currTsk->tsk_state==READY)
        nextTSK_StackPtr = currTsk->stack_top;
        currTsk->tsk_state =TSK_RUNNING;
        Rest_CTX();*/
    }
    else if(currTsk&&currTsk->tsk_state==TSK_EMPTY)//from tskEnd
    {
        if((currTsk->next!=r_queue.TCBtail)&&currTsk->next)//there is a next task and it's not the idle task
            currTsk = currTsk->next;
        else
            currTsk = r_queue.TCBhead;
    }
    //deal with the new current task
    if(currTsk&&(currTsk->tsk_state==TSK_READY||currTsk->tsk_state==TSK_WAIT))
    //at the beginning start the first task || resume the suspended task
    {
            //run(); it's incorrect to call the function
            //println(WHITE,"next task sp:0x%x",currTsk->stack_top);
            nextTSK_StackPtr = (unsigned long)(currTsk->stack_top);
            if(currTsk->tsk_state==TSK_READY)
                stack_init((unsigned long **)&nextTSK_StackPtr, currTsk->taskBody);
        currTsk->tsk_state = TSK_RUNNING;
            PIC_sendEOI(0);//tell PIC that it can serve interrupt again
            Rest_CTX();
        
    }
    
    else
    {
        for(;;); //do nothing when no tasks
    }
}
void idleTskBody()
{
    for (;;)
    {
        //schedule();   //no need in RR algorithm
    }
}
void tick()//show time message
{
    static int count=0,i=0;
    count = (count+1)%100;
    if(count==0)
    {
        i++;
        put_char('0'+i%10,RED,2,50);
        //myprintf(WHITE,"tick%d!\n",i);
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
    asm("cli");    
    //examine memory:
    pMemInit();
    int TCBindex = 0;
    //ready-queue initialization:
    r_queue.TCBhead = 0;
    r_queue.TCBtail = 0;
    //create init idle task:
    createTsk(initTskBody);
    createTsk(idleTskBody);
    // Initialisation of 8259 PIC
    PIC_remap(0x20,0x28);
    // Initialization of PIT
    init_PIT();
    // Mask all other IRQs except IRQ0 (sys clk)
    IRQ_clear_mask(0);
    int i=1;
    for(i=1;i<=15;i++)
        IRQ_set_mask(i);
    stack_init((unsigned long **)&nextTSK_StackPtr, initTskBody);
    glb_TCB[0].tsk_state = TSK_RUNNING;
    Rest_CTX();
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
    //println(WHITE,"TASK SP=0x%x",p->stack_top);
    //println(WHITE,"globl SP=0x%x",stack_ptr);
    stack_ptr += TSK_STACK_INTERVAL;//bug3 fixed at 5:16 栈设的空间太小
    r_queue.tsk_num++;
    //link:
    //the first 2 tasks will be head and tail
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
void tskEnd() //执行此函数时，要终止的任务一定处于队首 now it doesn't!
//Todo: need to consider cases when the task is not the head
{
    //though the procesure may be interrupted and currTsk maybe modified,
    //when returned, the currTsk will be correct.
    asm("cli");
    destroyTsk(currTsk);
    //out of ready-queue: atomic operation

    if (r_queue.TCBhead==currTsk)       //currTsk is head
    {
        r_queue.TCBhead = r_queue.TCBhead->next;
    }
    else if(r_queue.TCBtail==currTsk)   //currTsk is tail
    {
        currTsk->priv->next=0;
        r_queue.TCBtail = currTsk->priv;
    }
    else if(currTsk)                    //currTsk is in the midst
        currTsk->priv = currTsk->next;
    else
    {
        panic("trying to end a task when there isn't one.\n");
    }
    r_queue.tsk_num--;
    if (r_queue.TCBhead == 0)
        r_queue.TCBtail = 0;
    asm("sti");
    schedule();
}

