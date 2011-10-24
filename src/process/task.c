#include <process/task.h>

volatile task_t *current_task;
volatile task_t *ready_queue;

extern page_directory_t *kernel_directory;
extern page_directory_t *current_directory;
extern void alloc_frame(page_t*,int, int);
extern u32int initial_esp;
extern u32int read_eip();

u32int next_pid = 1;

void initialize_tasking() {
    move_stack((void*)0xE0000000, 0x2000);
    current_task = ready_queue = (task_t*)kmalloc(sizeof(task_t));
    current_task->id = next_pid++;
    current_task->esp = current_task->ebp = 0;
    current_task->eip = 0;
    current_task->page_directory = current_directory;
    current_task->next = 0;
}

int fork() {
    __asm volatile("cli");
    
    task_t *parent_task = (task_t*)current_task;
    page_directory_t *directory = clone_directory(current_directory);
    
    task_t *new_task = (task_t*)kmalloc(sizeof(task_t));
    new_task->id = next_pid++;
    new_task->esp = new_task->ebp = 0;
    new_task->eip = 0;
    new_task->page_directory = directory;
    new_task->next = 0;
    
    task_t *tmp_task = (task_t*)ready_queue;
    while (tmp_task->next) {
        tmp_task = tmp_task->next;
    }
    tmp_task->next = new_task;
    
    u32int eip = read_eip();
    
    if (current_task == parent_task) {
        u32int esp;
            __asm volatile("mov %%esp, %0" : "=r" (esp));
        u32int ebp;
            __asm volatile("mov %%ebp, %0" : "=r" (ebp));
        new_task->esp = esp;
        new_task->ebp = ebp;
        new_task->eip = eip;
        
        __asm volatile("sti");
        return new_task->id;
    } else {
        return 0;
    }
    
}

void move_stack(void *new_stack_start, u32int size) {
      // Allocate some space for the new stack.
      for(int i = (u32int)new_stack_start;
           i >= ((u32int)new_stack_start-size);
           i -= 0x1000)
      {
        // General-purpose stack is in user-mode.
        alloc_frame( get_page(i, 1, current_directory), 0 /* User mode */, 1 /* Is writable */ );
      }
      
      // Flush the TLB by reading and writing the page directory address again.
      u32int pd_addr;
      __asm volatile("mov %%cr3, %0" : "=r" (pd_addr));
      __asm volatile("mov %0, %%cr3" : : "r" (pd_addr));

      // Old ESP and EBP, read from registers.
      u32int old_stack_pointer; asm volatile("mov %%esp, %0" : "=r" (old_stack_pointer));
      u32int old_base_pointer;  asm volatile("mov %%ebp, %0" : "=r" (old_base_pointer));
      
      // Offset to add to old stack addresses to get a new stack address.
      u32int offset            = (u32int)new_stack_start - initial_esp;

      // New ESP and EBP.
      u32int new_stack_pointer = old_stack_pointer + offset;
      u32int new_base_pointer  = old_base_pointer  + offset;

      // Copy the stack.
      memcpy((void*)new_stack_pointer, (void*)old_stack_pointer, initial_esp-old_stack_pointer);

      // Backtrace through the original stack, copying new values into
      // the new stack.  
      for(int i = (u32int)new_stack_start; i > (u32int)new_stack_start-size; i -= 4)
      {
        u32int tmp = * (u32int*)i;
        // If the value of tmp is inside the range of the old stack, assume it is a base pointer
        // and remap it. This will unfortunately remap ANY value in this range, whether they are
        // base pointers or not.
        if (( old_stack_pointer < tmp) && (tmp < initial_esp))
        {
          tmp = tmp + offset;
          u32int *tmp2 = (u32int*)i;
          *tmp2 = tmp;
        }
      }
      
      // Change stacks.
      __asm volatile("mov %0, %%esp" : : "r" (new_stack_pointer));
      __asm volatile("mov %0, %%ebp" : : "r" (new_base_pointer)); 
}

void switch_task() {
    if (!current_task)
        return;
    
    u32int esp, ebp, eip;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    asm volatile("mov %%ebp, %0" : "=r"(ebp));

    eip = read_eip();
    if (eip == 0x12345)
        return;
    printf("EIP that will break: %p", eip);
    current_task->eip = eip;
    current_task->esp = esp;
    current_task->ebp = ebp;
    
    current_task = current_task->next;
    if (!current_task) current_task = ready_queue;
    
    eip = current_task->eip;
    esp = current_task->esp;
    ebp = current_task->ebp;

    current_directory = current_task->page_directory;
    
    
    __asm volatile("         \
      cli;                 \
      mov %0, %%ecx;       \
      mov %1, %%esp;       \
      mov %2, %%ebp;       \
      mov %3, %%cr3;       \
      mov $0x12345, %%eax; \
      sti;                 \
      jmp *%%ecx;           "
                 : : "r"(eip), "r"(esp), "r"(ebp), "r"(current_directory->physicalAddr));
}

int getpid() {
    return current_task->id;
}