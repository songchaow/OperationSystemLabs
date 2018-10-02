    .file "start32.s"
    .text
    .code32
    .globl main;
    .globl _start;
_start:
    #initialize
    xorw %ax,%ax
    
    movw $0x10, %ax
    movw %ax, %fs

    movw $0x18, %ax
    movw %ax, %ds
    movw %ax, %ss #stack segement = data segment = 0x0 PC goes away!
    #栈顶指针
    movw $0x2000, %sp
    #清空 bss
    leaw __bss_start, %edi
    leaw _end       , %esi
clear:
    movw $0,    (%edi)
    addw $4,    %edi
    cmpw %edi       , %esi
    jne clear
    jmp main





