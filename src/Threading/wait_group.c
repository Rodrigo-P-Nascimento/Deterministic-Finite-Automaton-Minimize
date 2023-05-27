//
// Created by ekank on 5/20/23.
//

#include "wait_group.h"

#include <stdlib.h>

WaitGroup *WG_New(uint64_t n){
    WaitGroup* wg = malloc(sizeof(WaitGroup));
    pthread_mutex_init(&wg->mutex, NULL);
    pthread_cond_init(&wg->signal, NULL);
    wg->count = n;

    return wg;
}

void inline WG_Add(WaitGroup* wg, uint64_t n){
    pthread_mutex_lock(&wg->mutex);
    wg->count += n;
    pthread_mutex_unlock(&wg->mutex);
}

void inline WG_Done(WaitGroup* wg){
    pthread_mutex_lock(&wg->mutex);
    wg->count--;
    pthread_cond_signal(&wg->signal);
    pthread_mutex_unlock(&wg->mutex);
}

void inline WG_Wait(WaitGroup* wg){
    pthread_mutex_lock(&wg->mutex);
    while (wg->count > 0) {
        pthread_cond_wait(&wg->signal, &wg->mutex);
    }
    pthread_mutex_unlock(&wg->mutex);
}

void inline WG_Destroy(WaitGroup* wg){
    pthread_mutex_destroy(&wg->mutex);
    pthread_cond_destroy(&wg->signal);
    free(wg);
}