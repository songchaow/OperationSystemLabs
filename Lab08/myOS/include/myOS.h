#ifndef __MYOS_H__
#define __MYOS_H__

#include <userApp.h>
#include <stdint.h>
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
#define TSK_WAIT    3

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

//=======PIC functions=================================================

void PIC_remap(int offset1, int offset2);
void PIC_sendEOI(unsigned char irq);
void IRQ_set_mask(unsigned char IRQline);
void IRQ_clear_mask(unsigned char IRQline);

//=======Common Inline Assembly functions==============================
static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}
/*
Forces the CPU to wait for an I/O operation to complete. only use this when there's nothing like a status register or an IRQ to tell you the info has been received. 
*/
static inline void io_wait(void)
{
    /* TODO: This is probably fragile. */
    asm volatile ( "jmp 1f\n\t"
                   "1:jmp 2f\n\t"
                   "2:" );
}

//=======APP, user should define this==================================
void initTskBody(void); //init的task body。不是init tskbody这个过程

#endif
