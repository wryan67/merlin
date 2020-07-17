#include "Common.h"
#include <sys/time.h>


unsigned long long currentTimeMillis() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);

    return (unsigned long long)(currentTime.tv_sec) * 1000 +
        (unsigned long long)(currentTime.tv_usec) / 1000;
}


int random(int low, int high) {
    double r = (double)rand() / RAND_MAX;

    return low + (r * (1 + high - low));
}

