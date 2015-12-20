#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t store, unsigned int npages) {

	STATWORD ps;
	if (check_store(store) || Page_check(npages)) {
		return SYSERR;
	}

	disable(ps);
	if ((bsm_tab[store].bs_private == HEAP_VALUE) && (bsm_tab[store].bs_status == MAPPED)) {
		restore(ps);
		return SYSERR;
	}

	if (bsm_tab[store].bs_status == UNMAPPED) {
		if (bsm_map(currpid, 4096, store, npages) == SYSERR) {
			restore(ps);
			return SYSERR;
		}
		restore(ps);
		return npages;
	} else {
		restore(ps);
		return bsm_tab[store].bs_npages;
	}
}

