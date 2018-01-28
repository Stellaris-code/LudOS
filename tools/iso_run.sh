set -e
./iso.sh
./strip-bin.sh
cd ..
qemu-system-i386 -d mmu,cpu_reset,guest_errors -soundhw pcspk -serial stdio -no-reboot -m 256M -cdrom build/iso/LudOS.iso -drive id=disk,file=build/bin/LudOS.img,if=none -device ahci,id=ahci -device ide-drive,drive=disk,bus=ahci.0 -boot d -enable-kvm
