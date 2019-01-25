#include "paging.h"
#include "stats.h"

/* The stats. See the definition in stats.h. */
stats_t stats;

/*  --------------------------------- PROBLEM 10 --------------------------------------
    Checkout PDF section 10 for this problem

    Calculate any remaining statistics to print out.

    You will need to include code to increment many of these stats in
    the functions you have written for other parts of the project.

    Use this function to calculate any remaining stats, such as the
    average access time (AAT).

    You may find the #defines in the stats.h file useful.
    -----------------------------------------------------------------------------------
*/
void compute_stats() {
	// uint64_t numOfAccessing = stats.accesses;
	// uint64_t dealingMem = numOfAccessing - stats.page_faults;
	// uint64_t totalTime = dealingMem * MEMORY_READ_TIME;
	// stats.aat = totalTime / numOfAccessing;
	// stats.aat = ((stats.writes + stats.reads - stats.page_faults) / MEMORY_READ_TIME);
	stats.aat = ((double)(MEMORY_READ_TIME * stats.accesses + DISK_PAGE_READ_TIME * stats.page_faults 
		+ DISK_PAGE_WRITE_TIME * stats.writebacks))/((double)(stats.accesses));
}
