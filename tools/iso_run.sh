set -e
./iso.sh
cd ..
qemu-system-i386 -d mmu,pcall,cpu_reset,guest_errors -soundhw pcspk -m 256M -cdrom isodir/LudOS.iso
