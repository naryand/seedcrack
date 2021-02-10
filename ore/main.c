#include "../random.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define THREADS 24
#define COUNT 30

typedef struct ore { int chunk_x; int chunk_z; int num; } ore;

// num = bound
ore arr[COUNT] = {{0,0,5},{-4,-7,7},{-4,-8,4},
                  {-5,-8,7},{-6,-8,7},{-6,-7,7},
                  {-5,-9,8},{-5,-10,5},{-4,-10,4},
                  {-6,-10,3},{-7,-10,6},{-7,-11,7},
                  {1,1,5},{1,2,4},{1,3,8},
                  {2,1,4},{2,2,8},{2,3,6},
                  {3,1,8},{3,2,3},{3,3,8},
                  {4,1,7},{4,2,7},{4,3,3},
                  {5,1,5},{5,2,5},{5,3,4},
                  {6,1,7},{6,2,7},{6,3,8}};


typedef struct block { char id; int found; int lower30; } block;
u64 world_seed;

// ores step 6, diamond index 9, emerald index 14
void set_decorator_seed(u64 *seed, u64 world_seed, int x, int z, char index, char step) {
    set_seed(seed, world_seed);
    u64 a = next_long(seed) | 1L;
    u64 b = next_long(seed) | 1L;
    // ab[0] = a; ab[1] = b;
    u64 population_seed = (((u64) x)*a+((u64) z)*b)^world_seed;
    u64 decorator_seed = population_seed + (index + (step*10000)); // bottom max 30 bits affects parity
    set_seed(seed, decorator_seed);
}

// find bottom 30 bits using mod 2 on decorator
void *thread30(void *arg) {
    block *info = (block *)arg; u64 seed[1]; char matches;
    int range = (1L << 30)/THREADS;
    for(int i = info->id*range; i < (info->id+1)*range; i++, matches = 0) {
        for(char j = 0; j < COUNT; j++) {
            set_decorator_seed(seed, i, arr[j].chunk_x*16, arr[j].chunk_z*16, 14, 6);
            char bound = next_int_bound(seed, 6)+3;
            if(bound%2 == arr[j].num%2) { matches++; } else { break; }
        }
        if(matches == COUNT) { info->found++; info->lower30 = i; }
    }
}

void *findupper18(void *arg) {
    block *info = (block *)arg; u64 seed[1]; char matches;
    for(u64 i = 0; i < (1L << 18); i++, matches = 0) {
        u64 set = (i<<30) | info->lower30;
        for(char j = 0; j < COUNT; j++) {
            set_decorator_seed(seed, set, arr[j].chunk_x*16, arr[j].chunk_z*16, 14, 6);
            char bound = next_int_bound(seed, 6)+3;
            if(bound == arr[j].num) { matches++; } else { break; }
        }
        if(matches == COUNT) { info->found++; world_seed = set; }
    }
}


int main(int argc, char *argv[]) {
    block info[THREADS] = {0,0,0}; 
    pthread_t threads[THREADS];
    for(int i = 0; i < THREADS; i++) {
        info[i].id = i;
        pthread_create(threads+i, NULL, thread30, info+i);
    }

    int lower30;
    int found = 0;
    for(int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
        found += info[i].found;
        if(info[i].lower30) { lower30 = info[i].lower30; }
    }
    if(found == 1) { printf("found lower 30: %u\n", lower30); } 
    else if(found > 1) { puts("multiple lower 30 found"); return 0; } 
    else { puts("no lower 30 found"); return 0; }

    block info18 = {0,0,lower30};
    findupper18(&info18);
    if(info18.found == 1) { printf("found seed: %llu\n", world_seed); } 
    else if(info18.found > 1) { puts("multiple seeds found"); return 0; } 
    else { puts("no seeds found"); return 0; }
}