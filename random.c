// unsigned long long typedef
typedef unsigned long long u64;

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