/* ESP32-C3 Bootloader Linker Script */

MEMORY
{
  /* Bootloader flash region - 32KB at start of flash */
  FLASH (rx) : ORIGIN = 0x0, LENGTH = 32K
  
  /* Firmware region starts after bootloader */
  FLASH_FW (rx) : ORIGIN = 0x8000, LENGTH = 992K
  
  /* Instruction RAM - for code that must run from RAM */
  IRAM (rx) : ORIGIN = 0x40380000, LENGTH = 128K
  
  /* Data RAM */
  DRAM (rw) : ORIGIN = 0x3FC80000, LENGTH = 400K
  
  /* Retained memory (RTC memory) - 256 bytes */
  RETAINED (rw) : ORIGIN = 0x50000000, LENGTH = 256
}

ENTRY(boot_main)

SECTIONS
{
    /* Vector table and startup code */
    .vectors : {
        KEEP(*(.vectors))
    } > FLASH

    /* Code section */
    .text : {
        _text_start = .;
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        . = ALIGN(4);
        _text_end = .;
    } > FLASH

    /* Retained section (for boot bits) */
    .retained : ALIGN(4) {
        __retained_start = .;
        *(.retained)
        *(.retained.*)
        __retained_end = .;
    } > RETAINED

    /* Data section (initialized data) */
    .data : {
        _data_start = .;
        *(.data)
        *(.data.*)
        . = ALIGN(4);
        _data_end = .;
    } > DRAM AT > FLASH

    _data_load_start = LOADADDR(.data);

    /* BSS section (uninitialized data) */
    .bss : {
        _bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } > DRAM

    /* Stack */
    .stack : {
        . = ALIGN(8);
        _stack_end = .;
        . += 0x2000;  /* 8KB stack */
        _stack_start = .;
    } > DRAM

    /* Firmware region marker */
    .flash_fw : ALIGN(4) {
        __fw_start = .;
    } > FLASH_FW

    /* Discard sections */
    /DISCARD/ : {
        *(.comment)
        *(.note*)
    }
}

/* Symbols for initialization */
PROVIDE(_heap_start = _bss_end);
PROVIDE(_heap_end = _stack_end);

