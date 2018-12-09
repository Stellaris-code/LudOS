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
 
cp build/bin/stripped.bin isodir/boot/LudOS.bin
cp build/bin/initrd.tar isodir/boot/initrd.tar

cat > isodir/boot/grub/grub.cfg << EOF
menuentry "LudOS" {
	multiboot /boot/LudOS.bin loglevel=debug
	module    /boot/initrd.tar initrd
} 
EOF
LC_ALL=en_US.UTF-8 grub-mkrescue  -o build/iso/LudOS.iso isodir --install-modules="part_msdos normal multiboot halt ls" --themes= --fonts= 
