/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <paging.h>

/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid) {
	STATWORD ps;
	struct pentry *pptr; /* points to proc. table for pid*/
	int dev;

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return (SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (!isbaddev(dev))
		close(dev);
	dev = pptr->pdevs[1];
	if (!isbaddev(dev))
		close(dev);
	dev = pptr->ppagedev;
	if (!isbaddev(dev))
		close(dev);

	int i;
	for (i = 0; i < BS_NUM; i++) {
		if (proctab[pid].bsmap[i].bs_status == MAPPED)
			bsm_unmap(pid, proctab[pid].bsmap[i].bs_vpno, proctab[pid].bsmap[i].bs_private);
	}
	free_frm(proctab[pid].pdbr/NBPG - FRAME0);

	send(pptr->pnxtkin, pid);
	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:
		pptr->pstate = PRFREE; /* suicide */
		resched();

	case PRWAIT:
		semaph[pptr->psem].semcnt++;

	case PRREADY:
		dequeue(pid);
		pptr->pstate = PRFREE;
		break;

	case PRSLEEP:
	case PRTRECV:
		unsleep(pid);
		/* fall through	*/
	default:
		pptr->pstate = PRFREE;
	}
	restore(ps);
	return (OK);
}