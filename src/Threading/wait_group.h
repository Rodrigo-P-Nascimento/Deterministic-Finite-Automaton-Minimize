//
// Created by ekank on 5/20/23.
//

#pragma once

#include <stdint.h>
#include <pthread.h>

typedef struct {
    uint64_t count;
    pthread_mutex_t mutex;
    pthread_cond_t signal;
} WaitGroup;

WaitGroup* WG_New(uint64_t n);
void WG_Add(WaitGroup* wg, uint64_t n);
void WG_Done(WaitGroup* wg);
void WG_Wait(WaitGroup* wg);
void WG_Destroy(WaitGroup* wg);
