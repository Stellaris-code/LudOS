set -e
cd ..
#qemu-system-i386 -d mmu,cpu_reset,guest_errors -soundhw pcspk -vga std -serial stdio -no-reboot -m 256M -kernel build/bin/LudOS.bin -hdd build/bin/LudOS.img
qemu-system-i386 -d mmu,cpu_reset,guest_errors -soundhw pcspk -vga std -serial stdio -no-reboot -m 16M -kernel build/bin/LudOS.bin -append "loglevel=info" -drive id=disk,file=build/bin/LudOS.img,if=none -device ahci,id=ahci -device ide-drive,drive=disk,bus=ahci.0 -initrd 'build/bin/initrd.tar initrd'
