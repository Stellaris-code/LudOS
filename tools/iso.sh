#!/bin/sh
set -e

./mkinitrd.sh

cd ..
 
mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub

rm -f isodir/LudOS.iso
 
cp build/bin/LudOS.bin isodir/boot/LudOS.kernel
cp build/bin/initrd.tar isodir/boot/initrd.tar

cat > isodir/boot/grub/grub.cfg << EOF
insmod fat
insmod iso9660
menuentry "LudOS" {
	multiboot /boot/LudOS.kernel loglevel=info
	module    /boot/initrd.tar initrd
} 
EOF
grub-mkrescue -o build/iso/LudOS.iso isodir
