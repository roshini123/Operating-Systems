/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>
bs_map_t bsm_tab[BS_NUM];
void bs_init(int);
/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL init_bsm() {

	int i;
	for (i = 0; i < BS_NUM; i++) {
		bs_init(i);
	}
	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm() {

	int i;
	for (i = 0; i < BS_NUM; i++) {
		if (bsm_tab[i].bs_status == UNMAPPED)
		return i;
	}
	return -1;

}

/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i) {
	if ( check_store(i)|| bsm_tab[i].bs_refCount != 0) {
		return SYSERR;
	}

	bs_init(i);
	return OK;

}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth) {

	unsigned int i;
	unsigned int vpno = (vaddr / NBPG) & 0x000fffff;

	for (i = 0; i < BS_NUM; i++) {
		bs_map_t *temp = &proctab[pid].bsmap[i];
		if ((temp->bs_status == MAPPED) && (temp->bs_vpno <= vpno) && (temp->bs_vpno + temp->bs_npages > vpno)) {
			*store = i;
			*pageth = vpno - temp->bs_vpno;
			return OK;
		}
	}
	*store = -1;
	*pageth = -1;
	return SYSERR;
}

/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages) {

	if (vpno < NBPG || check_store(source) || Page_check(npages)) {
		return SYSERR;
	}

	if (proctab[pid].bsmap[source].bs_status == UNMAPPED)
		bsm_tab[source].bs_refCount++;

	proctab[pid].bsmap[source].bs_private = 0;
	proctab[pid].bsmap[source].bs_npages = npages;
	proctab[pid].bsmap[source].bs_pid = pid;
	proctab[pid].bsmap[source].bs_refCount = 0;
	proctab[pid].bsmap[source].bs_sem = -1;
	proctab[pid].bsmap[source].bs_status = MAPPED;
	proctab[pid].bsmap[source].bs_vpno = vpno;
	
	if (bsm_tab[source].bs_status == UNMAPPED) {
		bsm_tab[source].bs_status = MAPPED;
		bsm_tab[source].bs_npages = npages;
	}

	return OK;

}

/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag) {

	if (vpno < NBPG) {
		return SYSERR;
	}

	int store, pageth;
	int status = bsm_lookup(pid, (vpno * NBPG), &store, &pageth);
	if (status == -1 || (store == -1) || (pageth == -1)) {
		return SYSERR;
	}


	if (flag == HEAP_VALUE) {
		proctab[pid].vmemlist = NULL;
	} else {
		int t_vpno = vpno;
		while (proctab[pid].bsmap[store].bs_npages + proctab[pid].bsmap[store].bs_vpno > t_vpno) {
			unsigned int virt_Addr = t_vpno * NBPG;

			pd_t *pageDir_Entry = proctab[pid].pdbr + (virt_Addr >> 22) * sizeof(pd_t);
			pt_t *pageTable_Entry = (pageDir_Entry->pd_base) * NBPG + sizeof(pt_t) * ((virt_Addr >> 12) & 0x000003ff);

			if (frm_tab[pageTable_Entry->pt_base - FRAME0].fr_status == MAPPED)
				free_frm(pageTable_Entry->pt_base - FRAME0);

			if (pageDir_Entry->pd_pres == 0)
				break;
			t_vpno++;
		}
	}

	proctab[pid].bsmap[store].bs_status = UNMAPPED;
	proctab[pid].bsmap[store].bs_vpno = -1;
	proctab[pid].bsmap[store].bs_private = 0;
	proctab[pid].bsmap[store].bs_npages = 0;
	proctab[pid].bsmap[store].bs_pid = -1;
	proctab[pid].bsmap[store].bs_refCount = 0;
	proctab[pid].bsmap[store].bs_sem = -1;
	bsm_tab[store].bs_refCount--;
	return OK;
	int r;
	for(r=0;r<4;r++);
}

void bs_init(int i){
	bsm_tab[i].bs_status = UNMAPPED;
	bsm_tab[i].bs_vpno = -1;
	bsm_tab[i].bs_private = 0;
	bsm_tab[i].bs_npages = 0;
	bsm_tab[i].bs_pid = -1;
	bsm_tab[i].bs_refCount = 0;
	bsm_tab[i].bs_sem = -1;
}

