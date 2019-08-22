cref_CC = gcc
cref_FLAGS = -g -Wall -Wextra -Werror

arm_CC = arm-none-eabi-gcc
arm_CFLAGS = -Wall -Wextra -Werror -fshort-enums -O3 -DCORTEX_M0 -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=cortex-m0 -fno-builtin  -ffreestanding -mfloat-abi=soft -nostdlib
arm_ASFLAGS = -mcpu=cortex-m0 -mfloat-abi=soft -nostdlib
arm_INCDIRS = -I./arm/stm32f0xx
arm_AR = arm-none-eabi-ar

STMOBJ = arm/stm32f0xx/system_stm32f0xx.o  \
         arm/stm32f0xx/stm32f0xx_gpio.o  \
         arm/stm32f0xx/stm32f0xx_rcc.o  \
         arm/stm32f0xx/stm32f0xx_usart.o  \
         arm/stm32f0xx/startup_stm32f0xx.o \
				 arm/stm32f0xx/sbrk.o

LINKERFILE = arm/stm32f0xx/stm32f0_linker.ld

all: cref_m25519 cref_kl25519 arm_m25519 arm_kl25519

cref_m25519: test/cref/test_cref_m25519
cref_kl25519: test/cref/test_cref_kl25519
arm_m25519: test/arm/m25519/speed.bin test/arm/m25519/test.bin test/arm/m25519/stack.bin
arm_kl25519: test/arm/kl25519/speed.bin test/arm/kl25519/test.bin test/arm/kl25519/stack.bin

cref = obj/cref/fe25519.o \
			  obj/cref/hash.o \
			  obj/cref/scalar.o

arm = obj/arm/mul.o \
	  obj/arm/cortex_m0_reduce25519.o \
	  obj/arm/fe25519_mulconst_asm.o \
	  obj/arm/sqr.o \
	  obj/arm/keccak.o \
      obj/arm/hash.o \
      obj/arm/fe25519.o \
      obj/arm/scalar.o

cref_m25519 = obj/cref/m25519/ladder.o \
		 obj/cref/m25519/dh.o \
		 obj/cref/m25519/sign.o

cref_kl25519 = obj/cref/kl25519/ladder.o \
		  obj/cref/kl25519/dh.o \
		  obj/cref/kl25519/sign.o

arm_m25519 = obj/arm/m25519/ladder.o \
		  obj/arm/m25519/dh.o \
		  obj/arm/m25519/sign.o

arm_kl25519 = obj/arm/kl25519/ladder.o \
		  obj/arm/kl25519/dh.o \
		  obj/arm/kl25519/sign.o

test/cref/print_cref: test/cref/cref_print.c
	$(cref_CC) $(cref_FLAGS) $^ -o $@

test/cref/test_cref_m25519: test/cref/test_cref_m25519.c test/cref/cref_print.c libs/cref_m25519.a
	$(cref_CC) $(cref_FLAGS) $^ -o $@

test/cref/test_cref_kl25519: test/cref/test_cref_kl25519.c test/cref/cref_print.c libs/cref_kl25519.a
	$(cref_CC) $(cref_FLAGS) $^ -o $@

libs/cref_m25519.a: $(cref) $(cref_m25519)
	mkdir -p libs/
	$(AR) -ar cr $@ $^

libs/cref_kl25519.a: $(cref) $(cref_kl25519)
	mkdir -p libs/
	$(AR) -ar cr $@ $^

obj/cref/%.o: cref/%.[cS]
	mkdir -p obj/cref/
	$(cref_CC) $(cref_FLAGS) -c $^ -o $@

obj/cref/m25519/%.o: m25519/%.[cS]
	mkdir -p obj/cref/m25519/
	$(cref_CC) $(cref_FLAGS) -c $^ -o $@

obj/cref/kl25519/%.o: kl25519/%.[cS]
	mkdir -p obj/cref/kl25519/
	$(cref_CC) $(cref_FLAGS) -c $^ -o $@

obj/cref/m25519/%.o: %.[cS]
	mkdir -p obj/cref/m25519/
	$(cref_CC) $(cref_FLAGS) -c $^ -o $@

obj/cref/kl25519/%.o: %.[cS]
	mkdir -p obj/cref/kl25519/
	$(cref_CC) $(cref_FLAGS) -c $^ -o $@

# ARM

%.bin: %.elf
		 arm-none-eabi-objcopy  -O binary $^ $@

## ARM M25519

test/arm/m25519/speed.elf: $(STMOBJ) test/arm/speed.c test/arm/print.c libs/arm_m25519.a 
	$(arm_CC) $(arm_CFLAGS) $(arm_INCDIRS) -T $(LINKERFILE) $(STMOBJ)  test/arm/speed.c test/arm/print.c libs/arm_m25519.a -o $@

test/arm/m25519/stack.elf: $(STMOBJ) test/arm/stack.c test/arm/print.c libs/arm_m25519.a 
	$(arm_CC) $(arm_CFLAGS) $(arm_INCDIRS) -T $(LINKERFILE) $(STMOBJ)  test/arm/stack.c test/arm/print.c libs/arm_m25519.a -o $@

test/arm/m25519/test.elf: $(STMOBJ) test/arm/test.c libs/arm_m25519.a 
	$(arm_CC) $(arm_CFLAGS) $(arm_INCDIRS) -T $(LINKERFILE) $(STMOBJ) test/arm/test.c test/arm/print.c test/arm/fail.c libs/arm_m25519.a -o $@

arm/stm32f0xx/%.o: arm/stm32f0xx/%.c
	$(arm_CC) $(arm_CFLAGS) $(arm_INCDIRS) -c $^ -o $@

arm/stm32f0xx/%.o: arm/stm32f0xx/%.s
	$(arm_CC) $(arm_ASFLAGS) -c $^ -o $@

obj/arm/%.o: arm/%.c
	mkdir -p obj/arm/
	$(arm_CC) $(arm_CFLAGS) $(arm_INCDIRS) -c $^ -o $@

obj/arm/%.o: arm/%.s
	$(arm_CC) $(arm_ASFLAGS) -c $^ -o $@

obj/arm/m25519/%.o: m25519/%.c
	mkdir -p obj/arm/m25519/
	$(arm_CC) $(arm_CFLAGS) $(arm_INCDIRS) -c $^ -o $@

obj/arm/m25519/%.o: %.c
	mkdir -p obj/arm/m25519/
	$(arm_CC) $(arm_CFLAGS) $(arm_INCDIRS) -c $^ -o $@

libs/arm_m25519.a: $(arm) $(arm_m25519)
	mkdir -p libs/
	$(arm_AR) -ar cr $@ $^ 

## ARM KL25519

test/arm/kl25519/speed.elf: $(STMOBJ) test/arm/speed.c test/arm/print.c libs/arm_kl25519.a 
	$(arm_CC) $(arm_CFLAGS) $(arm_INCDIRS) -T $(LINKERFILE) $(STMOBJ)  test/arm/speed.c test/arm/print.c libs/arm_kl25519.a -o $@

test/arm/kl25519/stack.elf: $(STMOBJ) test/arm/stack.c test/arm/print.c libs/arm_kl25519.a 
	$(arm_CC) $(arm_CFLAGS) $(arm_INCDIRS) -T $(LINKERFILE) $(STMOBJ)  test/arm/stack.c test/arm/print.c libs/arm_kl25519.a -o $@

test/arm/kl25519/test.elf: $(STMOBJ) test/arm/test.c libs/arm_kl25519.a 
	$(arm_CC) $(arm_CFLAGS) $(arm_INCDIRS) -T $(LINKERFILE) $(STMOBJ) test/arm/test.c test/arm/print.c test/arm/fail.c libs/arm_kl25519.a -o $@

arm/stm32f0xx/%.o: arm/stm32f0xx/%.c
	$(arm_CC) $(arm_CFLAGS) $(arm_INCDIRS) -c $^ -o $@

arm/stm32f0xx/%.o: arm/stm32f0xx/%.s
	$(arm_CC) $(arm_ASFLAGS) -c $^ -o $@

obj/arm/%.o: arm/%.c
	mkdir -p obj/arm/
	$(arm_CC) $(arm_CFLAGS) $(arm_INCDIRS) -c $^ -o $@

obj/arm/%.o: arm/%.s
	$(arm_CC) $(arm_ASFLAGS) -c $^ -o $@

obj/arm/kl25519/%.o: kl25519/%.c
	mkdir -p obj/arm/kl25519/
	$(arm_CC) $(arm_CFLAGS) $(arm_INCDIRS) -c $^ -o $@

obj/arm/kl25519/%.o: %.c
	mkdir -p obj/arm/kl25519/
	$(arm_CC) $(arm_CFLAGS) $(arm_INCDIRS) -c $^ -o $@

libs/arm_kl25519.a: $(arm) $(arm_kl25519)
	mkdir -p libs/
	$(arm_AR) -ar cr $@ $^ 

.PHONY: clean

clean:
	-rm -r obj/cref/*
	-rm -r obj/arm/*
	-rm libs/*
	-rm test/cref/test_cref_m25519
	-rm test/cref/test_cref_kl25519
	-rm test/arm/kl25519/test.elf
	-rm test/arm/kl25519/test.bin
	-rm test/arm/kl25519/stack.elf
	-rm test/arm/kl25519/stack.bin
	-rm test/arm/kl25519/speed.elf
	-rm test/arm/kl25519/speed.bin
	-rm test/arm/m25519/test.elf
	-rm test/arm/m25519/test.bin
	-rm test/arm/m25519/stack.elf
	-rm test/arm/m25519/stack.bin
	-rm test/arm/m25519/speed.elf
	-rm test/arm/m25519/speed.bin

run_cref_m25519:
	test/cref/test_cref_m25519

run_cref_kl25519:
	test/cref/test_cref_kl25519

run_arm_m25519:
	sudo test/arm/m25519/test_stm32f0.sh

run_arm_kl25519:
	sudo test/arm/kl25519/test_stm32f0.sh
