CPPFILES := $(shell find src/ -name "*.cpp")

build:
    nasm -f elf src/boot.asm -o boot.o
    g++ -Wall -Wpedantic -fanalyzer -c kernel.cpp -o kernel.o -ffreestanding -fno-exceptions -fno-rtti -march=i386 -m32
    i386-linux-gcc boot.o kernel.o -T src/linker.ld -o kern -nostdlib -nodefaultlibs -lgcc -z noexecstack
    rm -f *.o
    mkdir -p iso/boot/grub/
    touch iso/boot/grub/grub.cfg
    echo "menuentry \"os\"\n{\nmultiboot /boot/kern\n}" > iso/boot/grub/grub.cfg
    mv kern iso/boot/kern
    grub-mkrescue iso --output=gg.iso
    rm -r iso/
