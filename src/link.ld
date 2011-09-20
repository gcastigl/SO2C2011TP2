OUTPUT_FORMAT("binary")
ENTRY(start)
SECTIONS
{
    . = 0x100000;	/* inicializacion de location counter(LC) con direccion fisica ( 1MB )*/
    .text : {
    code = .;  		/* definicion e inicializacion de code con el valor actual del LC*/
    *(.text)
    . = ALIGN(4096); 	/* se lleva el LC al comienzo de un pagina*/
  }
  .data  : {
    data = .; 
    *(.data)
    . = ALIGN(4096);
  }
  .bss  :
  {
    bss = .; 
    *(.bss)
    . = ALIGN(4096);
  }
  end = .; 
}
