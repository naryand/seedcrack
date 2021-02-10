// unsigned long long typedef
typedef unsigned long long u64;

// sets internal seed
void set_seed(unsigned long long *seed, unsigned long long set);

// gets the top n bits off the internal seed
int next(unsigned long long *seed, int bits);

// returns a pseudorandom int given a seed
int next_int(unsigned long long *seed);

// returns bounded pseudorandom int given a seed and bound n
int next_int_bound(unsigned long long *seed, int n);

// returns pseudorandom long long given a seed
long long next_long(unsigned long long *seed);

// ores step 6, diamond index 9, emerald index 14
void set_decorator_seed(u64 *seed, u64 world_seed, int x, int z, char index, char step);