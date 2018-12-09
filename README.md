
# LudOS
LudOS is a hobby linux-like kernel written for fun in C++.

## Features
- Currently supports the 32-bit x86 architecture
- An extensible virtual file system with the ability to expose function call interfaces on nodes to provide a cleaner interface than ioctl
- PS/2 Keyboard and Mouse support
- Preemptive and Cooperative Multitasking
- Supports resolutions up to 1920x1080 using VBE framebuffer
- Uses MTRRs to provide better performance with the framebuffer
- Support for SATA drives via AHCI and ATA
- ext2 filesystem and tar files mounting on the VFS
- A lighting-fast graphical framebuffer terminal and shell

## Goals
- Support for the 64-bit x86 architecture and ARM
- TCP/IP driver
- A full-fledged graphical window manager
- Dynamic linker support
- Porting programs like GCC, NASM or FreeDoom to LudOS

## Dependancies
`QEMU` is needed to run the kernel using `tools/run.sh`.
On Ubuntu and derivatives : `sudo apt-get install qemu-system`

## How to build :
Built binaries are already included in the build/bin directory, but if you want to build yourself the repository do as follow :

- First build your own cross-compiler following these instructions : https://wiki.osdev.org/GCC_Cross-Compiler
- Build to project by running `CXX=<cross-g++-path> CC=<cross-gcc-path> cmake CMakeLists.txt` and then `make`.

## How to run :
Run the script `tools/run.sh` which will run QEMU on the kernel binary file.

## Screenshots :


![Imgur](https://i.imgur.com/QgkMo0l.png)
![Imgur](https://i.imgur.com/bhkklbx.png)
![Imgur](https://i.imgur.com/jxmEKjQ.png)
![Imgur](https://cdn.discordapp.com/attachments/398232429246152704/405785414264422410/Capture_du_2018-01-24_19-03-05.png)
![Imgur](https://i.imgur.com/7AWLds2.png)
