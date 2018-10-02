#!/bin/sh

IMG_NAME = output/a_boot2C.img

echo "编译链接并制作成二进制映像"
echo "make"
make

echo "制作启动软盘"
echo "dd if=/dev/zero of=" ${IMG_NAME} "bs=512 count=2880"
dd if=/dev/zero of=output/a_boot2C.img bs=512 count=2880

echo "sudo losetup /dev/loop4 output/a_boot2C.img"
sudo losetup /dev/loop4 output/a_boot2C.img

echo "sudo dd if=output/start16.bin of=/dev/loop4 bs=512 count=1"
sudo dd if=output/start16.bin of=/dev/loop4 bs=512 count=1

echo "sudo dd if=output/myOS.bin of=/dev/loop4 bs=512 seek=1"
sudo dd if=output/myOS.bin of=/dev/loop4 bs=512 seek=1

echo "sudo losetup -d /dev/loop4"
sudo losetup -d /dev/loop4
