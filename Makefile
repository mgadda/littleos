OBJECTS = multiboot.o framebuffer.o loader.o kernel.o \
					io.o serial.o string.o interrupt.o \
					descriptor_tables.o dt.o isr.o timer.o \
					apic.o keyboard.o paging.o paging.asm.o memory.o
CC = gcc
CFLAGS = -m32 -fno-stack-protector \
					-ffreestanding \
					-Wall -Wextra -Wno-unused -Werror -g -c
# things you may or may not need -nostdinc -fno-builtin -nostdlib -nostartfiles -nodefaultlibs
# see http://forum.osdev.org/viewtopic.php?f=1&t=25585
LDFLAGS = -T link.ld -melf_i386 -L/usr/lib/gcc/i686-linux-gnu/4.6 -lgcc
AS = nasm
ASFLAGS = -f elf

all: kernel.elf os.iso

run: os.iso
	bochs -f bochsrc.txt -q

os.iso: kernel.elf
	cp kernel.elf iso/boot/
	genisoimage -R \
              -b boot/grub/stage2_eltorito    \
              -no-emul-boot                   \
              -boot-load-size 4               \
              -A os                           \
              -input-charset utf8             \
              -quiet                          \
              -boot-info-table                \
              -o os.iso                       \
              iso

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

dump: kernel.elf
	readelf -WS kernel.elf
	objdump -D -Mintel-mnemonics kernel.elf

clean:
	rm -f kernel.elf iso/boot/kernel.elf *.o os.iso

