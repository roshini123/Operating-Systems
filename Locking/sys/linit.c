#include<kernel.h>
#include<proc.h>
#include<q.h>
#include<lock.h>
int nextlock;
struct lentry locktab[NLOCKS];
void linit()
{
	int i,j;
	struct lentry *ptr;
	nextlock=NLOCKS-1;
	for(i=0;i<NLOCKS;i++)
	{		
		ptr=&locktab[i];
		ptr->lstate=LFREE;
		ptr->lqtail=0;
		ptr->lmaxprio=-1;
		ptr->lrcount=0;
		ptr->lwcount=0;
		ptr->ltype=0;
		for(j=0;j<NPROC;j++)
		{
			ptr->pids[j]=0;
			proctab[i].pltype[j]=0;
			proctab[i].pltime[j]=0;
			ptr->qu[j].qkey=-1;
			ptr->qu[j].procid=-1;
			proctab[i].pctime[j]=0;
		}
		ptr->lctime=0;
		proctab[i].plockret=OK;
	}
}


