#!/bin/sh
set -e

./mkinitrd.sh
./strip-bin.sh

cd ..
 
rm -rf isodir/boot/*
 
mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

rm -f isodir/LudOS.iso
 
cp build/bin/LudOS.bin isodir/boot/LudOS.bin
cp build/bin/stripped.bin isodir/boot/stripped.bin
cp build/bin/initrd.tar isodir/boot/initrd.tar

cat > isodir/boot/grub/grub.cfg << EOF
menuentry "LudOS" {
	multiboot /boot/LudOS.bin loglevel=debug
	module    /boot/initrd.tar initrd
	module    /boot/stripped.bin kernel_binary
} 
EOF
grub-mkrescue -o build/iso/LudOS.iso isodir
