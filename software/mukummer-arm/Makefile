CC= clang
CFLAGS= -fshort-enums -O2 -DCORTEX_M0 -mthumb -ffunction-sections -fdata-sections -fmessage-length=0 -mcpu=cortex-m0 -fno-builtin  -ffreestanding -target arm-none-eabi -mfloat-abi=soft -nostdlib -no-integrated-as
ASFLAGS= -mcpu=cortex-m0 -target arm-none-eabi -mfloat-abi=soft -nostdlib -no-integrated-as
INCDIRS=-I./stm32f0xx
AR = arm-none-eabi-ar

STMOBJ = stm32f0xx/system_stm32f0xx.o  \
         stm32f0xx/stm32f0xx_gpio.o  \
         stm32f0xx/stm32f0xx_rcc.o  \
         stm32f0xx/stm32f0xx_usart.o  \
         stm32f0xx/startup_stm32f0xx.o \
				 stm32f0xx/sbrk.o

LINKERFILE = stm32f0xx/stm32f0_linker.ld

all: test/speed.bin test/test.bin test/stack.bin

test/speed.elf: $(STMOBJ) test/speed.c test/print.c obj/kummer.a 
	$(CC) $(CFLAGS) $(INCDIRS) -T $(LINKERFILE) $(STMOBJ)  test/speed.c test/print.c obj/kummer.a -o $@

test/stack.elf: $(STMOBJ) test/stack.c test/print.c obj/kummer.a 
	$(CC) $(CFLAGS) $(INCDIRS) -T $(LINKERFILE) $(STMOBJ)  test/stack.c test/print.c test/randombytes.c obj/kummer.a -o $@

test/test.elf: $(STMOBJ) test/test.c obj/kummer.a 
	$(CC) $(CFLAGS) $(INCDIRS) -T $(LINKERFILE) $(STMOBJ) test/test.c test/print.c test/randombytes.c test/fail.c obj/kummer.a -o $@

obj/kummer.a: obj/gfe.o \
                obj/kummer.o \
                obj/jac.o \
                obj/modN.o \
                obj/signatures.o \
                obj/keyexchange.o \
                obj/bigint_sqr.o \
                obj/bigint_mul.o \
                obj/bigint_red.o \
                obj/gfe_mulconst.o \
                obj/gfe_add.o \
                obj/gfe_sub.o \
                obj/hadamard.o \
                obj/gfe_neg.o \
                obj/gfe_pack.o \
                obj/Keccak-inplace32BI.o 
	mkdir -p obj/
	$(AR) -ar cr $@ $^ 

%.bin: %.elf
		 arm-none-eabi-objcopy  -O binary $^ $@

obj/%.o: %.c
	mkdir -p obj/
	$(CC) $(CFLAGS) $(INCDIRS) -c $^ -o $@

obj/%.o: %.s
	mkdir -p obj/
	$(CC) $(ASFLAGS) -c $^ -o $@

stm32f0xx/%.o: stm32f0xx/%.c
	$(CC) $(CFLAGS) $(INCDIRS) -c $^ -o $@

stm32f0xx/%.o: stm32f0xx/%.s
	$(CC) $(ASFLAGS) -c $^ -o $@

.PHONY: clean

clean:
	-rm obj/*
	-rm stm32f0xx/*.o
	-rm test/test.elf
	-rm test/test.bin
	-rm test/speed.elf
	-rm test/stack.bin
	-rm test/stack.elf
	-rm test/speed.bin
