/* vcreate.c - vcreate */

#include <conf.h>
#include <i386.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <paging.h>

/*
 static unsigned long esp;
 */

LOCAL newpid();
/*------------------------------------------------------------------------
 *  create  -  create a process to start running a procedure
 *------------------------------------------------------------------------
 */
SYSCALL vcreate(procaddr, ssize, npages, priority, name, nargs, args)
	int *procaddr; /* procedure address		*/
	int ssize; /* stack size in words		*/
	int npages; /* virtual heap size in pages	*/
	int priority; /* process priority > 0		*/
	char *name; /* name (for debugging)		*/
	int nargs; /* number of args that follow	*/
	long args; /* arguments (treated like an	*/
/* array in the code)		*/
{
	STATWORD ps;
	disable(ps);

	int source = 0;
	source = get_bsm();

	if ((source == -1) || Page_check(npages)) {
		restore(ps);
		return SYSERR;
	}

	int pid = create(procaddr, ssize, priority, name, nargs, args);
	if (bsm_map(pid, 4096, source, npages) == SYSERR) {
		restore(ps);
		return SYSERR;
	}
	
	proctab[pid].store = source;
	proctab[pid].vhpno = 4096;
	proctab[pid].vhpnpages = npages;
	proctab[pid].bsmap[source].bs_private = HEAP_VALUE;

	proctab[pid].vmemlist = getmem(sizeof(struct mblock));
	proctab[pid].vmemlist->mlen = npages * NBPG;
	proctab[pid].vmemlist->mnext = NULL;

	bsm_tab[source].bs_private = HEAP_VALUE;
	bsm_tab[source].bs_refCount = 1;

	restore(ps);
	return pid;
	
	int r;
	for(r=0;r<4;r++);
}

/*------------------------------------------------------------------------
 * newpid  --  obtain a new (free) process id
 *------------------------------------------------------------------------
 */
LOCAL newpid() {
	int pid; /* process id to return		*/
	int i;

	for (i = 0; i < NPROC; i++) { /* check all NPROC slots	*/
		if ((pid = nextproc--) <= 0)
			nextproc = NPROC - 1;
		if (proctab[pid].pstate == PRFREE)
			return (pid);
	}
	return (SYSERR);
}
