#include "types.h"
#include "pagesim.h"
#include "paging.h"
#include "swapops.h"
#include "stats.h"
#include "util.h"

pfn_t select_victim_frame(void);


/*  --------------------------------- PROBLEM 7 --------------------------------------
    Checkout PDF section 7 for this problem
    
    Make a free frame for the system to use.

    You will first call the page replacement algorithm to identify an
    "available" frame in the system.

    In some cases, the replacement algorithm will return a frame that
    is in use by another page mapping. In these cases, you must "evict"
    the frame by using the frame table to find the original mapping and
    setting it to invalid. If the frame is dirty, write its data to swap!
 * ----------------------------------------------------------------------------------
 */
pfn_t free_frame(void) {
    pfn_t victim_pfn;

    /* Call your function to find a frame to use, either one that is
       unused or has been selected as a "victim" to take from another
       mapping. */
    victim_pfn = select_victim_frame();

    /*
     * If victim frame is currently mapped, we must evict it:
     *
     * 1) Look up the corresponding page table entry
     * 2) If the entry is dirty, write it to disk with swap_write()
     * 3) Mark the original page table entry as invalid
     *
     */
    if (frame_table[victim_pfn].mapped) {
        fte_t * ftEntry = (fte_t *)(frame_table + victim_pfn);

        pte_t *page_table = (pte_t*)(mem + (ftEntry -> process -> saved_ptbr) * PAGE_SIZE);
        pte_t *ptEntry = (pte_t *)(page_table + frame_table[victim_pfn].vpn);

        if (ptEntry -> dirty) {
            stats.writebacks++;
            swap_write(ptEntry, (mem + victim_pfn * PAGE_SIZE));//&mem[PAGE_SIZE * victim_pfn]);
            ptEntry -> dirty = 0;
        }
        ptEntry -> valid = 0;
        frame_table[victim_pfn].mapped = 0;
    }


    /* Return the pfn */
    return victim_pfn;
}



/*  --------------------------------- PROBLEM 9 --------------------------------------
    Checkout PDF section 7 and 9 for this problem

    Finds a free physical frame. If none are available, uses either a
    randomized or LRU algorithm to find a used frame for
    eviction.

    Return:
        The physical frame number of a free (or evictable) frame.

    HINTS: Use the global variables MEM_SIZE and PAGE_SIZE to calculate
    the number of entries in the frame table.
    ----------------------------------------------------------------------------------
*/
pfn_t select_victim_frame() {
    /* See if there are any free frames first */
    size_t num_entries = MEM_SIZE / PAGE_SIZE;
    for (size_t i = 0; i < num_entries; i++) {
        if (!frame_table[i].protected && !frame_table[i].mapped) {
            return i;
        }
    }

    if (replacement == RANDOM) {
        /* Play Russian Roulette to decide which frame to evict */
        pfn_t last_unprotected = NUM_FRAMES;
        for (pfn_t i = 0; i < num_entries; i++) {
            if (!frame_table[i].protected) {
                last_unprotected = i;
                if (prng_rand() % 2) {
                    return i;
                }
            }
        }
        /* If no victim found yet take the last unprotected frame
           seen */
        if (last_unprotected < NUM_FRAMES) {
            return last_unprotected;
        }
    } else if (replacement == LRU) {
        /* Implement an LRU algorithm here */
        // timestamp_t min = frame_table[0].timestamp;
        timestamp_t min = get_current_timestamp();
        pfn_t counter = 0;
        for (pfn_t i = 0; i < NUM_FRAMES; i++) {
            if (frame_table[i].timestamp < min) {
                if (!frame_table[i].protected) {
                    min = frame_table[i].timestamp;
                counter = i;
                }
            }
        } 
        if (counter > 0) {
            return counter;
        }
        // return counter;
    }

    /* If every frame is protected, give up. This should never happen
       on the traces we provide you. */
    panic("System ran out of memory\n");
    exit(1);
}
