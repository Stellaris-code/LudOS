set -e
cd ..
#qemu-system-i386 -d mmu,cpu_reset,guest_errors -soundhw pcspk -vga std -serial stdio -no-reboot -m 256M -kernel build/bin/LudOS.bin -hdd build/bin/LudOS.img -initrd 'build/bin/initrd.tar initrd' -enable-kvm
qemu-system-i386 -d cpu_reset,guest_errors -soundhw pcspk -vga std -serial stdio -no-reboot -m 32M -kernel build/bin/LudOS.bin -append "loglevel=debug" -initrd 'build/bin/initrd.tar initrd' -enable-kvm -drive id=disk,file=build/bin/LudOS.img,if=none -device ahci,id=ahci -device ide-drive,drive=disk,bus=ahci.0 
