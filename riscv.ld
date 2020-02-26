SECTIONS
{
  . = 0x10000;
  .text ALIGN(0x4) : { *(.text) }
  . = 0x80000;
  .sdata ALIGN(0x4) : { *(.sdata) }
}

