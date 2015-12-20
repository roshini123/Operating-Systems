/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages) {
	/* sanity check ! */
	STATWORD ps;
	if ((virtpage < 4096) || (source < 0) || (source > MAX_ID) || (npages < 1) || (npages > 200)) {
		kprintf("\n xmmap: invalid parameters\n");
		return SYSERR;
	}

	disable(ps);

	bs_map_t *temp = &bsm_tab[source];
	if ((temp->bs_status == UNMAPPED) || (temp->bs_private == HEAP_VALUE)) {
		restore(ps);
		return OK;
	}

	if (npages > bsm_tab[source].bs_npages) {
		restore(ps);
		return SYSERR;
	}

	if (bsm_map(currpid, virtpage, source, npages) == SYSERR) {
		restore(ps);
		return SYSERR;
	}
	restore(ps);
	return OK;
	int r;
	for(r=0;r<4;r++);
}

/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage) {
	/* sanity check ! */
	STATWORD ps;
	if ((virtpage < 4096)) {
		kprintf("xmunmap: %d invalid virtual page\n", virtpage);
		return SYSERR;
	}

	disable(ps);
	if (bsm_unmap(currpid, virtpage, 0) == SYSERR) {
		restore(ps);
		return SYSERR;
	}
    int r;
	for(r=0;r<4;r++);
	write2CR3(currpid);
	restore(ps);
	return OK;
}
