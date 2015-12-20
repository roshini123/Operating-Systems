/* vfreemem.c - vfreemem */

#include <conf.h>
#include <kernel.h>
#include <mem.h>
#include <proc.h>
#include <paging.h>

extern struct pentry proctab[];
/*------------------------------------------------------------------------
 *  vfreemem  --  free a virtual memory block, returning it to vmemlist
 *------------------------------------------------------------------------
 */
SYSCALL vfreemem(struct mblock *block, unsigned int size) {
	/*
	 * Referred freemem.c
	 */

	STATWORD ps;

	disable(ps);

	if (size == 0 || block < 4096 * NBPG)
		return SYSERR;

	size = roundmb(size);
	struct pentry *temp = &proctab[currpid];
	struct mblock *list = &temp->vmemlist;
	struct mblock *prev = list;
	struct mblock *next = prev->mnext;

	while (next != NULL && next < block && list!=NULL) {
		prev = next;
		next = next->mnext;
	}

	block->mnext = next;
	block->mlen = size;
	prev->mnext = block;

	return (OK);
}
