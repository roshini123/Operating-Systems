#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include<lock.h>
SYSCALL ldelete(int lock)
{
STATWORD ps;
        int     tempid,i;
        struct  lentry  *ptr;

        disable(ps);
        if (isbadlock(lock) || locktab[lock].lstate==LFREE) {
                restore(ps);
                return(SYSERR);
        }
	ptr = &locktab[lock];
        ptr->lstate = LFREE;
	int temp=(ptr->lqtail-1);
	if(ptr->lqtail>0)
	{
		while(temp>=0)
		{
			tempid=ptr->qu[temp].procid;
			proctab[tempid].plockret=DELETED;
			proctab[tempid].pltime[lock]=-1;
			proctab[tempid].pltype[lock]=0;
			ptr->lqtail=ptr->lqtail-1;
			temp--;
		}
	}
	for(i=0;i<NPROC;i++)
	{
		if(ptr->pids[i]==1)
		{
			ptr->pids[i]=0;
			proctab[i].pltype[lock]=0;
			proctab[i].pltime[lock]=0;
		}
	}
	ptr->ltype=0;
	ptr->lmaxprio=-1;
	ptr->lwcount=0;
	ptr->lrcount=0;
        restore(ps);
        return(OK);
}



