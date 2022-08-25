# RaspberryPi4B_Drivers
These are some drivers for hardware control on the Raspberry Pi 4B peripherals.
I made them on Raspberry Pi OS (Raspbian) arm64. I haven't tested it on Raspberry Pi OS armhf or any other OS.
I believe these will compile and run on Raspberry Pi OS armhf, since it's the same hardware and GNU-Linux distro, (Raspberry Pi OS, BCM2711).
I don't know if they will compile on other GNU-Linux distros, as their kernel headers might be incompatible to these code files.

Before anything, make sure you have the kernel headers.
On Raspberry Pi OS, you can get those using "sudo apt-get install raspberrypi-kernel-headers".
Unfortunatelly though, not all versions of Raspberry Pi OS has the build kernel header files.
Once kernel header files are installed, check if there is a folder named "build" in directory "/lib/modules/$(uname -r)/"

Files:

"Makefile", "_RegisterMapping.h" and "_CtrlMod.c" files are the driver (module) files themselves. 
Unless you want to understand in depth how these drivers work, you don't need to understand these files.
You'll just need to compile them using kernel headers to generate the kernel object file ".ko" and then install the .ko file.
To install a .ko file, use "sudo insmod <file.ko>"
To uninstall a .ko file, use "sudo rmmod <file.ko>"
To list installed modules, use "lsmod"

"_Ctrl.c" and "_Ctrl.h" are user files to control the driver (module).
These files are used by the user application to control the driver from userspace.

I'm not a professional software developer. Everything here was made just for fun. Don't expect professional performance from these codes.

Author: Rafael Sabe
Email: rafaelmsabe@gmail.com
