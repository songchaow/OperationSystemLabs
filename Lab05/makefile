a_boot2C.img: myOS.bin start16.bin
	dd if=/dev/zero of=a_boot2C.img bs=512 count=2880
	sudo losetup /dev/loop4 a_boot2C.img
	sudo dd if=start16.bin of=/dev/loop4 bs=512 count=1
	sudo dd if=myOS.bin of=/dev/loop4 bs=512 seek=1
	sudo losetup -d /dev/loop4
myOS.bin: myOS.elf
	objcopy -O binary myOS.elf myOS.bin
start16.bin: start16.elf
	objcopy -O binary start16.elf start16.bin
start16.elf: start16.ld start16.o
	ld -T start16.ld start16.o -o start16.elf
myOS.elf: myOS.ld start32.o main.o
	ld -T myOS.ld start32.o main.o -o myOS.elf
start16.o: start16.s
	gcc -c  --pipe -Wall -fasm -g -O1  start16.s -o start16.o -m32
start32.o: start32.s
	gcc -c  --pipe -Wall -fasm -g -O1  start32.s -o start32.o -m32
main.o: main.c
	gcc -c main.c -o main.o -m32
#a_boot2C.img: myOS.bin start16.bin
