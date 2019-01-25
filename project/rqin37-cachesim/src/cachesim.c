#include "cachesim.h"

#define TRUE 1
#define FALSE 0

/**
 * The stuct that you may use to store the metadata for each block in
 * the cache
 */
typedef struct block {
    uint64_t tag;  // The tag stored in that block
    uint8_t valid; // Valid bit
    uint8_t dirty; // Dirty bit

    // Add any metadata here to perform LRU/FIFO replacement. For LRU,
    // think about how you could use a counter variable to track the
    // oldest block in a set. For FIFO, consider how you would
    // implement a queue with only a single pointer.

    // TODO
    uint64_t FIFO;
    uint64_t LRU;
    uint64_t LFU; // least frequency first -> COSTUM

} block_t;

/**
 * A struct for storing the configuration of the cache as passed in
 * the cache_init function.
 */
typedef struct config {
    uint64_t C;
    uint64_t B;
    uint64_t S;
    enum REPLACEMENT_POLICY policy;
} config_t;

// You may add global variables, structs, and function headers below
// this line if you need any

// TODO
typedef struct {
    block_t *blocks;
} cache_line;
cache_line *cache;
config_t config;

int totalLines;
int blocksEachLine;

uint64_t lru_counter = 0;
uint64_t fifo_counter = 1;


/**
 * Initializes your cache with the passed in arguments.
 *
 * @param C The total size of your cache is 2^C bytes
 * @param S The total number of blocks in a line/set of your cache are 2^S
 * @param B The size of your blocks is 2^B bytes
 * @param policy The replacement policy of your cache
 */
void cache_init(uint64_t C, uint64_t B, uint64_t S, enum REPLACEMENT_POLICY policy)
{
    // Initialize the cache here. We strongly suggest using arrays for
    // the cache meta data. The block_t struct given above may be
    // useful.

    // TODO
    // return NULL;
    totalLines = (1 << (C - B - S));
    cache = malloc((uint64_t) totalLines * sizeof(cache_line));
    if (cache == NULL) {
        return;
    }
    config.C = C;
    config.B = B;
    config.S = S;
    blocksEachLine = (1 << config.S);
    for (int i = 0; i < totalLines; i++) {
        cache[i].blocks = (block_t*) calloc((size_t) blocksEachLine, sizeof(block_t));
        if (cache[i].blocks == NULL) {
            return;
        }
    }
    config.policy = policy;
}

/**
 * Simulates one cache access at a time.
 *
 * @param rw The type of access, READ or WRITE
 * @param address The address that is being accessed
 * @param stats The struct that you are supposed to store the stats in
 * @return TRUE if the access is a hit, FALSE if not
 */
uint8_t cache_access(char rw, uint64_t address, cache_stats_t* stats)
{
    // Find the tag and index and check if it is a hit. If it is a
    // miss, then simulate getting the value from memory and putting
    // it in the cache. Make sure you track the stats along the way!

    uint8_t is_hit = FALSE;
    int blockIndex = 0;
    uint64_t tag = get_tag(address, config.C, config.B, config.S);
    uint64_t index = (address >> config.B) & (uint64_t) ((1 << (config.C - config.B - config.S)) - 1);

    for (int i = 0; i < blocksEachLine; i++) {
        if ((cache[index].blocks[i].tag == tag) && cache[index].blocks[i].valid) {
            is_hit = TRUE;
            if (rw == WRITE) {
                cache[index].blocks[i].dirty = TRUE; 
            }

            cache[index].blocks[i].LRU = lru_counter;
            lru_counter++;
            cache[index].blocks[i].LFU++;
        } 
    }

    if (rw == READ) {
        stats->reads++;
        stats->accesses++;
        if (!is_hit) {
            stats->read_misses++;
            stats->misses++;
        } 

    } else {
        stats->writes++;
        stats->accesses++;
        if (!is_hit) {
            stats->write_misses++;
            stats->misses++;
        } 
    }
    
    if (!is_hit) {
        if (!config.S == 0) {
            uint8_t isValid = FALSE;
            for (int i = 0; i < blocksEachLine; i++) {
                if (!cache[index].blocks[i].valid) {
                    blockIndex = i;
                    i = blocksEachLine;
                    isValid = TRUE;
                }
            }
            if (!isValid) {
                if (config.policy == LRU) {
                    uint64_t count = cache[index].blocks[0].LRU;
                    for (int i = 0; i < blocksEachLine; i++) {
                        if (cache[index].blocks[i].LRU < count) {
                            blockIndex = i;
                            count = cache[index].blocks[i].LRU;
                        }
                    }
                } else if (config.policy == FIFO) {
                    uint64_t count = cache[index].blocks[0].FIFO;
                    for (int i = 0; i < blocksEachLine; i++) {
                        if (cache[index].blocks[i].FIFO < count) {
                            blockIndex = i;
                            count = cache[index].blocks[i].FIFO;
                        }
                    }
                } else if (config.policy == CUSTOM) {
                    // CUSTOM policy is least frequently first
                    uint64_t min = cache[index].blocks[0].LFU;
                    for (int i = 0; i < blocksEachLine; i++) {
                        if (cache[index].blocks[i].LFU < min) {
                            min = cache[index].blocks[i].LFU;
                            blockIndex = i;
                        }
                    }
                }
            }
        }
        if (cache[index].blocks[blockIndex].valid && cache[index].blocks[blockIndex].dirty) {
            stats->write_backs++;
            cache[index].blocks[blockIndex].dirty = FALSE;
        }
        cache[index].blocks[blockIndex].LRU = lru_counter;
        lru_counter++;

        cache[index].blocks[blockIndex].FIFO = fifo_counter;
        fifo_counter++;

        cache[index].blocks[blockIndex].LFU = 0;

        cache[index].blocks[blockIndex].tag = tag;
        cache[index].blocks[blockIndex].valid = TRUE;
        if (rw == WRITE) {
            cache[index].blocks[blockIndex].dirty = TRUE;
        }
        return FALSE;
    } else {
        return TRUE;
    }
}

/**
 * Frees up memory and performs any final calculations before the
 * statistics are outputed by the driver
 */
void cache_cleanup(cache_stats_t* stats)
{
    // Make sure to free any malloc'd memory here. To check if you
    // have freed up the the memory, run valgrind. For more
    // information, google how to use valgrind.

    // TODO
    stats->misses = stats->read_misses+stats->write_misses;
    stats->miss_rate= (double)(stats->misses)/(stats->accesses);
    stats->avg_access_time= stats->cache_access_time + (stats->memory_access_time*stats->miss_rate);
    for (int i = 0; i < totalLines; i++) {
        free(cache[i].blocks);
    }   
    free(cache);
}

/**
 * Computes the tag of a given address based on the parameters passed
 * in
 *
 * @param address The address whose tag is to be computed
 * @param C The size of the cache (i.e. Size of cache is 2^C)
 * @param B The size of the cache block (i.e. Size of block is 2^B)
 * @param S The set associativity of the cache (i.e. Set-associativity is 2^S)
 * @return The computed tag
 */
uint64_t get_tag(uint64_t address, uint64_t C, uint64_t B, uint64_t S)
{
    uint64_t indexbit = C - B - S;
    return address >> (B + indexbit);
}

/**
 * Computes the index of a given address based on the parameters
 * passed in
 *
 * @param address The address whose tag is to be computed
 * @param C The size of the cache (i.e. Size of cache is 2^C)
 * @param B The size of the cache block (i.e. Size of block is 2^B)
 * @param S The set associativity of the cache (i.e. Set-associativity is 2^S)
 * @return The computed index
 */
uint64_t get_index(uint64_t address, uint64_t C, uint64_t B, uint64_t S)
{
    uint64_t indexbit = C - B - S;
    uint64_t tagbit = 64 - B - indexbit;
    return (address << tagbit) >> (tagbit + B);
}
