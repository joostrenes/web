TARGET_DEVICE=atmega2560
CPUFREQ=16000000 
CC=/usr/bin/avr-gcc
OBJCOPY=/usr/bin/avr-objcopy
AR=/usr/bin/avr-ar
STRIP=/usr/bin/avr-strip

CFLAGS  = -g -Wall -Wextra -Werror -Wno-uninitialized -mmcu=$(TARGET_DEVICE) -O2 -DF_CPU=$(CPUFREQ) -mcall-prologues

all: test/test.hex \
    test/speed.hex \
    test/stack.hex

KUMMER = obj/gfe.o \
		obj/kummer.o \
        obj/bigint_mul.o \
        obj/bigint_sqr.o \
        obj/bigint_red.o \
        obj/gfe_mulconst.o \
        obj/gfe_neg.o \
        obj/gfe_add.o \
        obj/gfe_sub.o \
        obj/hadamard.o \
        obj/gfe_pack.o \
        obj/jac.o \
        obj/modN.o \
        obj/Keccak-shake128-fast.o \
        obj/signatures.o \
        obj/keyexchange.o

TEST = test/print.c \
       test/avr.c \
       test/fail.c

SPEED = test/print.c \
        test/avr.c \
        test/fail.c \
        test/cpucycles.c

test/test: test/test.c obj/kummer.a $(TEST)
	$(CC) $(CFLAGS) $^ -o $@

test/speed: test/speed.c obj/kummer.a $(SPEED)
	$(CC) $(CFLAGS) $^ -o $@

test/stack: test/stack.c obj/kummer.a $(TEST)
	$(CC) $(CFLAGS) $^ -o $@

test/%.hex: test/%
	$(OBJCOPY) -O ihex -R .eeprom $^ $@

obj/kummer.a: $(KUMMER)
	$(AR) -ar cr $@ $^

obj/%.o: %.[cS]
	mkdir -p obj/
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean

clean:
	-rm -r obj/*
	-rm -r test/test
	-rm -r test/speed
	-rm -r test/stack
