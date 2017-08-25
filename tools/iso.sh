#!/bin/sh
set -e

cd ..
 
mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub
 
cp build/bin/LudOS.bin isodir/boot/LudOS.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "LudOS" {
	multiboot /boot/LudOS.kernel
}
EOF
grub-mkrescue -o isodir/LudOS.iso isodir &> /dev/null
