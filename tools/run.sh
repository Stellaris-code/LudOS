set -e
cd ..
qemu-system-i386 -d mmu,cpu_reset,guest_errors -soundhw pcspk -serial stdio -no-reboot -m 256M -kernel build/bin/LudOS.bin 
