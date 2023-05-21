//
// Created by ekank on 5/19/23.
//

#include "threading.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#define MAX_THREADS 16 // Maximum number of threads in the pool

typedef struct {
    pthread_t* threads;
    uint8_t num_threads;
    bool shutdown;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} ThreadPool;

static ThreadPool pool = {};
static ThreadTask* threadTask;

#ifdef _WIN32
static inline long getPhysicalProcessorCount()
{
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    return sysinfo.dwNumberOfProcessors;
}
#else
static inline long getPhysicalProcessorCount()
{
    return sysconf(_SC_NPROCESSORS_CONF);
}
#endif

static void* workerThread()
{
    ThreadTask* task;

    while (1) {
        pthread_mutex_lock(&(pool.mutex));
        while (!pool.shutdown && pool.num_threads == 0) {
            pthread_cond_wait(&(pool.cond), &(pool.mutex));
        }

        if (pool.shutdown) {
            pthread_mutex_unlock(&(pool.mutex));
            pthread_exit(NULL);
        }

        assert(threadTask != NULL);
        task = threadTask;
        threadTask = NULL;

        pthread_mutex_unlock(&(pool.mutex));

        task->task(task->arg);
        free(task);
    }
}

void AddTask(ThreadTask task)
{
    while(!(threadTask = threadTask)) continue;

    pthread_mutex_lock(&(pool.mutex));
    threadTask = malloc(sizeof(ThreadTask));
    *threadTask = task;
    pthread_mutex_unlock(&(pool.mutex));

    pthread_cond_signal(&(pool.cond));
}

void CreateThreadPool()
{
    pool.num_threads = getPhysicalProcessorCount();
    pool.shutdown = false;
    pthread_mutex_init(&(pool.mutex), NULL);
    pthread_cond_init(&(pool.cond), NULL);

    pool.threads = calloc(sizeof(pthread_t), pool.num_threads);

    for (int i = 0; i < pool.num_threads; ++i)
        pthread_create(&(pool.threads[i]), NULL, workerThread, NULL);
}

void DestroyThreadPool()
{
    if (pool.threads == NULL)
        return;

    pthread_mutex_lock(&(pool.mutex));
    pool.shutdown = true;
    pthread_mutex_unlock(&(pool.mutex));

    pthread_cond_broadcast(&(pool.cond));

    pthread_mutex_destroy(&(pool.mutex));
    pthread_cond_destroy(&(pool.cond));

    free(pool.threads);
}
