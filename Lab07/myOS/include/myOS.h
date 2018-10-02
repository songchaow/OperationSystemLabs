#ifndef __MYOS_H__
#define __MYOS_H__

#include <userApp.h>
//-----Memory--------
#define VGA_BASE 0xb8000
#define VGA_rows  25
#define VGA_cols  80
#define WHITE 0x7
#define RED 0xC
#define MEMORYLIMIT (1024 * 20) //20M
//-----Task----------
#define TASK_NUM (2 + USER_TASK_NUM) // at least: 0-idle, 1-init
#define TSK_EMPTY 0
#define TSK_CREATED -1
#define TSK_READY 1
#define TSK_RUNNING 2

#define STACK_ADDRESS 0x1000000 //栈空间起始地址
#define TSK_STACK_INTERVAL 0x10000 //两个任务之间的间隔
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
    unsigned long *stack_top;
    /*link attributes:
    沿着priv走，靠近队首
    沿着next走，靠近队尾
    */
    struct TCB *priv;
    struct TCB *next;
} myTCB;
typedef struct ready_queue
{
    int tsk_num;
    myTCB *TCBhead;
    myTCB *TCBtail;
} myready_queue;
//=======OS API to user ============================================
//scheduling API:
void osStart(void);
int createTsk(void (*tskBody)(void));
void tskEnd(void);
//memory API:
void pMemInit();
unsigned long dPartitionInit(unsigned long start, unsigned long size);//初始化并返回dPartition句柄
unsigned long dPartitionAlloc(unsigned long size);  //0失败；其他：所分配到的内存块起始地址
unsigned long dPartitionFree(unsigned long start); //0失败；1成功

unsigned long eFPartitionInit(unsigned long start, unsigned long perSize, unsigned long n); //初始化并返回eFPartition句柄
unsigned long eFPartitionTotalSize(unsigned long perSize, unsigned long n); //根据单位大小和单位个数，计算出eFPartition所需内存大小
unsigned long eFPartitionAlloc(unsigned long EFPHandler); //0失败；其他：所分配到的内存块起始地址
int eFPartitionFree(unsigned long EFPHandler,unsigned long mbStart); //0失败；1成功

#define malloc(size)  dPartitionAlloc(size)
#define free(start)		dPartitionFree(start)

int myprintf(char color,const char *fmt, ...);
int println(char color,const char *fmt, ...);
void showln(char *, char, int);
//=======APP, user should define this==================================
void initTskBody(void); //init的task body。不是init tskbody这个过程

#endif
