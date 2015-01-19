OBJECTS = multiboot.o io.o framebuffer.o loader.o kernel.o serial.o string.o
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
					-Wall -Wextra -Werror -g -c
LDFLAGS = -arch i386 -e loader -static -macosx_version_min 10.10 \
					-image_base 0x00100000 \
					-segaddr __TEXT 0x00100000 \
					-segalign 0x1000 \
					-sectalign __TEXT __text 0x1000 \
					-sectalign __DATA __data 0x1000 \
					-sectalign __DATA __common 0x1000 \
					-sectalign __DATA __bss 0x1000 \
					-sectalign __TEXT __cstring 0x1000 \
					-sectalign __RODATA __rodata 0x1000 \
					-sectalign __BSS __bss 0x1000
AS = nasm
ASFLAGS = -f macho

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

	# hdiutil makehybrid \
	# 				-o os.iso \
	# 				-iso -hfs -joliet \
	# 				-eltorito-boot iso/boot/grub/stage2_eltorito \
	# 				-no-emul-boot \
	# 				-boot-load-size 4 \
	# 				iso/

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

dump: kernel.elf
	otool -d kernel.elf

clean:
	rm -f kernel.elf iso/boot/kernel.elf *.o os.iso

