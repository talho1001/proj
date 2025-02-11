#include "ustack.h"
#include "kernel/riscv.h"

typedef struct {
    void* addr;
    uint size;
} StackEntry;

static StackEntry stack[512];
static int top = 0;
//static  last_buffer = 0;

void* ustack_malloc(uint len) {
    if (len > MAX_ALLOC || len <= 0) {
        return (void*)-1;
    }
    void* new_addr = sbrk(len);
    if (new_addr == (void*)-1)
        return (void*)-1;
    memset(new_addr,0,len);    
    stack[top] =  (StackEntry){.addr = new_addr, .size = len};
    return new_addr;


    /*
    if (stack[top].size + len < PGSIZE){
        int pSize = stack[top].size;//pre size
        stack[top].size = pSize + len;
        void* ret_addr = stack[top].addr + len; 
        last_buffer = ret_addr;
        return ret_addr;
    }
    else{
        top++;//add ,memset
        void* new_addr = sbrk(PGSIZE);
        if (new_addr == (void*)-1)
            return (void*)-1;
        memset(new_addr,0,PGSIZE);    
        stack[top] =  (StackEntry){.addr = new_addr, .size = len};
        last_buffer = new_addr;
        return new_addr;    
    }*/


}


int ustack_free(void) {
    if (top == 0 && stack[top].size == 0) {
        return -1;
    }
    // Pop from the stack
    StackEntry last_vuffer = stack[top];
    int size_vuffer = last_vuffer.size;
    sbrk(-last_vuffer.size);
    last_vuffer.addr = 0;
    last_vuffer.size = 0;
    top--;
    return size_vuffer;
    /*
    mem_top += entry.size;
    // Free a page if necessary
    if ((void*)PGROUNDDOWN((uint64)mem_top + PGSIZE) != mem_top) {
        mem_top = (void*)PGROUNDDOWN((uint64)mem_top + PGSIZE);
        sbrk(PGSIZE);
    }*/

    //return entry.size;
    return 1;
}

