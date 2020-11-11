// struct for managing each worker thread
typedef struct info { 
    int found; int lower18;
    unsigned long long start; 
    unsigned long long end; 
} info;

// sets internal seed based on world seed and chunk coordinates
void set_slime_seed(unsigned long long *seed, unsigned long long set, int j);

// gets the lower 18 bits of the world seed using a mod 2 exploit
void *findlower18(void *arg);

// finds the remaining 30 bits using the mod 10
void *thread30(void *arg);