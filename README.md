## CH32V003 Examples
本サンプルは、以下のオープンソース開発環境を用いて開発されています。
https://github.com/cnlohr/ch32v003fun

本サンプルは、ch32v003fun/examplesのサンプルをベースとしていて、
CH32V003に関する開発理解を深めるために、コメントを追加したり、
手元の回路や目的に合わせて改変したものです。

また、マイコンへの書き込みにはWCH-LinkEやそのクローンが必要になります。

### Setup development environment

自身は、Ubuntu22.04環境で開発しています。
準備としては、本リポジトリをクローンして、
直下にch32v003funをクローンするだけです。

```bash
> git clone git@github.com:chihiro1234567/ch32v003_examples.git
> cd ch32v003_examples
> git clone https://github.com/cnlohr/ch32v003fun.git
```

### How to build & flash

ch32v003-blinkを参考にソースのビルドをフラッシュの方法になります。

1. WCH-LinkEをPCに接続する。（RISC-Vモードに変更しておく。
2. WCH-LinkEの3V3、GND、SWDIOをマイコンのそれと接続する。
3. makeコマンドを実行する

```bash
> cd ch32v003_examples/ch32v003-blink
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
