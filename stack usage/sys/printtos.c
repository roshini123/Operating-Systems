#include <stdio.h>
static unsigned long *top;
int i;
void printtos()
{
    kprintf("\n\n3. printtos");
    asm("movl %ebp,top");
    kprintf("Address of stack top before printtos: 0x%08x\n",top+2);
    kprintf("Address of stack top after printtos: 0x%08x\n",top);
    for(i = 1; i <= 6; ++i){
	kprintf("stack at[0x%08x]: 0x%08x\n",top-i,*(top-i));
    }

}        
