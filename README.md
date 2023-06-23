# Simple OS

## Description

This repository serves as well the guide to setting your beginner footsteps into the world of making OS and OS-related stuff. Don't be scared, for because outputting the text and changing the colors isn't that difficult!

You can use makefile to generate the image file that you can load in any virtual machine that supports it.

Or maybe you want to do it yourself? No problem! The following parts will be about writing the necessary stuff to compile and then create an image file to load.

## Notes

- This is intended to be the improvement upon the older guides that allowed it to happen.
- The code is entirely 32-bit, so no -x86-64!

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

Then, we will write a kernel code, `kernel.cpp`... **without standard libraries!** What does that mean!? There's a good reason: These libraries are meant for user mode, not kernel mode. As well, we will not be able to use library subroutines either. But hey, we still have portable headers that we can use, such as `<stdint.h>` and `<stddef.h>` that will be included in the code.

Afterwards, we will write linker file so that the image file will be read by the bootloader start execution at the symbol designated as the entry point.

Lastly, all of these files need to be compiled and linked together to form one kernel binary, which then should be put in the `iso` directory and generate the image file with `grub-mkrescue`.

`WIP`
