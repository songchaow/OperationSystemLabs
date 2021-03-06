STACK_SIZE =	0x1000	# Size of heap and stack. 4KB

WHITE	= 0x7			# 白色，用于输出字符，为字符颜色

VGA_START	= 0xb8000

/* ===== code32 =========================== */

.text
	.globl 	_start		# GNU default entry point
	.globl 	myMain
.code32
_start:
its_ok:
	movl	$ok_message, %esi
	movb	$WHITE, %ah
#	movl	$0, %edx 	# %edx is set above
	call	pm_print_string

	jmp fun1

dead:	jmp dead			# Never here

pm_print_string:
	movl	$VGA_START, %ebx	
pm_print_string_loop:	
	lodsb
	andb	%al, %al
	jz	pm_print_string_end
	movw	%ax, %es:0(%ebx,%edx,2)
	incl	%edx
	jmp	pm_print_string_loop
pm_print_string_end:
	ret

fun1:
	movl	$_end, %eax		
	addl	$STACK_SIZE, %eax	
	andl	$0xffffffc0, %eax	#align the stack 
	
	movl	%eax, %esp		
	movl	%eax, %ebp		

fun2:
	cld				            
	movl	$_end, %ecx		   
	movl	$_bss_start, %edi	
	subl	%edi, %ecx		    
	shrl	%ecx			   
	shrl	%ecx
	
	xorl	%eax, %eax		    
	repne			            
	stosl				           
loadidt:
	lidt	idtr
# Transfer control to main
to_main:
	movl	$to_main_message, %esi 
	movb	$WHITE, %ah
	movl	$(11*80), %edx 	
	call	pm_print_string
	call	main

shut_down:	
	jmp	shut_down	# Never here

.global prevTSK_StackPtr
.global nextTSK_StackPtr
prevTSK_StackPtr:	# will change later
	.long 0x4000
nextTSK_StackPtr:	# will change later
	.long 0x5000

.global CTX_SW
.global Save_CTX
.global Rest_CTX
.global init_PIT
.global StartTsk
CTX_SW:
    pusha
    movl %esp, prevTSK_StackPtr
	call schedule
	#call C function, convenient to store the stack pointer and determine the next task
    movl nextTSK_StackPtr, %esp
    popa
    iret
Save_CTX: #interrupt service routine entry
	pusha
	movl %esp, prevTSK_StackPtr
	call tick
	call schedule #will not return!
	iret
Rest_CTX:
	movl nextTSK_StackPtr, %esp
	popa
	#int		$32 #manual test	
	sti		#can be deleted
	iret	#really important when we manually create the stack's data


init_PIT:
	pusha
	cli	#just in case
	movb $0b00110100, %al
	outb %al, $0x43
	movw $11932, %ax #about 100Hz (or 200Hz?)
	outb %al, $0x40
	movb %ah, %al
	outb %al, $0x40
	popa
	ret
/* ===== data =========================== */
.data
		.globl _stack_size
_stack_size:	.long STACK_SIZE

ok_message:
	.ascii	"O.K.!"
	.byte	0

to_main_message:
	.ascii	"Call myMain..."
	.byte	0
idtr:
	.word (64*8)-1 #size in bytes
	.long idt_table
.p2align 4
idt_table:
	#the first 32 entries
	.rept 32
	.word 0x7e84#0x0000ffff&(Save_CTX-_start+0x7e00)	#offset1
	.word 0x08					#selector
	.byte 0
	.byte 0x8E					#type attribute
	.word 0		#offset2
	.endr

	# the 33rd entry clk interrupt
	.rept 32
	.word 0x7e84#0x0000ffff&(Save_CTX-_start+0x7e00)	#offset1
	.word 0x08					#selector
	.byte 0
	.byte 0x8E					#type attribute
	.word 0			#offset2
	.endr