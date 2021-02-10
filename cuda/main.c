// test CPU diamond ore cracker
#include "../random.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define THREADS 24
#define COUNT 4

typedef struct ore { char x; char z; char y; } ore;
typedef struct block { char id; int found; u64 world_seed; } block;

ore arr[COUNT] = {{7,3,4},
                  {5,7,15},
                  {6,5,12},
                  {4,10,13}};

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

void *find(void *arg) {
    block *info = (block *)arg; u64 seed[1]; char matches;
    u64 range = (1L << 48)/THREADS;
    for(u64 i = info->id*range; i < (info->id+1)*range; i++, matches = 0) {
        for(int j = 0; j < COUNT; j++) {
            set_decorator_seed(seed, i, ((j&2) >> 1) << 4, (j&1) << 4, 9, 6);
            char x = next(seed, 4);
            char z = next(seed, 4);
            char y = next(seed, 4);
            if(x != arr[j].x) { break; }
            if(z != arr[j].z) { break; }
            if(y == arr[j].y) { matches++; } else { break; }
        }
        if(matches == COUNT) { info->found++; info->world_seed = i; break; }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    u64 seed[1];
    // int chunk_x = strtol(argv[1], NULL, 10);
    // int chunk_z = strtol(argv[2], NULL, 10);
    u64 world_seed = strtoull(argv[3], NULL, 10);
    printf("%lld\n", world_seed);
    // set_decorator_seed(seed, world_seed, chunk_x*16, chunk_z*16, 9, 6);
    // char x = next(seed, 4);
    // char z = next(seed, 4);
    // char y = next(seed, 4);
    // printf("{%d,%d, %d,%d,%d},\n", chunk_x,chunk_z, x, y, z);
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            set_decorator_seed(seed, world_seed, i<<4, j<<4, 9, 6);
            char x = next(seed, 4);
            char z = next(seed, 4);
            char y = next(seed, 4);
            printf("{%d,%d, %d,%d,%d},\n", i,j, x, y, z);
        }
    }

    // block info[THREADS] = {{0,0,0}}; 
    // pthread_t threads[THREADS];
    // for(int i = 0; i < 1; i++) {
    //     info[i].id = i;
    //     pthread_create(threads+i, NULL, find, info+i);
    // }
    // int found = 0;
    // for(int i = 0; i < 1; i++) {
    //     pthread_join(threads[i], NULL);
    //     found += info[i].found;
    //     if(info[i].world_seed) { seed[0] = info[i].world_seed; }
    // }
    // if(found == 1) { printf("found seed: %llu\n", seed[0]); } 
    // else if(found > 1) { puts("multiple seeds found"); return 0; } 
    // else { puts("no seed found"); return 0; }

}