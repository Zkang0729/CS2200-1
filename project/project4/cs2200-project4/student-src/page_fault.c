#include "paging.h"
#include "pagesim.h"
#include "swapops.h"
#include "stats.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

/*  --------------------------------- PROBLEM 6 --------------------------------------
    Checkout PDF section 7 for this problem

    Page fault handler.

    When the CPU encounters an invalid address mapping in a page table,
    it invokes the OS via this handler.

    Your job is to put a mapping in place so that the translation can
    succeed. You can use free_frame() to make an available frame.
    Update the page table with the new frame, and don't forget
    to fill in the frame table.

    Lastly, you must fill your newly-mapped page with data. If the page
    has never mapped before, just zero the memory out. Otherwise, the
    data will have been swapped to the disk when the page was
    evicted. Call swap_read() to pull the data back in.

    HINTS:
         - You will need to use the global variable current_process when
           setting the frame table entry.

    ----------------------------------------------------------------------------------
 */
void page_fault(vaddr_t address) {
    /* First, split the faulting address and locate the page table entry.
       Remember to keep a pointer to the entry so you can modify it later. */
    stats.page_faults++;
    pte_t *ptfault = (pte_t *)(mem + PTBR * PAGE_SIZE);
    vpn_t VPN = vaddr_vpn(address);

    pte_t *ptfault_Entry = &ptfault[VPN];


    /* It's a page fault, so the entry obviously won't be valid. Grab
       a frame to use by calling free_frame(). */
    pfn_t newframe_num = free_frame();
    fte_t *ftEntry =  (fte_t *)(frame_table + newframe_num);//&(frame_table[newframe_num]);    

    /* Update the page table entry. Make sure you set any relevant values. */
    ptfault_Entry -> pfn = newframe_num;
    ptfault_Entry -> valid = 1;
    ptfault_Entry -> dirty = 0;

    /* Update the frame table. Make sure you set any relevant values. */
    ftEntry -> timestamp = get_current_timestamp();
    ftEntry -> vpn = VPN;
    ftEntry -> mapped = 1;
    ftEntry -> process = current_process;
    // ftEntry -> protected = 0;


    /* Initialize the page's memory. On a page fault, it is not enough
     * just to allocate a new frame. We must load in the old data from
     * disk into the frame. If there was no old data on disk, then
     * we need to clear out the memory (why?).
     *
     * 1) Get a pointer to the new frame in memory.
     * 2) If the page has swap set, then we need to load in data from memory

     *    using swap_read().
     * 3) Else, just zero the page's memory. If the page is later written
     *    back, swap_write() will automatically allocate a swap entry.
     */
    if (swap_exists(ptfault_Entry)) {
        // swap_read(ptfault_Entry, &mem[ptfault_Entry -> pfn * PAGE_SIZE]);
        swap_read(ptfault_Entry, &mem[newframe_num * PAGE_SIZE]);
    } else {
        // memset(&mem[ptfault_Entry -> pfn * PAGE_SIZE], 0, PAGE_SIZE);
        memset(&mem[newframe_num * PAGE_SIZE], 0, PAGE_SIZE);
    }
}

#pragma GCC diagnostic pop
