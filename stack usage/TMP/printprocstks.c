#include <stdio.h>
#include <kernel.h>
#include <proc.h>
static unsigned long *t;
void printprocstks(int priority)
{
	kprintf("\n4.printprocstks\n");
        int i;
        struct pentry *pro;
        for(i=0;i<NPROC;i++)
        {
                pro=&proctab[i];
         	if(pro->pstate==PRFREE)
		continue;
		else
		{
	        	kprintf("\nprocess name: %s",pro->pname);
                	kprintf("\nprocess priority:%d",pro->pprio);
                	kprintf("\nprocess id:%d",i);
                	if(priority > pro->pprio)
                	{
                        	kprintf("\nPROCESS PRIORITY LOWER THAN PRIORIY ARGUMENT");
                        	kprintf("\nstack base:0x%08x",pro->pbase);
                        	kprintf("\nstack size: %d",pro->pstklen);
                        	kprintf("\nstack limit: 0x%08x",pro->plimit);
                        	if(currpid!=i)
                        	kprintf("\nstack pointer 0x%08x",pro->pesp);
                	}
			else
			{
				kprintf("\nPROCESS PRIORITY GREATER THAN PRIORITY ARGUMENT");
				kprintf("\nstack base:0x%08x",pro->pbase);
			}
                	if(currpid==i)
                	{
                		asm("movl %esp,t");
                		kprintf("\nthe current running process (%s) has the stack pointer:0x%08x\n",pro->pname,t);
                	}
		}
		kprintf("\n");
        }
}

