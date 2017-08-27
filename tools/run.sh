set -e
cd ..
qemu-system-i386 -d mmu,pcall,cpu_reset,guest_errors -soundhw pcspk -m 256M -kernel build/bin/LudOS.bin 
