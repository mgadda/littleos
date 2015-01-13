OBJECTS = multiboot.o framebuffer.o loader.o kernel.o io.o serial.o string.o
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
					-nostartfiles -nodefaultlibs -Wall -Wextra -Werror \
					-Wall -Wextra -Werror -c
LDFLAGS = -arch i386 -e loader -static
#-T link.ld -melf_i386
AS = nasm
ASFLAGS = -f macho32

all: kernel.elf os.iso

run: os.iso
	bochs -f bochsrc.txt -q

os.iso: kernel.elf
	cp kernel.elf iso/boot/
	hdiutil makehybrid \
					-o os.iso \
					-iso -hfs -joliet \
					-eltorito-boot iso/boot/grub/stage2_eltorito \
					-no-emul-boot \
					-boot-load-size 4 \
					iso/

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

