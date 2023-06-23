# Simple OS

## Description

This repository serves as well the guide to setting your beginner footsteps into the world of making OS and OS-related stuff. Don't be scared, for because outputting the text and changing the colors isn't that difficult!

You can use the included makefile to generate the image file that you can load in any virtual machine that supports it.

Or maybe you want to do it yourself? No problem! The following parts will be about writing the necessary stuff to compile and then create an image file to load.

## Notes

- This is intended to be the improvement upon the older guides that allowed it to happen.
- The kernel is entirely 32-bit, so no -x86-64!

## Requirements and Dependencies

- `nasm` compiler -> <https://www.nasm.us/> or install with the package manager: `nasm`
- Latest `g++` compiler

    -> as part of GCC: <https://gcc.gnu.org/>
  
    Can be installed in Linux/Unix with the package manager: `g++`

- specific `i386` cross-compiler

    -> <https://mirrors.edge.kernel.org/pub/tools/crosstool/>

    Short guide for installation:
  
    - Depending on your architecture, choose the `i386-linux` one.
    - Read the index carefully and make sure to choose `x86_64-gcc-13.1.0-nolibc-i386-linux.tar.gz` or `x86_64-gcc-13.1.0-nolibc-i386-linux.tar.xz`
    - Unpack and do the following commands (with sudo one time):

    ```bash
        tar xf x86_64-gcc-13.1.0-nolibc-i386-linux.tar.gz
        cd gcc-13.1.0-nolibc/i386-linux/
        sudo cp -r * /usr/local/
        cd ../..
        rm -r gcc-13.1.0-nolibc
    ```

    - Test by writing `i386-linux-gcc` and nothing else.

 - `grub2` -> install `grub2` with the package manager, as well `xorriso` in case.

 - Virtual machine of your choice that can do `i386` (32-bit) architecture.

## Introduction

Let us ask ourselves the first question: What the hell is an OS!?

There are many definitions for OS, or operating system. According to <a href="https://www.oxfordlearnersdictionaries.com/definition/english/operating-system?q=operating+system">Oxford Learner's Dictionary</a>: An operating system is "a set of programs that controls the way a computer works and runs other programs".

In this page, we will learn step by step on how to create a pretty basic, 32-bit OS that purely relies on <a href="https://en.wikipedia.org/wiki/VGA_text_mode">**VGA text mode**</a>.

Operating systems can be written in many different programming languages, even your own custom/hobby language. Typically, for the development of an operating system, a popular choice of languages suited for OS development and kernel (core of an operating system) are: `C/C++`, `ASM`, `Ada`, `Bliss`, ... (refer to <a href="https://wiki.osdev.org/Languages">OSDev Wiki</a> for languages).

In this repo, we will deal exclusively with Assembly and C++ as well the linker language.

**Assembly** is a low-level programming language that is used to directly communicate with a computer's hardware.

**C++** is a medium-level (can do both low-level and high-level), general-purpose programming language.

We will use Assembly to create a boot file, set architecture to 32 bit and multiboot header as well call the kernel.

Then, we will write a kernel code, `kernel.cpp`... **without standard libraries!** What does that mean!? There's a good reason: These libraries are meant for user mode, not kernel mode. As well, we will not be able to use library subroutines either. But hey, we have portable headers that we can use, such as `<stdint.h>` and `<stddef.h>` that will be included in the code.

Afterwards, we will write linker file so that the image file will be read by the bootloader and start execution at the symbol designated as the entry point.

Lastly, all of these files need to be compiled and linked together to form one kernel binary, which then should be put in the `iso` directory, containing boot and grub directories, and generate the image file with `grub-mkrescue`.

Technically we could write a 1 sector (512 bytes) bootloader in Assembly entirely, but for the sake of simplicity, we'll use `grub2`. 

Remember, since we're making a kernel, **we have to be considerate and careful at all times.** Like for example, you might have a wrong value, then the kernel won't work as intended or crash and possibly mess with the hardwares. Always test kernel in the **virtual machine**, not on an actual machine.

All right, let's get going!

## Step 1

This is how the `boot.asm` should look like:

```as
; BOOT.ASM: BOOTLOADER
[bits 32] ; 32 BIT
extern _kernel_main ; MAIN IS IN CPP FILE AND SHOULD BE LINKED
; NEEDED BY BOOTLOADERS SUCH AS GRUB
section .mbHeader
align 0x4

; MULTIBOOT HEADER - SEE GRUB DOCS FOR DETAILS
MODULEALIGN equ 1<<0
MEMINFO equ 1<<1
FLAGS equ MODULEALIGN | MEMINFO
MAGIC equ 0x1BADB002 ; MAGIC NUMBER FOR BOOTLOADER
CHECKSUM equ -(MAGIC + FLAGS)

MULTIBOOTHEADER:
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

global start ; START OPERATING SYSTEM
start:
push ebx
call _kernel_main
```

Here's what's going on:
- 32 bit mode gets set
- `extern` allows the assembler to know the name, that is not defined in the current assembly, of the function `_kernel_main` in the C++ source file. This also lets us to link the assembly with the source kernel code.
- section directive assembles the following code into `.mbHeader`
- Alignment of the instruction by the 4 byte boundary is enforced for optimization purposes (we're working with 32-bit kernel)
- Align loaded modules on page boundaries
- Provide information about memory map (structure of data)
- Multiboot flag field
- Magic number to allow the bootloader to find the header
- Checksum -> "The field ‘checksum’ is a 32-bit unsigned value which, when added to the other magic fields (i.e. ‘magic’ and ‘flags’), must have a 32-bit unsigned sum of zero." (see <a href="https://www.gnu.org/software/grub/manual/multiboot/multiboot.html">multiboot manual</a>)
- In `MULTIBOOTHEADER`, assemble the contents into the output.
- Export the symbol `start`
- in `start`, push `ebx` (pushes the 32-bit register `ebx` to the stack) and call the function `_kernel_main` while passing the pointer to a multiboot header as well.

## Step 2

Now, let's write the kernel code like this:

```c++
#include <stddef.h>
#include <stdint.h>
/* Portable headers */

struct vga {
  static const size_t VGA_WIDTH = 80;
  static const size_t VGA_HEIGHT = 24;
  size_t term_row;
  size_t term_column;
  uint8_t term_color;
  volatile uint16_t *term_buf;
};
vga vgaobj;

/* Hardware text mode color constants */

enum vga_color {
  COLOR_BLACK,
  COLOR_BLUE,
  COLOR_GREEN,
  COLOR_CYAN,
  COLOR_RED,
  COLOR_MAGENTA,
  COLOR_BROWN,
  TEXT_COLOR_LIGHT_GREY,
  TEXT_COLOR_DARK_GREY,
  TEXT_COLOR_LIGHT_BLUE,
  TEXT_COLOR_LIGHT_GREEN,
  TEXT_COLOR_LIGHT_CYAN,
  TEXT_COLOR_LIGHT_RED,
  TEXT_COLOR_LIGHT_MAGENTA,
  TEXT_COLOR_LIGHT_BROWN,
  TEXT_COLOR_WHITE
};

uint8_t make_color(enum vga_color fg, enum vga_color bg) {
  return fg | bg << 4;
}

uint16_t vga_entry(uint8_t c, uint8_t color) {
  uint16_t c16 = c;
  uint16_t color16 = color;
  return c16 | color16 << 8;
}

size_t strlen(const char *str) {
  size_t ret = 0;
  while (str[ret] != 0) {
    ret++;
  }
  return ret;
}

void term_init() {
  vgaobj.term_row = 0;
  vgaobj.term_column = 0;
  vgaobj.term_color = make_color(TEXT_COLOR_WHITE, COLOR_BLUE);
  vgaobj.term_buf = (volatile uint16_t *)0xB8000;
  for (size_t y = 0; y < vgaobj.VGA_HEIGHT; y++) {
    for (size_t x = 0; x < vgaobj.VGA_WIDTH; x++) {
      const size_t index = y * vgaobj.VGA_WIDTH + x;
      vgaobj.term_buf[index] = vga_entry(' ', vgaobj.term_color);
    }
  }
}
/* In case the user wants to change color */
inline void term_setcolor(uint8_t color) { vgaobj.term_color = color; }

void term_putentry(uint8_t c, uint8_t color, size_t x, size_t y) {
  const size_t index = y * vgaobj.VGA_WIDTH + x;
  vgaobj.term_buf[index] = vga_entry(c, color);
}

void term_putchar(char c) {
  term_putentry(c, vgaobj.term_color, vgaobj.term_column, vgaobj.term_row);
  if (++vgaobj.term_column == vgaobj.VGA_WIDTH) {
    vgaobj.term_column = 0;
    if (++vgaobj.term_row == vgaobj.VGA_HEIGHT) {
      vgaobj.term_row = 0;
    }
  }
}

void term_writestr(const char *data) {
  size_t datalen = strlen(data);
  for (size_t i = 0; i < datalen; i++) {
    term_putchar(data[i]);
  }
}

extern "C" void _kernel_main() {
  term_init();
  term_writestr("Hello, world! >>Simple OS<<");
  while (1) {
  };
}
```

See, we have to create the stuff by ourselves entirely without the standard libraries.

What the code does:
- Include two headers we've mentioned above
- Create a structure called `vga`, which contains information about sizes, rows and columns as well color and buffer. We declare the variable afterwards so that we can access it at all times.
- We create an enum of VGA colors. **Note** that first 8 colors are for the background, while the remaining 8 are for the foreground (text in this case)
- `make_color`: Set colors with bitwise shift to left by 4, calculate the value of the attribute bytes
- `vga_entry`: Bitwise shift character and color to left by 8, calculate the value of the attribute bytes
- `strlen`: Count and return the length of the string that is passed by `term_writestr`
- `term_init`: Initalize the VGA terminal variables. **Note** that the number `0xB8000`is the physical memory address for VGA text buffer. See the VGA text mode link above.
- `term_init`: Further, fill the entire screen with the terminal color
- `term_setcolor`: Set new color
- `term_putentry`: Put the character and the color associated with it in the terminal
- `term_putchar`: Call `term_putentry`, do checks
- `term_writestr`: Call strlen, loop for each character in the string. During the loop, call `term_putchar` with the current index in a string
- `_kernel_main`: Call to initalize terminal and write string, then loop forever

As you might have noticed, the main function needs to be same name as in `boot.asm` as well, otherwise you'll get undefined reference errors.

## Step 3

Now, the linker part:

```txt
ENTRY(start)

SECTIONS
{
    /* 1 MIB */
    . = 1M;

    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.mbHeader)
        *(.text)
    }
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(.bss)
    }
}
```

`ENTRY(start)` is our entry point symbol. `SECTIONS` is basically a "picture" of how it should look like, the output file's layout. The sections are put at 1 MiB, which is a conventional place for kernels to be loaded at by the bootloader. The multiboot header needs to be placed first so it can be recognized by the GRUB bootloader. The rest are as it follows: code segment, read-only data, initalized read-write data, (uninitalized) read-write data and stack.

## Step 4

Now that we have them all, let's build the kernel, the core of the OS!

These are the instructions that you have to follow:

```bash
nasm -f elf boot.asm -o boot.o
g++ -Wall -Wpedantic -fanalyzer -c kernel.cpp -o kernel.o -ffreestanding -fno-exceptions -fno-rtti -march=i386 -m32
i386-linux-gcc boot.o kernel.o -T linker.ld -o kern -nostdlib -nodefaultlibs -lgcc -z noexecstack
```

You may wonder: "What are these flags for??? What do they do?"

We'll go step by step:

`nasm`:
    - `-f` means: filename
    - `elf`: produce linux output file
    - `-o`: specify output filename

Now come the lengthy ones for g++ and i386-linux-gcc:

`g++`:
    - `-Wall`: Enable all compiler warnings
    - `-Wpedantic`: Issue all the warnings demanded by strict ISO C and ISO C++
    - `-fanalyzer`: This option enables an static analysis of program flow which looks for “interesting” interprocedural paths through the code, and issues warnings for problems found on them
    - `-c`: Compile or assemble without linking
    - `-ffreestanding`: Target a freestanding environment, where standard library may not exist
    - `-fno-exceptions`: Disable exception handling
    - `-fno-rtti`: No runtime type information
    - `-march=i386`: Generate instructions for the machine type `i386`
    - `-m32`: Set int, long, and pointer types to 32 bits, and generates code that runs in 32-bit mode

`i386-linux-gcc`:
    - `-T`: Use script as the linker script.
    - `-nostdlib`: Do not use standard libraries
    - `-nodefaultlibs`: Same as above
    - `-lgcc`: Do not use internal GCC subroutines
    - `-z noexecstack`: Do not use executable stack

Whoops, that's a lot of flags to explain! We finally should have the output, `kern`.

We put the kernel in the `iso/boot/`, create another directory inside: `grub`, where we'll write a simple `grub.cfg`:

```cfg
menuentry "os"
{
multiboot /boot/kern
}
```

Don't forget to change name, the /boot/**kern**, in any case if you ever output kernel with a different name such as "simpleos", otherwise you won't get intended results.
You can change menuentry name as well.

Once done, this is what the structure should look like:

```txt
iso/
├─ boot/
│  ├─ kern
│  ├─ grub/
│  │  ├─ grub.cfg
```

Now, go back and finally, you get to use `grub-mkrescue iso --output=kern.iso`.
You then should have a ready image file that is approximately 5 MB in size.

Load it in the virtual machine, don't panic if there is GRUB console. Just type `boot`!

Bam, you have made it that far and wrote a pretty basic OS so you can write some text!

But remember, the actual long, complex and hard stuff comes when you start going beyond just text and colors, since you'll have to deal with way many things such as managing memory.

Otherwise, happy coding! I hope this guide has helped you in setting your first foots in OS/kernel development!
