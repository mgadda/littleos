

all: kernel.elf iso

run: iso 
	bochs -f bochsrc.txt -q
	
iso: kernel.elf 
	cp kernel.elf iso/boot/
	genisoimage -R\
              -b boot/grub/stage2_eltorito    \
              -no-emul-boot                   \
              -boot-load-size 4               \
              -A os                           \
              -input-charset utf8             \
              -quiet                          \
              -boot-info-table                \
              -o os.iso                       \
              iso

kernel.elf: loader.o
	ld -T link.ld -melf_i386 loader.o -o kernel.elf

loader.o:
	nasm -f elf32 loader.s
	#nasm -f elf loader.s

clean:
	rm -f kernel.elf iso/boot/kernel.elf *.o os.iso

