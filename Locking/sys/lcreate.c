#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include<q.h>
#include<lock.h>
#include<stdio.h>
SYSCALL lcreate()
{
	STATWORD ps;
	disable(ps);
	int lock=newlock();
	locktab[lock].lctime=ctr1000;
	restore(ps);
	return lock;
}
LOCAL int newlock()
{
	int lock;
	int i;	
	for(i=(NLOCKS-1);i>=0;i--)
	{
		lock=nextlock;
		nextlock--;
		if(nextlock<0)
			nextlock=NLOCKS-1;
		if(locktab[lock].lstate==LFREE)
		{
			locktab[lock].lstate=LUSED;
			for(i=0;i<NPROC;i++)
			{
				proctab[i].pltype[lock]=0;
				locktab[lock].pids[i]=0;
				proctab[i].pltime[lock]=-1;
			}
			return lock;
		}
	}
	return SYSERR;
}
