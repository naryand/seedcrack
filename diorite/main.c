#include "../random.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define THREADS 24
#define COUNT 36

typedef struct ore { int chunk_x; int chunk_z; int num; } ore;

// num = first ore y
ore arr[COUNT] = {{0,0,28},{0,1,26},{0,2,9},{0,3,5},{0,4,7},{0,5,16},
                  {1,0,29},{1,1,26},{1,2,31},{1,3,28},{1,4,10},{1,5,9},
                  {2,0,22},{2,1,15},{2,2,22},{2,3,4},{2,4,23},{2,5,23},
                  {3,0,6},{3,1,15},{3,2,30},{3,3,31},{3,4,26},{3,5,20},
                  {4,0,17},{4,1,27},{4,2,18},{4,3,31},{4,4,8},{4,5,7},
                  {5,0,30},{5,1,26},{5,2,12},{5,3,25},{5,4,22},{5,5,18}};

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


// TODO: compare parity of lower n bits when calling next_int_bound for x,z before y
void parity() {
    u64 seed[1];
    // u64 world = 14088749458738167797UL;
    u64 world = (1L<<48)-1;

    int counter;
    for(int n = 1; n <= 48; n++, counter = 0) {
        for(int i = 0; i < 6; i++) {
            for(int j = 0; j < 6; j++) {
                set_decorator_seed(seed, world, i*16, j*16, 14, 6); 
                next_int_bound(seed, 6); next_int_bound(seed, 16); next_int_bound(seed, 16);
                int y = next_int_bound(seed, 28)+4;
                next_int_bound(seed, 6); next_int_bound(seed, 16); next_int_bound(seed, 16);
                y = next_int_bound(seed, 28)+4;
                set_decorator_seed(seed, (world & ((1L<<n)-1)), i*16, j*16, 14, 6); 
                next_int_bound(seed, 6); next_int_bound(seed, 16); next_int_bound(seed, 16);
                int y_n = next_int_bound(seed, 28)+4;
                next_int_bound(seed, 6); next_int_bound(seed, 16); next_int_bound(seed, 16);
                y_n = next_int_bound(seed, 28)+4;
                if(y%4 == y_n%4) { counter++; }
            }
        }
        if(counter != 36) { printf("lower %d bits no parity\n", n); }
    }

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

int main(int argc, char const *argv[]) {
    u64 seed[1];
    u64 world = 14088749458738167797UL;
    int chunk_x = strtol(argv[1], NULL, 10);
    int chunk_z = strtol(argv[2], NULL, 10);
    set_decorator_seed(seed, world, chunk_x*16, chunk_z*16, 8, 6);
    for(int i = 0; i < 8; i++) {
        int x = next_int_bound(seed, 16);
        int z = next_int_bound(seed, 16);
        int y = next_int_bound(seed, 16);
        printf("%d %d %d\n", x+chunk_x*16, y, z+chunk_z*16);
    }
}
