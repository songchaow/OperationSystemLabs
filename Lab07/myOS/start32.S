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
CTX_SW:
    pusha
    movl %esp, prevTSK_StackPtr
    movl nextTSK_StackPtr, %esp
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
