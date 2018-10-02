dd if=/dev/zero of=a_boot2C.img bs=512 count=2880
sudo losetup /dev/loop4 a_boot2C.img
sudo dd if=output/start16.bin of=/dev/loop4 bs=512 count=1
sudo dd if=output/myOS.bin of=/dev/loop4 bs=512 seek=1
sudo losetup -d /dev/loop4
objdump -D output/myOS.elf >output/disassembly