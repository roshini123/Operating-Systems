#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <mark.h>
#include <bufpool.h>
#include <paging.h>

int write_bs(char *src, bsd_t store, int page) {

	if (check_store(store) || Page_check(page)) {
		return SYSERR;
	}

	char * phy_addr = BACKING_STORE_BASE + (store << 19) + (page * NBPG);
	bcopy((void*) src, phy_addr, NBPG);
	return OK;
}

