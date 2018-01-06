set -e
./mkinitrd.sh
./strip-bin.sh
cd ..
qemu-system-i386 -d mmu,cpu_reset,guest_errors -soundhw pcspk -vga std -serial stdio -no-reboot -m 72M -kernel build/bin/LudOS.bin -hdd build/bin/LudOS.img -initrd 'build/bin/initrd.tar initrd,build/bin/stripped.bin kernel_binary' -enable-kvm
#qemu-system-i386 -d mmu,cpu_reset,guest_errors,int -soundhw pcspk -vga std -serial stdio -no-reboot -m 72M -kernel build/bin/LudOS.bin -append "loglevel=debug" -initrd 'build/bin/initrd.tar initrd' -drive id=disk,file=build/bin/LudOS.img,if=none -device ahci,id=ahci -device ide-drive,drive=disk,bus=ahci.0  -enable-kvm
