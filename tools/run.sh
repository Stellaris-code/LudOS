set -e
cd ..
qemu-system-i386 -d int,mmu,pcall,cpu_reset,guest_errors -m 256M -kernel build/bin/LudOS.bin 
