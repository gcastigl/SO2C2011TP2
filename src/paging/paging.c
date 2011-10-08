#include <paging/paging.h>
// bitset con la informacion de los frames en uso
int *frames;
int nframes;

// definida en kheap.c
extern int nf_address; // ( Next free address )
extern heap_t *kheap;  

extern PROCESS process[];
extern int nextPID; 
extern int currentPID;

page_directory_t* kernel_directory = 0;

static void set_frame(int frameAddress );
static void clear_frame(int frameAddress );
static int test_frame(int frameAddress );
static int first_fframe( void );



void initialize_paging(void) 
{
    int i=0;
    int mem_end_page = 0x1000000;   // TEMPORALMENTE se asume que la memoria fisica total es de 32 MB
    
    nframes= mem_end_page / 0x1000; // Se inicializa nframes: se divide la memoria total en secciones de 4KB
    frames= (int*)kmalloc( INDEX_FROM_BIT(nframes) ); // Se aloca el campo de bits que indica los frames ocupados o no

    memset( frames, 0, INDEX_FROM_BIT(nframes) );     // Se inicializa frames en 0

   // Se aloca el directorio de paginas alineado
   kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
   memset( kernel_directory, 0, sizeof(page_directory_t) ); // Se inicializa en 0 el directorio de paginas

   //    current_directory = kernel_directory;
   
    // Map some pages in the kernel heap area.
    // Here we call get_page but not alloc_frame. This causes page_table_t's 
    // to be created where necessary. We can't allocate frames yet because they
    // they need to be identity mapped first below, and yet we can't increase
    // placement_address between identity mapping and enabling the heap!

    for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
        get_page(i, MAKE, kernel_directory);

    // We need to identity map (phys addr = virt addr) from
    // Allocate a liltle bit extra so the kernel heap can be initialized properly.
   
   i = 0;
   while( i < nf_address+0X1000 ) // Hasta el final de la memoria usada hasta el momento
   {
       // el codigo del kernel es legible pero no modificable por el espacio del usuario
       alloc_frame( get_page(i, MAKE, kernel_directory), 0, 0 );
       i += 0x1000; // avanza 4KB
   }
   
    // Now allocate those pages we mapped earlier.
    for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
        alloc_frame( get_page(i, MAKE, kernel_directory), 0, 0);

  enablePaging( kernel_directory );

  // Initialise the kernel heap.
  kheap = create_heap(KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, HEAP_MAX_SIZE, 0, 0);

}
 

void enablePaging( page_directory_t* dir )
{
  int cr0; 

  asm volatile("mov %0, %%cr3":: "r"(&dir->tablesPhysical)); // direccion fisica de la tabla de directorios
  asm volatile("mov %%cr0, %0": "=r"(cr0));
  cr0 |= 0x80000000;
  asm volatile("mov %0, %%cr0":: "r"(cr0));		    // Se reescribe el registro CR0 con el bit PG encendido
}


page_t *get_page( int address, int make, page_directory_t *dir )
{
   address /= 0x1000; 		   // pasa la direccion a un indice
   int table_idx = address / 1024; // Find the page table containing this address.
   u32int tmp;
   
   if (dir->tables[table_idx]) // If this table is already assigned

     return &dir->tables[table_idx]->pages[address%1024];
   
   else if( make )
   {
      // aloco una nueva tabla de paginas
       dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
       memset(dir->tables[table_idx], 0, 0x1000);       // Se inicializa en 0
       dir->tablesPhysical[table_idx] = tmp | 0x7; 	// PRESENT, RW, US
       
       return &dir->tables[table_idx]->pages[address%1024];
   }
   else
     
       return (void*)0;
} 


// Funcion para alocar un frame.
// Quien llama a alloc_frame previamente hace uso de la funcion get_page para crear la pagina previamente

void alloc_frame( page_t *page, int is_kernel, int is_writeable )
{
   int bit_idx=0;
   if (page->frame != 0)
     
      return; // El frame ya fue alocado
      
   else
   {
       bit_idx = first_fframe(); // indice del bit apagado, es decir el primer frame libre
       
       if( bit_idx == (unsigned int)-1 )
	       panic("No free frames!", 1, true);
	 
       set_frame( bit_idx*0x1000 ); 	// marca el frame como ocupado
       page->present = 1; 		// pagina presente
       page->rw = (is_writeable)?1:0; 	// se deberia poder escribir?
       page->user = (is_kernel)?0:1;	// deberia estar en modo usuario?
       page->frame = bit_idx;		
   }
}

// Funcion para desalojar un frame
void free_frame( page_t *page )
{
   int frame;
   if( !(frame=page->frame) )
       return; // La pagina no fue alocada previamente
       
   else
   {
       clear_frame( frame ); // Se desmarca el frame
       page->frame = 0x0;    // se reinicializa el frame a 0 
   }
} 

// FUNCIONES DE CAMPOS DE BITS PARA LA ADMINISTRACION DE LOS FRAMES OCUPADOS O LIBRES

static void set_frame(int frameAddress )
{
  int frame=frameAddress / 0x1000;	// tomo el indice de la direccion de memoria
  int idx= INDEX_FROM_BIT ( frame );	// en que indice en un arreglo de ints esta el bit que quiero encender
  int off= OFFSET_FROM_BIT( frame );	// dentro de ese int en que posicion se encuentra el bit a encender
  frames[idx] |= ( 0x1 << off );	// encienco el bit correspondiente
}

static void clear_frame(int frameAddress )
{
  int frame=frameAddress / 0x1000;	// tomo el indice de la direccion de memoria
  int idx= INDEX_FROM_BIT ( frame );	// en que indice en un arreglo de ints esta el bit que quiero encender
  int off= OFFSET_FROM_BIT( frame );	// dentro de ese int en que posicion se encuentra el bit a encender
  frames[idx] &= ~( 0x1 << off );	// apago el bit correspondiente
}

// retorna 1 en caso que el indice que corresponde al frameAddress este asignado
static int test_frame(int frameAddress )
{
  int frame=frameAddress / 0x1000;	// tomo el indice de la direccion de memoria
  int idx= INDEX_FROM_BIT ( frame );	// en que indice en un arreglo de ints esta el bit que quiero encender
  int off= OFFSET_FROM_BIT( frame );	// dentro de ese int en que posicion se encuentra el bit a encender
  return (frames[idx] & ( 0x1 << off ));// al frame referenciado le encienco el bit correspondiente
}

// Busca dentro del arreglo frames el primer lugar libre (bit apagado) de derecha a izquierda
// Este representa el primer frame que esta disponible para ser usado
// De encontrarlo retorna su indice traducido a bits en caso contrario retorna -1

static int first_fframe( void )
{
    int i,j,toTest;
    for( i = 0; i < INDEX_FROM_BIT( nframes ); i++ ) {
        if( frames[i] != 0xFFFFFFFF ) // Si hay algun lugar libre es decir algun bit apagado
            for (j = 0; j < 32; j++) { // Me fijo el primer bit que este apagado de derecha a izquierda
                toTest = 0x1 << j;
                if ( !(frames[i]&toTest) ) // si ese lugar esta libre, bit apagado
                   return i*32+j;// retorno el indice del frame que puede mapearse
            }
    }
    return 0;
} 

// a partir de un proceso dado setea como presentes o ausentes todas las paginas de un proceso ademas 
// de las paginas de sus ancestros

void flushPages	( int pid, int action )
{
	int pages, mem_dir, p, proc_idx;
	page_t *page;
	PROCESS * temp, *proc_parent;
	
	if( pid == 0 )
	  return;
	
	for(proc_idx=0; proc_idx<MAX_PROCESSES && process[proc_idx].pid!=pid;proc_idx++ ); 
	
	if( proc_idx == MAX_PROCESSES ){
	  printf("No se encontro proceso!");
	  return;
	}
	
	pages=process[proc_idx].stacksize/0x1000; // cuantas paginas tiene ese proceso

	//direccion de memoria donde comienza el stack ( operacion inversa de create process )
	mem_dir=process[proc_idx].stackstart-process[proc_idx].stacksize+1;
	
	for( p=0; p< pages ; ++p )
	{
		page=get_page( mem_dir,0,kernel_directory );
		page->present= action ; // DISABLE or ENABLE
		mem_dir+=0x1000; 	// 4kb step!
	}
	
	temp = getProcessByPID ( pid );
	if( temp->parent > 1 )
	{
		proc_parent=getProcessByPID( temp->parent );
		flushPages( temp->parent, action );
	}

	
}

void downPages( int old_pid )
{
  flushPages( old_pid, DISABLE ); 
  // Bajo las paginas del proceso actual que pasa a ser antiguo
  
}

void upPages( int cur_pid )
{
  flushPages( cur_pid, ENABLE );
  // levanto las paginas del proceso actual
}
