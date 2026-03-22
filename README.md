# The Moss OS kernel

Well currently all of Moss OS.  I think this is going to be a micro-kernel architecture though

See https://github.com/Moss-OS

# Dependencies
make
gcc-aarch64-linux-gnu 
binutils-aarch64-linux-gnu
qemu

# Known Issues
- The linker currently emits a warning that `build/kernel8.elf` has a LOAD segment with RWX permissions.
- This does not block the current bare-metal `kernel8.img` boot flow, but the linker script should be revisited later to split ELF load segments into separate RX and RW regions.
