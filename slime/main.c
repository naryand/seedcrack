#include "slime.h"
#include <stdio.h>
#include <pthread.h>
#include "../random.h"
#define THREADS 8

int main(int argc, char *argv[]) {
    // start and ends for top 30 bits
    //u64 start = strtold(argv[1], NULL); u64 end = strtold(argv[2], NULL);
    u64 start = 0; u64 end = 1073741824;

    // init variables
    int found, ret;
    info find18;
    info bounds[THREADS]; 
    pthread_t threads[THREADS];

    // get the lower 18 bits of world seed
    find18.found = 0;
    findlower18(&find18);
    found = find18.found;
    printf("found %d lower 18 bits\n", found);
    if(found > 1) { return 0; } found = 0;

    // start threads to crack middle 30 bits
    for(int i = 0; i < THREADS; i++) {
        bounds[i].start = i*((end-start)/THREADS)+start;
        bounds[i].end = (i+1)*((end-start)/THREADS)+start;
        bounds[i].found = 0;
        bounds[i].lower18 = find18.lower18;
        ret = pthread_create(threads+i, NULL, thread30, bounds+i);
        if(ret) { printf("thread %d create error\n", i); ret = 0; }
    }

    // end threads and report # of lower 48 bit seeds found
    for(int i = 0; i < THREADS; i++) {
        ret = pthread_join(threads[i], NULL);
        if(ret) { printf("thread %d join error\n", i); ret = 0; }
        found += bounds[i].found;
    }
    printf("found %d 48 bits\n", found);
}