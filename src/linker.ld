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
