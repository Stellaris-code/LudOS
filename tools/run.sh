set -e
cd "${BASH_SOURCE%/*}/" 
./mkinitrd.sh
./strip-bin.sh
cd ..
qemu-system-i386 -d mmu,cpu_reset,guest_errors,int -soundhw pcspk -vga std -serial stdio -no-reboot -m 72M -kernel build/bin/LudOS.bin -append "loglevel=debug" -initrd 'build/bin/initrd.tar initrd,build/bin/stripped.bin kernel_binary' -drive id=disk,file=build/bin/LudOS.img,if=none -device ide-hd,drive=disk,bus=ide.0,unit=0 -enable-kvm
#qemu-system-i386 -netdev user,id=n0 -device rtl8139,netdev=n0,mac=DE:AD:BE:EF:12:34 -d mmu,cpu_reset,guest_errors -soundhw pcspk -vga std -serial stdio -no-reboot -m 72M -kernel build/bin/LudOS.bin -append "loglevel=debug" -hdd build/bin/LudOS.img -initrd 'build/bin/initrd.tar initrd,build/bin/stripped.bin kernel_binary' -enable-kvm
#qemu-system-i386 -d mmu,cpu_reset,guest_errors,int -soundhw pcspk -vga std -serial stdio -no-reboot -m 72M -kernel build/bin/LudOS.bin -append "loglevel=debug" -initrd 'build/bin/initrd.tar initrd' -drive id=disk,file=build/bin/LudOS.img,if=none -device ahci,id=ahci -device ide-drive,drive=disk,bus=ahci.0  -enable-kvm
