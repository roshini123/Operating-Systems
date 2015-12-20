
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <math.h>
#include <lab1.h>
#include <stdio.h>
extern unsigned long ctr1000;
unsigned long currSP;   /* REAL sp of current process */
int scheduleclass;
void setschedclass(int schedclass)
{
        scheduleclass=schedclass;
}

int getschedclass()
{
        return scheduleclass;
}


extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:       Upon entry, currpid gives current process id.
 *      Proctab[currpid].pstate gives correct NEXT state for
 *      current process if other than PRREADY.
 *------------------------------------------------------------------------
 */

int getgoodness(int ppid)
{
        int goodness=-1;

        if(proctab[ppid].pcounter<=0)
            goodness = 0;
        else 
            goodness = proctab[ppid].pcounter+proctab[ppid].pprio;
        
        return goodness;
}
int count=0;
int nepoch=0;
int resched()
{
	register struct     pentry  *optr;  /* pointer to old process entry */
	register struct     pentry  *nptr;  /* pointer to new process entry */

        int next_ppid  = 0;
        int isnewepoch = 1;
        int preempt_value=0;

                	int i = 0;
		optr = &proctab[currpid];                
              int max = 0, goodness;

//				if(count<4)
//		                kprintf("npoch: %d,%d,%d\n",nepoch,preempt_value,optr->pcounter);
		 if(optr->pcounter>10)
                {
                        preempt_value=QUANTUM;
                }
                else if(optr->pcounter<=10 && optr->pcounter>0)
                {
                        preempt_value=optr->pcounter;
                }
                else
                {
                        preempt_value=0;
                }
		if(count==0)
		preempt=0;
		count++;	
		 nepoch =nepoch -(preempt_value-preempt);

                proctab[currpid].pcounter = proctab[currpid].pcounter-(preempt_value-preempt);

		if(proctab[currpid].pcounter<=0)
	        proctab[currpid].pcounter=0;

                //check if process is in new epoch
            	for(i = 1;i < NPROC; i++) {
                	if(proctab[i].pstate == PRCURR || proctab[i].pstate == PRREADY){
                        	if(getgoodness(i) > 0)
                                	isnewepoch = 0;
                    	}
           	}
		if(nepoch<=0)
		{
			nepoch=0;
			isnewepoch=1;	
//			kprintf("%d\n",optr->pcounter);
		}
                if(nepoch==0) {
			//	kprintf("\nepoch:%d\n",nepoch); 
                        nepoch=0;
	        	// set counter, quantum and epoch for the processes in the current epoch
                	for(i=1;i<NPROC;i++) {          
                	       	proctab[i].pprio = proctab[i].newprio;
                                proctab[i].pquantum = floor(proctab[i].pcounter/2) + proctab[i].pprio;
				proctab[i].pcounter = proctab[i].pquantum;
                                nepoch = nepoch + proctab[i].pquantum;              
                	}
                } 
                
		for(i = 0;i < NPROC;i++) {
                	if(proctab[i].pstate == PRCURR || proctab[i].pstate == PRREADY) {
                            	
				goodness = getgoodness(i);
                            	if(goodness > max) {
                                    	max = goodness;
                                    	next_ppid = i;
                            	}
                    	}
           	}
		 if(max==getgoodness(currpid) && currpid!=next_ppid)
                {
                        if(proctab[currpid].pprio>=proctab[next_ppid].pprio)
                        next_ppid=currpid;
                }


                
//		kprintf("\nthe pre value is : %d\n",preempt_value);        
                // current and next process are the same set the preempt to counter
 		if( currpid==next_ppid)
		{
			preempt=QUANTUM;
			return (OK);
		}             
		else 
		{

	                if (optr->pstate == PRCURR) {
                        optr->pstate = PRREADY;
                        insert(currpid,rdyhead,optr->pprio);
                }
                        //defining old and new process entry pointers
                        nptr = &proctab[next_ppid];
                        if (optr->pstate == PRCURR)
                        optr->pstate = PRREADY;
                        currpid = next_ppid;
                        nptr->pstate = PRCURR;
                        
                        nptr = &proctab[ (currpid = dequeue(next_ppid)) ];
			preempt=QUANTUM;			

			#ifdef  RTCLOCK
                        preempt = QUANTUM;              // reset preemption counter
                        #endif

                        ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                        return(OK);
}	
}	
