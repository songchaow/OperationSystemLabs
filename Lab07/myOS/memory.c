//manually probing memory is dicouraged.
#include <myOS.h>
void showln(char* msg, char color, int row);//显示一行消息
int DPoolHead;
#define ALIGNUNIT 4
#define ENTRYNUM 20

#define STATUS_FREE 0
#define STATUS_UNUSED -1
#define STATUS_OCCUPIED 1
typedef struct DFreePartEntry
{
    char status;//0:free -1:unused 1:occupied
    long addr_start;
    long addr_end;
    int next;//stores the index
}DEntry;
DEntry DEntryPool[ENTRYNUM];
unsigned long count_memory()
{
    long *mem,mem_count,a,mem_kb;
    /*
    a: temporarily store the original content
    mem: pointer to the mem location being probbed
    */
    mem_kb=0;
    mem_count=1024*1024;//mem address in Byte
    do
    {
        //每1KB取一字节的样本
        mem_kb++;
        mem_count +=1024;
        mem =(long*) mem_count;

        a = *mem;
        *mem = 0x55AA55AA;
        asm("":::"memory");//在写与读之间加上memory barrier 防止被优化
        if(*mem!=0x55AA55AA) mem_count=0;
        else{
            *mem = 0xAA55AA55;
            asm("":::"memory");
            if(*mem!=0xAA55AA55)
                mem_count = 0;
        }
        asm("":::"memory");
        *mem = a;
        println(WHITE,"checking memory addr:%d",mem_count);
    }while((mem_kb<MEMORYLIMIT)&&mem_count!=0);
    println(WHITE,"Finish cheking memory!\n");
    println(WHITE,"    PMemStart: 1M");
    println(WHITE,"    PMemSize: 0x%x",mem_count);
    return mem_count;
}

unsigned long dPartitionInit(unsigned long start, unsigned long size)
{
    //初始化DEntryPool
    DEntryPool[0].status=STATUS_FREE;
    DEntryPool[0].addr_start=start;
    DEntryPool[0].addr_end=start+size;
    DEntryPool[0].next=-1;
    int i;
    for(i=1;i<=ENTRYNUM-1;i++)
        DEntryPool[i].status=STATUS_UNUSED;
    return start;
}
void pMemInit()
{
    unsigned long size=count_memory();
    dPartitionInit(1024*1024,size);
}
int findUnusedEntry()
{
    int i;
    for(i=0;i<=ENTRYNUM-1;i++)
        if(DEntryPool[i].status==STATUS_UNUSED) //bug1 fixed at 1:45
            return i;
    return -1;
}
unsigned long dPartitionAlloc(unsigned long size)  //0失败；其他：所分配到的内存块起始地址
{
    //ALIGNUNIT Byte 对齐
    unsigned long size_align=(size/ALIGNUNIT+1)*ALIGNUNIT;
    int p,p0;
    int newentry;
    for(p=0;p<=ENTRYNUM-1;)
    {
        if((DEntryPool[p].status==STATUS_FREE)&&(DEntryPool[p].addr_end-DEntryPool[p].addr_start>=size_align))
        {
            DEntryPool[p].status=STATUS_OCCUPIED;//bug1 fixed at 13:37
            if(DEntryPool[p].addr_end-DEntryPool[p].addr_start>=2*size_align)//若剩余空间大于1个单元，则分成两块
            {
                //add new entry to pool
                newentry=findUnusedEntry();
                if(newentry==-1) println(RED,"Failed to create new memory entry. Pool is full.");
                //new entry is the free entry
                DEntryPool[newentry].status=STATUS_FREE;
                DEntryPool[newentry].addr_start=DEntryPool[p].addr_start+size_align;
                DEntryPool[newentry].addr_end=DEntryPool[p].addr_end;
                DEntryPool[p].addr_end=DEntryPool[newentry].addr_start;
            }
            return(DEntryPool[p].addr_start);
        }
        else
        {
            p=(p+1)%ENTRYNUM;//bug2 fixed at 16:38
            if(p==0)//循环回来了
                return 0;
        }
    }
}
unsigned long dPartitionFree(unsigned long start) //0失败；1成功
{
    //find the occupied block:
    int i,j,k;
    for(i=0;i<=ENTRYNUM-1;i++)
    {
        if(DEntryPool[i].addr_start==start)
        {
            if(DEntryPool[i].status==STATUS_OCCUPIED)
            {
                //查找其上是否有空
                for(j=0;j<=ENTRYNUM;j++)
                {
                    if((DEntryPool[j].status==STATUS_FREE)&&(DEntryPool[j].addr_end==start))
                    {
                        //将i项对应块的空闲空间合并记录到j项里
                        DEntryPool[j].addr_end=DEntryPool[i].addr_end;
                        DEntryPool[i].status=STATUS_UNUSED;
                        break;
                    }
                }
                if(DEntryPool[i].status==STATUS_UNUSED)//i上有空'j'，已合并至j，继续检查j下面是否有空
                    i=j;//操作对象换为j
                //查找其下是否有空
                for(k=0;k<=ENTRYNUM;k++)
                {
                    if((DEntryPool[k].status==STATUS_FREE)&&(DEntryPool[k].addr_start==DEntryPool[i].addr_end))
                    {
                        //将k项对应块的空闲空间合并记录到i项里
                        DEntryPool[i].status=STATUS_FREE;
                        DEntryPool[i].addr_end=DEntryPool[k].addr_end;
                        DEntryPool[k].status=STATUS_UNUSED;
                        return 1;
                    }
                }
                //上有空、下面没空，或者是独立的一块区域
                DEntryPool[i].status=STATUS_FREE;
                return 1;
            }
            else return 0;
        }
    }
    return 0;
}

unsigned long eFPartitionTotalSize(unsigned long perSize, unsigned long n) //根据单位大小和单位个数，计算出eFPartition所需内存大小
{
    //TotalSize = Manage Cost(n) + Data Area Size(n,perSize)
    unsigned long data_size=perSize*n;
    unsigned long manage_size=((n/8)+1)*8;//one byte for 8 units
    return(data_size+manage_size);
}
unsigned long eFPartitionInit(unsigned long start, unsigned long perSize, unsigned long n) //初始化并返回eFPartition句柄
{
    /*Memory Layout:
    Manage:
        int perSize         Byte: 0-3
        int n               Byte: 4-7
        char[manage_size]   Byte: 8...
        //最后一个字节会有多余
    Data:
        char[n][perSize]
    */
    int manage_size=((n/8)+1)*8;
    int i;
    int* pi=(int*)start;
    *pi++=perSize;
    *pi=n;
    for(i=8;i<=manage_size-1+8;i++)
        *(char*)(start+i)=0;
    return start;
}
unsigned long eFPartitionAlloc(unsigned long EFPHandler) //0失败；其他：所分配到的内存块起始地址
{
    //姑且把Hanlder理解为静态分区的首地址
    int perSize=*(int*)EFPHandler;
    int n=*(int*)(EFPHandler+4);
    int manage_size=((n/8)+1)*8;
    char* p=(char*)(EFPHandler+8);//pointer to management area
    unsigned long location=0;
    //从左到右查找空闲块：
    int i;
    for(i=0;i<=manage_size-1;i++)
    {
        if(p[i]!=0b11111111)//all occupied
        {
                 if(!(p[i]&0b00000001))
            {p[i]+=1;location=((i*8+0)*perSize+EFPHandler+8+manage_size);break;}
            else if(!(p[i]&0b00000010))
            {p[i]+=2;location=((i*8+1)*perSize+EFPHandler+8+manage_size);break;}
            else if(!(p[i]&0b00000100))
            {p[i]+=4;location=((i*8+2)*perSize+EFPHandler+8+manage_size);break;}
            else if(!(p[i]&0b00001000))
            {p[i]+=8;location=((i*8+3)*perSize+EFPHandler+8+manage_size);break;}
            else if(!(p[i]&0b00010000))
            {p[i]+=16;location=((i*8+4)*perSize+EFPHandler+8+manage_size);break;}
            else if(!(p[i]&0b00100000))
            {p[i]+=32;location=((i*8+5)*perSize+EFPHandler+8+manage_size);break;}
            else if(!(p[i]&0b01000000))
            {p[i]+=64;location=((i*8+6)*perSize+EFPHandler+8+manage_size);break;}
            else if(!(p[i]&0b10000000))
            {p[i]+=128;location=((i*8+7)*perSize+EFPHandler+8+manage_size);break;}
        }
    }
    if(location>=EFPHandler+8+manage_size+n*perSize) return 0; 
    return location;
}
int eFPartitionFree(unsigned long EFPHandler,unsigned long mbStart) //0失败；1成功
{
    //姑且把Hanlder理解为静态分区的首地址
    int perSize=*(int*)EFPHandler;
    int n=*(int*)(EFPHandler+4);
    int manage_size=((n/8)+1)*8;
    char* p=(char*)(EFPHandler+8);//pointer to management area
    //locate:
    int count=(mbStart-(EFPHandler+8+manage_size))/perSize; //minus 1
    if(count>=n) return 0;
    int manage_count=count/8;
    int rest=count-manage_count*8;
    p[manage_count] ^= (1<<rest);
    return 1;
}