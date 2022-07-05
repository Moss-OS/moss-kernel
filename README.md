# cribbos
An OS for Raspberry Pi
## Tools
1. [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-a/downloads)
   - ~~brew: `gcc-arm-embedded`~~ This is 32 bit only, 64bit not available on brew
   - Extract this [pre-built cross-compiler toolchain](https://github.com/thinkski/osx-arm-linux-toolchains/releases/download/8.3.0/aarch64-unknown-linux-gnu.tar.xz) into the build folder (should be located at `$PROJECT/build/aarch64-unkown-linux-gnu`)
2. [QEMU](https://www.qemu.org/download/)
3. - ~~brew: `qemu`~~ Version on brew does not support the Raspberry Pi 4
   - Checkout this [patched fork](https://github.com/mcribbs/qemu-patch-raspberry4) and compile.
   - Update QEMU_COMMAND in the Makefile with the location of your built binary
## Resources
- https://jsandler18.github.io/ - This is old and targets 32bit machines
- https://github.com/s-matyukevich/raspberry-pi-os