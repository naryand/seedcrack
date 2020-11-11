#include "../random.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define THREADS 8
#define COUNT 30

typedef struct ore { int chunk_x; int chunk_z; int num; } ore;

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

// ore arr[COUNT] = {{0,0,28},{0,1,26},{0,2,9},{0,3,5},{0,4,7},{0,5,16},
//                   {1,0,29},{1,1,26},{1,2,31},{1,3,28},{1,4,10},{1,5,9},
//                   {2,0,22},{2,1,15},{2,2,22},{2,3,4},{2,4,23},{2,5,23},
//                   {3,0,6},{3,1,15},{3,2,30},{3,3,31},{3,4,26},{3,5,20},
//                   {4,0,17},{4,1,27},{4,2,18},{4,3,31},{4,4,8},{4,5,7},
//                   {5,0,30},{5,1,26},{5,2,12},{5,3,25},{5,4,22},{5,5,18}};
// u64 ab[2] = {0};

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
    if(found == 1) { printf("found lower 30: %u\n", lower30); } else if(found > 1) { puts("multiple lower 30 found"); return 0; } else { puts("no lower 30 found"); return 0; }

    block info18 = {0,0,lower30};
    findupper18(&info18);
    if(info18.found == 1) { printf("found seed: %llu\n", world_seed); } else if(info18.found > 1) { puts("multiple seeds found"); return 0; } else { puts("no seeds found"); return 0; }

    // u64 seed[1];
    // u64 world = 14088749458738167797UL;
    // for(int i = 0; i < 6; i++, printf("\n")) {
    //     printf("{");
    //     for(int j = 0; j < 6; j++) {
    //         set_decorator_seed(seed, world, i*16, j*16, 14, 6); next_int_bound(seed, 6); next_int_bound(seed, 16); next_int_bound(seed, 16);
    //         int y = next_int_bound(seed, 28)+4;
    //         printf("{%d,%d,%d},", i,j,y);
    //     }
    //     printf("}");
    // }
    // return 0;

    // u64 world_seed = 14088749458738167797UL; //strtoull(argv[3], NULL, 10);
    // bottom 30 bits of seed affects # of decorator bottom bits that affect parity
    // 0xc3854afcc190bff5, 14088749458738167797, -4357994614971383819
    // c190bff5 -> 11|00 0001 ... 30-5=25 lower 25 affect parity
    // int chunk_x = strtol(argv[1], NULL, 10);
    // int chunk_z = strtol(argv[2], NULL, 10);
    // set_decorator_seed(seed, world_seed, chunk_x*16, chunk_z*16, 14, 6);
    // printf("%d\n", next_int_bound(seed, 6)+3);
            // set_decorator_seed(seed, world_seed, arr[j].chunk_x*16, arr[j].chunk_z*16, 14, 6);
            // int full = next_int_bound(seed, 6)%2;
            // u64 a = ab[0]; u64 b = ab[1];
            // set_decorator_seed(seed, world_seed & ((1L<<chunk_x)-1), arr[j].chunk_x*16, arr[j].chunk_z*16, 14, 6);
            // int lower = next_int_bound(seed, 6)%2;
            // u64 c = ab[0]; u64 d = ab[1];
            // printf("%llx, %llx\n", a, b);
            // printf("%llx, %llx\n", c, d);
            // printf("%llx, %llx\n", ~(a^c), ~(b^d));
            // if(full != lower) { puts("fail"); break; }
}