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