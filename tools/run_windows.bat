cd ..
"C:\Program Files\qemu\qemu-system-i386.exe" -d mmu,cpu_reset,guest_errors -soundhw pcspk -serial stdio -no-reboot -m 16M -kernel build/bin/LudOS.bin -append "loglevel=info" -drive id=disk,file=build/bin/LudOS.img,if=none -device ahci,id=ahci -device ide-drive,drive=disk,bus=ahci.0 -initrd 'build/bin/initrd.img initrd'
cd tools
