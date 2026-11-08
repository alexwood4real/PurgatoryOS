CROSS ?= aarch64-elf-

AS 		= $(CROSS)as
LD 		= $(CROSS)ld
OBJDUMP = $(CROSS)objdump

ASWFLAGS = -g
LDFLAGS = -T link.ld

TARGET = kernel.elf
OBJS = boot.o

all: $(TARGET)

$(TARGET): $(OBJS) link.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.S
	$(AS) $(ASWFLAGS) -o $@ $<

run: $(TARGET)
	qemu-system-aarch64 \
		-M virt \
		-cpu cortex-a53 \
		-nographic \
		-kernel $(TARGET) \
		-semihosting

dump: $(TARGET)
	$(OBJDUMP) -d $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run dump clean