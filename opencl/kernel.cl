// unsigned long long typedef
typedef unsigned long long u64;
typedef struct ore { char chunk_x; char chunk_z; char x; char y; char z; } ore;

// sets internal seed
void set_seed(u64 *seed, u64 set) {
    *seed = set; *seed = (*seed ^ 0x5DEECE66DL) & ((1L << 48) - 1);
}

// gets the top n bits off the internal seed
int next(u64 *seed, int n) {
    *seed = (*seed * 0x5DEECE66DL + 0xBL) & ((1L << 48) - 1);
    return (int) (*seed >> (48 - n));
}

// returns a pseudorandom int given a seed
int next_int(u64 *seed) { return next(seed, 32); }

// returns bounded pseudorandom int given a seed and bound n
int next_int_bound(u64 *seed, int n) {
    if(n <= 0) { return -1; }
    if((n & -n) == n) { return (int) ((n * (long) next(seed, 31)) >> 31); } // i.e., n is next power of 2

    int bits, val;
    do { 
        bits = next(seed, 31); 
        val = bits % n; 
    } while(bits - val + (n - 1) < 0);
    return val;
}

// returns pseudorandom long long given a seed
u64 next_long(u64 *seed) { return (((long long) next(seed, 32)) << 32) + (long long) next(seed, 32); }

// ores step 6, diamond index 9, emerald index 14
void set_decorator_seed(u64 *seed, u64 world_seed, int x, int z, char index, char step) {
    set_seed(seed, world_seed);
    u64 a = next_long(seed) | 1L;
    u64 b = next_long(seed) | 1L;
    u64 population_seed = (((u64) x)*a+((u64) z)*b)^world_seed;
    u64 decorator_seed = population_seed + (index + (step*10000)); // bottom max 30 bits affects parity
    set_seed(seed, decorator_seed);
}

#define COUNT 4
__constant ore arr[COUNT] = {{0,0, 1,2,4},
                             {0,1, 8,6,15},
                             {0,2, 2,2,6},
                             {1,0, 1,8,5}};

__kernel void find(int call) {
    u64 seed[1];
    // calculate attempt
    // set i to (ITEMS per GROUP)*(GROUPS per CALL)*(CALL number)
    u64 i = get_local_size(0)*get_num_groups(0)*call;
    // add on (ITEMS per group)*(GROUP number)+(ITEM number)
    i = i+get_group_id(0)*get_local_size(0)+get_local_id(0);

    for(char j = 0; j < COUNT; j++) { 
        set_decorator_seed(seed, i, arr[j].chunk_x << 4, arr[j].chunk_z << 4, 9, 6);
        char x = next(seed, 4);
        char z = next(seed, 4);
        char y = next(seed, 4);
        // printf("%d %d %d != %d %d %d\n", x,z,y, arr[j].x, arr[j].z, arr[j].y);
        if(x != arr[j].x) { return; }
        if(z != arr[j].z) { return; }
        if(y != arr[j].y) { return; }
    }
    printf("found seed %llu\n", i);
    // printf("%llu %llu %llu %llu %llu %llu\n", get_global_size(0), get_global_id(0), 
    //                                           get_local_size(0), get_local_id(0),
    //                                           get_num_groups(0), get_group_id(0));
}