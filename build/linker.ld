/*
 * Basic linker script to override the default gcc linker behaviour.
 * Needed because our kernel is not an ordinary user program.
 * See details at https://jsandler18.github.io/explanations/linker_ld.html
 */
ENTRY(_start)

SECTIONS
{
    /* Starts at LOADER_ADDR. */
    . = 0x80000;
    __start = .;

    /* executable code */
    __text_start = .;
    .text :
    {
        KEEP(*(.text.boot))
        *(.text)
    }
    . = ALIGN(4096); /* This and all subsequent seciton are page aligned */
    __text_end = .;

    /* read only data, where global constants are placed */
    __rodata_start = .;
    .rodata :
    {
        *(.rodata)
    }
    . = ALIGN(4096);
    __rodata_end = .;

    /* global variables that are initialized at compile time */
    __data_start = .;
    .data :
    {
        *(.data)
    }
    . = ALIGN(4096);
    __data_end = .;

    /* uninitialized global variables */
    __bss_start = .;
    .bss :
    {
        bss = .;
        *(.bss)
    }
    . = ALIGN(4096);
    __bss_end = .;
    __end = .;
}