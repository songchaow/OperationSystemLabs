gcc -c -m32 start.s -o start16_hello.o
ld -T start16_hello.ld start16_hello.o -o start16_hello.elf
objcopy -O binary start16_hello.elf start16_hello.bin
dd if=/dev/zero of=a.img bs=512 count=2880
sudo losetup /dev/loop4 a.img
sudo dd if=start16_hello.bin of=/dev/loop4 bs=512 count=1
sudo losetup -d /dev/loop4