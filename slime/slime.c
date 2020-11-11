#include <stdio.h>
#include "../random.h"
#define COUNT 22

// sets internal seed based on world seed and chunk coordinates
void set_slime_seed(u64 *seed, u64 set, int j) {
    // 2d array of slime chunk coordinates
    int slimes[COUNT][2] = {{10,-9},{11,-9},{9,-8},{10,-8},{7,6},{7,7},{7,8},{6,8},
    {-19,-1},{-19,0},{-20,0},{-21,0},{-14,-23},{-14,-22},{-14,-21},{-14,-20},
    {-13,-22},{-13,-21},{0,6},{-11,19},{-12,19},{-12,20}}; //4864391782977103771

    // set slime seed for that chunk
    set_seed(seed, set +
        (int) (slimes[j][0] * slimes[j][0] * 0x4c1906) +
        (int) (slimes[j][0] * 0x5ac0db) +
        (int) (slimes[j][1] * slimes[j][1]) * 0x4307a7 +
        (int) (slimes[j][1] * 0x5f24f) ^ 0x3ad8025f
    );
}

// struct for managing each worker thread
typedef struct block { 
    int found; int lower18;
    u64 start; 
    u64 end; 
} block;

// gets the lower 18 bits of the world seed using a mod 2 exploit
void *findlower18(void *arg) {
    // init variables
    block *range = (block *)arg;
    u64 seed[1];
    int next, matches = 0;

    // loop thru bottom 18 bits
    for(int i = 0; i < 262144; i++) {
        for(int j = 0; j < COUNT; j++) {
            // find 18 bits where all next are even
            // must be even for next == 0 in thread30
            set_slime_seed(seed, i, j);
            next = next_int_bound(seed, 10);
            if(next % 2 == 0) { matches++; } else { break; }
        }
        if(matches == COUNT) { 
            printf("%d\n", i); 
            range->lower18 = i; 
            (range->found)++; 
        } matches = 0;
    }
    if(range->found > 1) { puts("found multiple lower 18 bits"); }
}

// finds the remaining 30 bits using the mod 10
void *thread30(void *arg) {
    // init variables
    block *range = (block *)arg;
    u64 seed[1];
    int next, matches = 0;
    u64 set = 0;

    // loop thru middle 30 bits
    for(u64 i = range->start; i < range->end; i++) {
        set = (i<<18) | range->lower18;
        for(int j = 0; j < COUNT; j++) {
            // find 48 bits where all next == 0
            // aka where all chunks given are slime chunks
            set_slime_seed(seed, set, j);
            next = next_int_bound(seed, 10);
            if(next == 0) { matches++; } else { break; }
        }
        if(matches == COUNT) { (range->found)++; } matches = 0;
    }
}