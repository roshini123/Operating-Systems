#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <proc.h>
#include<lock.h>
int lock (int ldes1, int type, int priority)
{
	int lock,wait=0;
	STATWORD ps;
	struct lentry *lptr;
	struct pentry *pptr;
	disable(ps);

	lock=ldes1;
	lptr=&locktab[lock];
	if(isbadlock(lock) || locktab[lock].lstate==LFREE)
	{	
		restore(ps);
		return SYSERR;	
	}
	if(proctab[currpid].pctime[lock]<lptr->lctime && proctab[currpid].pctime[lock]!=0)
	{
		restore(ps);
		return SYSERR;
	}
	int temp;
	if(lptr->lrcount!=0 && lptr->lwcount==0 && type==READ)
        {
		temp=lptr->lqtail-1;
		while(temp>=0)
		{
			if(lptr->qu[temp].qkey>=priority)
			{
				wait=1;
			}
			temp--;
		}
	}
	else if((lptr->lrcount==0 && lptr->lwcount!=0) || (lptr->lrcount!=0 &&lptr->lwcount==0 && type==WRITE))
	{
		wait=1;
	}
	pptr = &proctab[currpid];
	if(wait)
	{
        	pptr->pstate = PRLOCK;
		pptr->plockret=OK;
		
		lptr->qu[lptr->lqtail].procid=currpid;		
		lptr->qu[lptr->lqtail].qkey=priority;
		lptr->lqtail++;

		pptr->pltime[ldes1]=ctr1000;//clear these after lock is assigned
		pptr->pltype[ldes1]=type;
		while(lptr->pids[currpid]==0)
		{
		restore(ps);
		sleep1000(1);
		if(pptr->plockret==DELETED)
		{
			return DELETED;					
		}
		disable(ps);
		}
	}
		
			lptr->lmaxprio=priority;
			lptr->lstate=LUSED;
			lptr->ltype=type;	
			lptr->pids[currpid]=1;
			pptr->pltype[lock]=type;
			pptr->pltime[lock]=ctr1000;
		
		if(type==READ)
		{
			lptr->lrcount++;
		}
		else if(type==WRITE)
		{
			lptr->lwcount++;
		}
	
	restore(ps);
	return OK;
}



