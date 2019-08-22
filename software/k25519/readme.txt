/*
 * Author: Joost Renes
 * Version: 2019-08-14
 *
 * This software is run on Ubuntu 18.04.3 LTS (bionic) with
 *      gcc version 7.4.0
 *      arm-none-eabi-gcc version 6.3.1
 *      GNU ar version 2.27
 *
 * Most code is in the public domain, based on the munacl project
 * (http://munacl.cryptojedi.org)
 * 
 * WARNING: The arm/stm32f0xx directory is *not* in the public domain
 * (software from STMicroelectronics)
 * 
 * Several assembler files are taken from other projects, which are
 * referenced in the files themselves
 *
 */

# C compilation

## m25519

make cref_m25519
sudo test/cref/test_cref_m25519

## kl25519

make cref_kl25519
sudo test/cref/test_cref_kl25519

# ARM compilation

First install the toolchain
(from http://munacl.cryptojedi.org/curve25519-cortexm0.shtml)

apt-get install llvm clang gcc-arm-none-eabi
apt-get install libc6-dev-i386 
apt-get install libusb-1.0-0-dev

git clone https://github.com/texane/stlink.git
cd stlink
./autogen.sh
./configure
make && make install 

3.3V → 3.3V
TXD → PA3
RXD → PA2
GND → GND
5V → DO NOT CONNECT

## m25519

make arm_m25519
sudo test/arm/m25519/test_stm32f0.sh
sudo test/arm/m25519/stack_stm32f0.sh
sudo test/arm/m25519/speed_stm32f0.sh

## kl25519

make arm_kl25519
sudo test/arm/kl25519/test_stm32f0.sh
sudo test/arm/kl25519/stack_stm32f0.sh
sudo test/arm/kl25519/speed_stm32f0.sh
