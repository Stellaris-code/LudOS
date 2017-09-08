set -e
cd ..
qemu-system-i386 -d mmu,cpu_reset,guest_errors -no-reboot -soundhw pcspk -m 256M -kernel build/bin/LudOS.bin 
