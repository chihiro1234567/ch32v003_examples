## CH32V003 Examples

### setup development

```bash
> cd ch32v003_examples
> git clone https://github.com/cnlohr/ch32v003fun.git
```

### how to build & flash

```bash
> cd ch32v003-blink
> make
riscv64-unknown-elf-gcc -o blink.elf ../ch32v003fun/ch32v003fun/ch32v003fun.c blink.c -g -Os -flto -ffunction-sections -static-libgcc -march=rv32ec -mabi=ilp32e -I/usr/include/newlib -I../ch32v003fun/ch32v003fun/../extralibs -I../ch32v003fun/ch32v003fun -nostdlib -I. -Wall  -T ../ch32v003fun/ch32v003fun/ch32v003fun.ld -Wl,--gc-sections -L../ch32v003fun/ch32v003fun/../misc -lgcc
riscv64-unknown-elf-size blink.elf
:
../ch32v003fun/minichlink/minichlink -w blink.bin flash -b
Found WCH Link
WCH Programmer is LinkE version 2.10
Chip Type: 003
Setup success
Flash Storage: 16 kB
Part UUID    : ab-d0-ab-cd-13-f8-bc-5b
PFlags       : ff-ff-ff-ff
Part Type (B): 07-13-bb-91
Read protection: disabled
Interface Setup
Image written.
```
