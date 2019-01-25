Running perlbench, CUSTOM is faster

Here is the data:
==================================================================
Cache Settings
C: 15
B: 5
S: 3
Replacement policy: CUSTOM

Cache Statistics
Accesses: 507441
Reads: 302052
Read misses: 15701
Writes: 205389
Write misses: 6237
Misses: 21938
Writebacks: 7192
Access time: 3
Memory access time: 120
Miss rate: 0.043233
Average access time (AAT): 8.187913

Cache Settings
C: 15
B: 5
S: 3
Replacement policy: FIFO

Cache Statistics
Accesses: 507441
Reads: 302052
Read misses: 15713
Writes: 205389
Write misses: 8664
Misses: 24377
Writebacks: 11701
Access time: 3
Memory access time: 120
Miss rate: 0.048039
Average access time (AAT): 8.764690

Cache Settings
C: 15
B: 5
S: 3
Replacement policy: LRU

Cache Statistics
Accesses: 507441
Reads: 302052
Read misses: 14024
Writes: 205389
Write misses: 7979
Misses: 22003
Writebacks: 10213
Access time: 3
Memory access time: 120
Miss rate: 0.043361
Average access time (AAT): 8.203285
==================================================================

Instruction of how to run: ---------------------------------------
Bascially, since I add Least frequently into the cachesim.c
just like what I implement FIFO and lru, so that you can use
the exactly same command line to run the custom policy only
swithc FIFO or lru into CUSTOM.
------------------------------------------------------------------

A description of how your custom algorithm works:-----------------
What I was doing is called Least Frequently Used Policy. As I
implemented for LRU, I also track down the least recently used
cache line and do the replacement. Stead of LRU, LFU would have
to focus on which cache line is accessed most frequenctly. Say
there is a cahce line A which has a accessing frequency of 9, 
and another cache line B has a accessing frequency of 3. This will
let us know that in the future, A has the high probability to be
accessed than B. In this way, the replacement option will be B. 
It's just another way of approaching to predictation of future
cache access.
------------------------------------------------------------------

Which trace acts better and why:----------------------------------
For those traces which are more possible to access the same cache
lines all the time, they might have a better performancce of LFU
policy since the LFU will always keep those cache lines which are 
accessed frequently.