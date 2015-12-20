#include<conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <q.h>
#include<lock.h>
#include<math.h>
int releaseall(numlocks,ldes1)
int numlocks;
int ldes1;
{
	STATWORD ps;
	struct lentry *lptr;
	int *a;
	int i,j,k,cnt=0;
	disable(ps);
	a=(int*) (&ldes1)+(numlocks-1);
	for(i=0;i<(numlocks-1);i++)
	*a--;
	int lock,ret=OK;
	for(i=0;i<numlocks;i++)
	{
		lock=a[i];
		lptr=&locktab[lock];		

	        if (isbadlock(lock) || lptr->lstate==LFREE) {
           		continue;
        	}

		int tem=lptr->lqtail-1;
		while(tem>=0)
		{
			if(lptr->qu[tem].procid==currpid)
			{
				if(tem==0)
				{
					lptr->qu[tem].qkey=-1;
					lptr->qu[tem].procid=-1;		
				}
				else
				{
				for(j=tem;j<((lptr->lqtail)-1);j++)
				{
					lptr->qu[tem].qkey=lptr->qu[tem+1].qkey;
					lptr->qu[tem].procid=lptr->qu[tem+1].procid;
				}
				}
				lptr->lqtail--;
			}
			tem--;

		}

		if(lptr->pids[currpid]==0)
		{
			ret=SYSERR;
			continue; //remove from the queue for this lock
		}
		if(proctab[currpid].pltype[lock]==WRITE)
		{
			lptr->lwcount--;
		}
		else if(proctab[currpid].pltype[lock]==READ)
		{
			lptr->lrcount--;
		}

//lstate change if none in queue

		lptr->pids[currpid]=0;
		lptr->ltype=0;
		proctab[currpid].pltype[lock]=0;
		proctab[currpid].pltime[lock]=0;
		proctab[currpid].plockret=OK;
		if(lptr->lqtail==0)
		continue;
		struct qent *qtemp;
		int temp=lptr->lqtail-1;
	
		//sorting the queue of this lock based on priority
		if(temp>0)
		{	
		for(j=0;j<temp;j++)
		for(k=j+1;k<=temp;k++)
		{
			if(lptr->qu[j].qkey>lptr->qu[k].qkey)
			{
				qtemp->qkey=lptr->qu[j].qkey;
				qtemp->procid=lptr->qu[j].procid;
				
				lptr->qu[j].qkey=lptr->qu[k].qkey;
				lptr->qu[j].procid=lptr->qu[k].procid;

				lptr->qu[k].qkey=qtemp->qkey;
				lptr->qu[k].procid=qtemp->procid;
			}
		}
		}

		if(temp==0 || lptr->qu[temp].qkey!=lptr->qu[(temp-1)].qkey)
		{
			int tempid=lptr->qu[temp].procid;
			if(proctab[tempid].pltype[lock]==WRITE && lptr->lwcount==0 && lptr->lrcount==0)
                        {
                                lptr->ltype=WRITE;
                                lptr->pids[tempid]=1;
				lptr->lqtail--;
			}
			else if(proctab[tempid].pltype[lock]==READ && lptr->lwcount==0)
			{
				int temp1=temp;
				int maxprio=-1;
				while(temp1>=0)
				{
					if(lptr->qu[temp1].qkey>maxprio && proctab[lptr->qu[temp1].procid].pltype[lock]==WRITE)
					{
					maxprio=lptr->qu[temp1].qkey;
					}
					temp1--;
				}

				temp1=temp;
				while(temp1>=0)
				{
					if(lptr->qu[temp1].qkey>=maxprio && proctab[lptr->qu[temp1].procid].pltype[lock]==READ)
					{
						lptr->ltype=READ;
						tempid=lptr->qu[temp1].procid;
						lptr->pids[tempid]=1;
						lptr->qu[temp1].procid=-1;
		                                lptr->qu[temp1].qkey=-1;
                		                lptr->lqtail--;
					}		
					temp1--;			
				}
			}
		}
		else
		{
			int temp3=-111,option=0;
			temp=lptr->lqtail-1;
			int temp2=temp-1;
			int maxprio=lptr->qu[temp].qkey;


			while(lptr->qu[temp2].qkey==maxprio && temp2>=0)
			{
				if(proctab[(lptr->qu[temp].procid)].pltype[lock]==WRITE && proctab[(lptr->qu[temp].procid)].pltime[lock]<proctab[(lptr->qu[temp2].procid)].pltime[lock])
				{
					temp3=temp;
				}
				if(proctab[(lptr->qu[temp2].procid)].pltype[lock]==WRITE)
				{
					if(abs(proctab[(lptr->qu[temp].procid)].pltime[lock]-proctab[(lptr->qu[temp2].procid)].pltime[lock])<1000)
					{
						if(temp3==-111 || (proctab[(lptr->qu[temp2].procid)].pltime[lock]<proctab[(lptr->qu[temp3].procid)].pltime[lock]))
						{temp3=temp2;}
					}
				}
				temp2=temp2-1;
			}			
			if(temp3!=-111 &&abs(proctab[(lptr->qu[temp].procid)].pltime[lock]-proctab[(lptr->qu[temp3].procid)].pltime[lock])<1000)
			{
				option=1;
			}
			else if(temp3!=-111)
			{
				option=2;
			}
			else
			{
				option=3;
			}
			temp=lptr->lqtail-1;
			temp2=temp;
			if(option==1)
			{
				int tempid=lptr->qu[temp3].procid;
				lptr->ltype=WRITE;
				lptr->pids[tempid]=1;
				if(temp3==(lptr->lqtail-1))
				{
					lptr->qu[temp3].qkey=-1;
					lptr->qu[temp3].procid=-1;
				}
				else
				{
					while(temp3<(lptr->lqtail-1))
					{
						lptr->qu[temp3].qkey=lptr->qu[temp3+1].qkey;
						lptr->qu[temp3].procid=lptr->qu[temp3+1].procid;
						temp3++;
					}
				}
				lptr->lqtail--;
			}
			else
			{
				while(lptr->qu[temp2].qkey==maxprio && lptr->lwcount==0 && temp2>=0)
				{
					if(proctab[lptr->qu[temp2].procid].pltype[lock]==READ)
					{
						int tempid=lptr->qu[temp2].procid;
						lptr->ltype=READ;
						lptr->pids[tempid]=1;
						int temp4=temp2;
						if(temp4==(lptr->lqtail-1))
                                		{
                                        		lptr->qu[temp4].qkey=-1;
                                        		lptr->qu[temp4].procid=-1;
                                		}
                                		else
                                		{
                                        		while(temp4<(lptr->lqtail-1))
                                        		{
                                                		lptr->qu[temp4].qkey=lptr->qu[temp4+1].qkey;
                                                		lptr->qu[temp4].procid=lptr->qu[temp4+1].procid;
                                                		temp4++;
                                        		}
                                		}
                                		lptr->lqtail--;
					}
					temp2--;
				}		
			}
		}

	}
	restore(ps);
    	resched();
	return ret;
}



