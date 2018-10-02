	.file	"main.c"
	.globl	prompt
	.data
	.align 32
	.type	prompt, @object
	.size	prompt, 100
prompt:
	.string	"Hello World from main function!\n"
	.zero	67
	.comm	color,1,1
	.comm	video_mem,4,4
	.comm	test,1,1
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$16, %esp
	.cfi_offset 3, -12
	call	__x86.get_pc_thunk.ax
	addl	$_GLOBAL_OFFSET_TABLE_, %eax
	movl	color@GOT(%eax), %edx
	movb	$11, (%edx)
	movl	video_mem@GOT(%eax), %edx
	movl	$753664, (%edx)
	leal	prompt@GOTOFF(%eax), %edx
	movl	%edx, -8(%ebp)
	jmp	.L2
.L3:
	movl	video_mem@GOT(%eax), %edx
	movl	(%edx), %edx
	leal	1(%edx), %ebx
	movl	video_mem@GOT(%eax), %ecx
	movl	%ebx, (%ecx)
	movl	-8(%ebp), %ecx
	movzbl	(%ecx), %ecx
	movb	%cl, (%edx)
	movl	video_mem@GOT(%eax), %edx
	movl	(%edx), %edx
	leal	1(%edx), %ebx
	movl	video_mem@GOT(%eax), %ecx
	movl	%ebx, (%ecx)
	movl	color@GOT(%eax), %ecx
	movzbl	(%ecx), %ecx
	movb	%cl, (%edx)
	addl	$1, -8(%ebp)
.L2:
	movl	-8(%ebp), %edx
	movzbl	(%edx), %edx
	testb	%dl, %dl
	jne	.L3
.L4:
	jmp	.L4
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.section	.text.__x86.get_pc_thunk.ax,"axG",@progbits,__x86.get_pc_thunk.ax,comdat
	.globl	__x86.get_pc_thunk.ax
	.hidden	__x86.get_pc_thunk.ax
	.type	__x86.get_pc_thunk.ax, @function
__x86.get_pc_thunk.ax:
.LFB1:
	.cfi_startproc
	movl	(%esp), %eax
	ret
	.cfi_endproc
.LFE1:
	.ident	"GCC: (Ubuntu 6.2.0-5ubuntu12) 6.2.0 20161005"
	.section	.note.GNU-stack,"",@progbits
