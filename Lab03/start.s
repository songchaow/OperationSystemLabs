    .file "start.s"
    
    .text
    .code16 #使GAS生成16位指令,用于实模式

.globl bootsect_start;
bootsect_start:
#将被链接到0x7C00
    cli #关中断

    

    #使%ds %ss置零
    #本初始化符合Intel的Tiny Model 三个段寄存器均为0
    xorw %ax, %ax
    movw %ax, %cs
    movw %ax, %ds
    movw %ax, %ss
    #栈顶指针
    movw $0x2000, %sp

    movw $0xB800, %ax
    movw %ax, %fs
    xorw %di, %di #显存起始offset:0 to %di
    #写入显存
    xorw %bp, %bp # index

clearscreen: #清屏
    movb $0, %fs:(%di)
    addw $2, %di #di += 2
    cmpw $2000, %di
    jl clearscreen

    xorw %di, %di
writetovideo_mem:

    #先写character
    movb display(%bp), %dl #char to $dl
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
    addw $1, %di #显存地址++
    
    #末尾检验
    xorw %dx, %dx
    addb display(%bp), %dl
    jnz writetovideo_mem #未结束就继续

loop:
    jmp loop

    #.data
    display: .string "Hello World, songchaow PB15000102"
    latter: .byte 171#1 010 1011 颜色位 浅绿
    #enable blink; green 
notification_string:	.string "HELLO "
disp:
	movw	$0x0001, %bx
	movb	$0xe, %ah
	int	$0x10		/* display a byte */
message:
	lodsb
	cmpb	$0, %al
	jne	disp	/* if not end of string, jmp to display */ #跳转有问题 跳转到了0x1
	ret
    . = bootsect_start + 510; #shift from the starting point
sign:
    .byte 0x55
    .byte 0xaa
    

#.section	.signature,"",@progbits
    
    #.word 0xaa55
