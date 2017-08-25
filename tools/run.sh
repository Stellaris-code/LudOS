set -e
cd ..
qemu-system-i386 -m 256M -kernel build/bin/LudOS.bin 
