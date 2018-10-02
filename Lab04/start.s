#define videomem_addresss 0xB800



    .file "start.s"
    
    .text
    .code16 #使GAS生成16位指令,用于实模式

#GDT entry format:
.macro descriptor base, limit, attribute
    .word (\limit) & 0x0FFFF #the lower 16 bits of limit
    .word (\base) & 0x0FFFF #the lower 16 bits of base Part I
    .byte (\base >> 16) & 0x0FF # the higher 16 bits of base Part II
    .word (\limit >> 8) & 0x0F00|((\attribute & 0x0F0FF))
    .byte (\base >> 24) & 0x0FF # the highest 16 bits of base Part III
.endm

.globl bootsect_start;
bootsect_start:
    jmp real_start

    
#contents of GDT:
    gdt_start:
    descriptor 0, 0, 0
    gdt_32bitcode:
    descriptor 0, 0xfffff, 0b1100000010011010 #可执行可读代码段属性值 512 is surely enough
    #1100: 1: 4KB granularity 1:32 bit protect mode
    #0000: limit bit to be cut out
    #1001: 1:present bit 00:kernel level 
    #1110: 
    #executable bit; not conforming bit(whether can be executed from lower levels)
    #Readable; not accessed.
    gdt_videomem:
    descriptor 0xB8000, 0xffff, 0b1100000010010010 #attr_data
    gdt_data:
    descriptor 0, 0xfffff, 0b1100000010010010 #可读写数据段属性值


    real_start:
    cli #关中断
    #使%ds %ss置零
    #本初始化符合Intel的Tiny Model 三个段寄存器均为0
    xorw %ax, %ax
    movw %ax, %ds
    movw %ax, %ss
    #栈顶指针
    movw $0x2000, %sp
    movw $0xB800, %ax
    movw %ax, %fs
    xorw %di, %di #显存起始offset:0 to %di
    #写入显存
    xorw %bp, %bp # index

clearscreen:
    #movw $2000, %ax #number of characters
    movb $0, %fs:(%di)
    addw $2, %di #di += 2
    cmpw $2000, %di
    jl clearscreen

    xorw %di, %di
writetovideo_mem0:

    #先写character
    movb display(%bp), %dl #char to $dl
    #displayed as: mov    0x15887c3a(%esi),%dl
    movb %dl, %fs:(%di) #以字节为单位
    addw $1, %di #显存地址++
    addw $1, %bp #char[index] index自增

    #写attribute:
    #dl: temp reg for storing attribute char
    movb latter, %dl
                #leaq latter, %si
                #movb 0(%si), %dl
    #movb (latter), %dl #wrong usage!
    movb %dl, %fs:(%di)
    #displayed as: 0x7c1f  mov    %dl,0x8a01c783 ?
    addw $1, %di #显存地址++
    
    #末尾检验
    xorw %dx, %dx
    addb display(%bp), %dl
    jnz writetovideo_mem0 #未结束就继续
    xorw %bp, %bp
#load to segement register: %fs:


    leaw promptstr, %si #load the address of prompt string
    call message


readkey:
    movb $0, %ah # function code: read a character
    int $0x16 # read a key

preparation:
    /*xorl %eax,%eax
    movw %cs, %ax
    shll $4, %eax
    addl $gdt_start, %eax
    movl %eax, (gdtr_content+2)*/
    lgdt gdtr_content
#switch to PM
    movl %cr0, %eax
    orb $1, %al
    movl %eax, %cr0

protect_mode:
    xorw %ax,%ax
    
    movw $0x10, %ax
    movw %ax, %fs

    movw $0x18, %ax
    movw %ax, %ds
    #xorw %di, %di #no need to clear
    #output prompt 1
    movl $(2*(12*80)),%edi
    movw $okdisp, %si
    call writetovideo_mem
    #a new line and output prompt 2
    movl $(2*(13*80)),%edi
    movw $succ, %si
    call writetovideo_mem
    jmp loop

writetovideo_mem:
    #先写character
    movb (%si), %dl #char to $dl
    movb %dl, %fs:(%edi) #以字节为单位
    addl $1, %edi #显存地址++
    addw $1, %si #char[index] index自增

    #写attribute:
    movb latter, %dl
    #movb (latter), %dl #wrong usage!
    movb %dl, %fs:(%edi)
    #displayed as: 0x7c1f  mov    %dl,0x8a01c783 ?
    addl $1, %edi #显存地址++
    
    #末尾检验
    xorw %dx, %dx
    addb (%si), %dl
    jnz writetovideo_mem #未结束就继续
    ret


loop:
    jmp loop
disp:
	movw	$0x0001, %bx
	movb	$0xe, %ah # This means writing characters in TTY(Teletype output) mode
	int	$0x10		/* display a byte */
message: #BIOS text output
    lodsb
    cmpb $0, %al
    jne disp
    ret
    .data

gdtr_content:
    .word 191 #the size of GDT 8*8*3-1
    .word (gdt_start-bootsect_start+0x7C00)&0xFFFF #the lower 16 bits
    .word ((gdt_start-bootsect_start+0x7C00)>>16)&0xFFFF #the higher 16 bits, usually zero

promptstr: .string "Press any key to continue haha!\n"
promptstr2: .string "Start entering protect mode! hahahahahaha...geh!"
display: .string "Hello World, songchaow PB15000102!"
okdisp: .string "Switching to protect mode...O.K.!!"
succ: .string "Now we are in protect mode!"
latter: .byte 171#1 010 1011 颜色位
    #enable blink; green 



.section	".signature","a" #"a" means allocatable. therefore signature section will be in bin file.
    .globl sign
sign:
    .byte 0x55
    .byte 0xaa
