TARGET		= firm_linux_loader_arm9
SOURCES		= source source/fatfs
INCLUDES	= include include/fatfs ../common

SFILES	= $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.S))
CFILES	= $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.c))

OBJS	= $(SFILES:.S=.o) $(CFILES:.c=.o)
DEPS	= $(OBJS:.o=.d)
INCLUDE	= $(foreach dir, $(INCLUDES), -I$(CURDIR)/$(dir))

ARCH	= -mcpu=arm946e-s -march=armv5te -mlittle-endian -mthumb-interwork
ASFLAGS	= $(ARCH) $(COMMON_ARCH) $(INCLUDE)
CFLAGS	= -Wall -Os -fno-builtin $(ARCH) $(INCLUDE) -fno-stack-protector

.PHONY: all clean copy

all: $(TARGET).elf

$(TARGET).elf: $(OBJS)
	$(CC) -T link.ld -Wl,--nmagic $^ -o $@ -nostartfiles -specs=nano.specs -specs=nosys.specs
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@
%.o: %.S
	$(CC) $(ASFLAGS) -MMD -MP -c $< -o $@

clean:
	@rm -f $(OBJS) $(TARGET).elf $(DEPS)

-include $(DEPS)
