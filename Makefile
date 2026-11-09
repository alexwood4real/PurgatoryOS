CROSS ?= aarch64-elf-

AS 		= $(CROSS)as
CC 		= $(CROSS)gcc
LD 		= $(CROSS)ld
OBJDUMP = $(CROSS)objdump

ASFLAGS  = -g
CFLAGS 	 = -g -ffreestanding -nostdlib -mcpu=cortex-a53 -O2
LDFLAGS  = -T link.ld

TARGET 	= kernel.elf
OBJS 	= boot.o uart.o kernel.o

all: $(TARGET)

$(TARGET): $(OBJS) link.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.S
	$(AS) $(ASFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

run: $(TARGET)
	qemu-system-aarch64 \
		-M virt \
		-cpu cortex-a53 \
		-nographic \
		-kernel $(TARGET)

gdb: $(TARGET)
	qemu-system-aarch64 \
		-M virt \
		-cpu cortex-a53 \
		-nographic \
		-kernel $(TARGET) \
		-S -gdb tcp::1234

dump: $(TARGET)
	$(OBJDUMP) -d $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run gdp dump clean