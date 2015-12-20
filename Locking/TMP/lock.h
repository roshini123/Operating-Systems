#include<kernel.h>
#include<q.h>

#ifndef NLOCKS
#define NLOCKS          50      /* number of semaphores, if not defined */
#endif

#ifndef NPROC
#define NPROC           50      /* number of semaphores, if not defined */
#endif


#define LFREE   '\03'           /* this semaphore is free               */
#define LUSED   '\04'           /* this semaphore is used               */

#define READ 	'\05'
#define WRITE	'\09'

#ifndef DELETED
#define DELETED '-6'
#endif

struct  lentry  {               /* semaphore table entry                */
        char    lstate;         /* the state SFREE or SUSED             */
        int     lqtail;         /* q index of tail of list              */
	int 	lmaxprio;	/* curr maximum priority		*/
	int	pids[NPROC];	/* process ids of the processes holding this lock */
	int 	ltype;	/* curr lock being held		*/
	int 	lrcount;		/* curr reader count			*/
	int 	lwcount;		/* curr writer count	*/
	int	lctime;
	struct 	qent 	qu[50];
};
extern  struct  lentry  locktab[NLOCKS];
extern  int     nextlock;

#define isbadlock(s)     (s<0 || s>=NLOCKS)

extern unsigned long ctr1000;

extern int lcreate(void);
extern void linit(void);
extern int ldelete(int lock);
extern int lock (int ldes1, int type, int priority);
extern int releaseall(int, int, ...);

