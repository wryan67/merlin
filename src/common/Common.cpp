#include "Common.h"
#include <pthread.h>

pthread_t threadCreate(void* (*method)(void*), const char* description, void* args) {
    pthread_t threadId;

    int status = pthread_create(&threadId, NULL, method, args);
    if (status != 0) {
        printf("%s::thread create failed %d--%s\n", description, status, strerror(errno));
        exit(9);
    }
    pthread_detach(threadId);
    return threadId;
}

int random(int low, int high) {
    double r = (double)rand() / RAND_MAX;

    return low + (r * (1 + high - low));
}