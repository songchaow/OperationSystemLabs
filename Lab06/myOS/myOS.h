#ifndef __MYOS_H__
#define __MYOS_H__

#include "../userApp/userApp.h"

#define TASK_NUM (2 + USER_TASK_NUM) // at least: 0-idle, 1-init
#define TSK_EMPTY 0
#define TSK_CREATED -1
#define TSK_READY 1
#define TSK_RUNNING 2

#define TSK_STACK_INTERVAL 500 //两个任务之间的间隔
//=======Data structure ============================================
typedef struct TCB
{
    char tsk_state;
    /*
    0: used to identify whether it's occupied'
   -1: created but not allowed to run
    1: ready to run
    2: running
    //3: completed, waiting to be deleted
    */
    void (*taskBody)();
    //pointer to the body function. long pc_addr

    //CPU schedule information
    int priority; //not used in this lab
    
    //Memory-management information, not used
    int base_reg;
    int limit_reg;
    unsigned long* stack_top;
    /*link attributes:
    沿着priv走，靠近队首
    沿着next走，靠近队尾
    */
    struct TCB* priv;
    struct TCB* next;
} myTCB;
typedef struct ready_queue
{
    int tsk_num;
    myTCB* TCBhead;
    myTCB* TCBtail;
}myready_queue;
//=======OS API to user ============================================
void osStart(void);
int createTsk(void (*tskBody)(void));
void tskEnd(void);

//=======APP, user should define this==================================
void initTskBody(void); //init的task body。不是init tskbody这个过程

#endif
