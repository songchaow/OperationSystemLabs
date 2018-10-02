INCLUDE = -I /media/songchaow/LENOVO/Lenovo/Documents/课程/大二下/OS/Lab/Lab08/myOS/include/ -I /media/songchaow/LENOVO/Lenovo/Documents/课程/大二下/OS/Lab/Lab08/userApp/
ASM_FLAGS= -m32 --pipe -Wall -fasm -O1 -fno-stack-protector
C_FLAGS = -m32 -fno-stack-protector

OWN_SECTIONS = -ffunction-sections -fdata-sections

BOOTSECT=start16
BOOTSECT_OBJS=output/${BOOTSECT}.o
BOOTSECT_BIN=output/${BOOTSECT}.bin

USER_APP_OBJS = output/userApp/main.o output/userApp/init.o

MYOS_OBJS     = output/myOS/start32.o output/myOS/memory.o output/myOS/display.o  output/myOS/task.o output/myOS/PIC.o

OS_OBJS       = ${MYOS_OBJS} ${USER_APP_OBJS}

myOS: ${BOOTSECT_BIN} ${OS_OBJS}
	ld  -v  -T myOS.ld ${OS_OBJS}   -o output/myOS.elf ${INCLUDE}  -static      -L /usr/lib32   -L /usr/lib -L /usr/lib/gcc/x86_64-linux-gnu/6/32/ --start-group -lc -lgcc -lgcc_eh  --end-group --gc-sections --print-gc-sections -S
#--as-needed /usr/lib32/crtn.o /usr/lib32/Mcrt1.o /usr/lib32/Scrt1.o /usr/lib32/crt1.o /usr/lib32/gcrt1.o /usr/lib32/crti.o
	objcopy -O binary output/myOS.elf output/myOS.bin

output/userApp/init.o: userApp/init.c
	gcc -c ${C_FLAGS} userApp/init.c -o output/userApp/init.o ${INCLUDE} ${OWN_SECTIONS}

output/userApp/main.o: userApp/main.c
	gcc -c ${C_FLAGS} userApp/main.c -o output/userApp/main.o ${INCLUDE} ${OWN_SECTIONS}

output/myOS/task.o: myOS/task.c
	gcc -c ${C_FLAGS} myOS/task.c -o output/myOS/task.o ${INCLUDE} ${OWN_SECTIONS}
output/myOS/memory.o: myOS/memory.c
	gcc -c ${C_FLAGS} myOS/memory.c -o output/myOS/memory.o ${INCLUDE} ${OWN_SECTIONS}

output/myOS/display.o: myOS/display.c
	gcc -c ${C_FLAGS} myOS/display.c -o output/myOS/display.o ${INCLUDE} ${OWN_SECTIONS}
#---
output/myOS/PIC.o: myOS/PIC.c
	gcc -c ${C_FLAGS} myOS/PIC.c -o output/myOS/PIC.o ${INCLUDE} ${OWN_SECTIONS}
#---
output/myOS/start32.o: myOS/start32.S
	gcc -c ${ASM_FLAGS} myOS/start32.S -o output/myOS/start32.o ${INCLUDE} ${OWN_SECTIONS}

output/${BOOTSECT}.o: ${BOOTSECT}.S
	gcc -c ${ASM_FLAGS} ${BOOTSECT}.S -o output/${BOOTSECT}.o ${INCLUDE}

output/${BOOTSECT}.bin: ${BOOTSECT_OBJS}
	ld -T ${BOOTSECT}.ld ${BOOTSECT_OBJS} -o output/${BOOTSECT}.elf
	objcopy -O binary output/${BOOTSECT}.elf output/${BOOTSECT}.bin
	
clean:
	rm -f output/*.o output/userApp/*.o output/myOS/*.o output/*.elf output/*.bin
