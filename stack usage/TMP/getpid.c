/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
SYSCALL getpid()
{
        if(flag==1)
        {
                call_flag[currpid][1]=1;
                call_count[currpid][1]++;
        }

	return(currpid);
}

