#include <paging/paging.h>

PRIVATE void _createFirstPage();
PRIVATE void _enablePaging();

//the page directory comes right after the kernel
u32int page_aligned_end;
u32int *page_directory;
u32int* first_page_table;


void paging_init(u32int start) {
	log(L_DEBUG, "start using address: %d", start);
	//the page directory comes right after the kernel
	page_aligned_end = (start&0xFFFFF000) + 0x1000; // 0x1000 = 1024 * sizeof(u32int) = 4096
	page_directory = (u32int*) page_aligned_end;
	//set each entry to not present
	for(int i = 0; i < TABLE_ENTRIES; i++) {
	    //attribute: supervisor level, read/write, not present.
	    page_directory[i] = 0 | 2;
	}
	first_page_table = page_directory + 0x1000;

	_createFirstPage();
	// put the newly created page table into page directory
	page_directory[0] = (u32int) first_page_table;
	page_directory[0] |= 3;// attributes: supervisor level, read/write, present

	_enablePaging();
}

PRIVATE void _createFirstPage() {
	// holds the physical address where we want to start mapping these pages to.
	// in this case, we want to map these pages to the very beginning of memory.
	unsigned int address = 0;
	// we will fill all 1024 entries, mapping 4 megabytes
	for(int i = 0; i < TABLE_ENTRIES; i++) {
	    first_page_table[i] = address | 3; // attributes: supervisor level, read/write, present.
	    address += 4096; //advance the address to the next page boundary
	}
}

PRIVATE void _enablePaging() {
	// moves page_directory(pointer) into the cr3 register.
	//asm volatile("mov %0, %%cr3":: "b"(page_directory));
	__asm volatile("mov %0, %%cr3":: "b"(page_directory));
	//reads cr0, switches the "paging enable" bit, and writes it back.
	unsigned int cr0;
	__asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;
	__asm volatile("mov %0, %%cr0":: "b"(cr0));
}
