//
// Created by ekank on 5/19/23.
//

#include "threading.h"
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

typedef struct {
    pthread_t* threads;
    uint8_t num_threads;
    bool shutdown;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} ThreadPool;

static ThreadPool pool = {};
static ThreadTask* threadTask = NULL;

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
        while (threadTask == NULL && !pool.shutdown) {
            pthread_cond_wait(&(pool.cond), &(pool.mutex));
        }

        if (pool.shutdown) {
            pthread_mutex_unlock(&(pool.mutex));
            pthread_exit(NULL);
        }

        assert(threadTask != NULL);
        task = threadTask;
        atomic_store_explicit(&threadTask, NULL, memory_order_seq_cst);
        assert(threadTask == NULL);

        pthread_mutex_unlock(&(pool.mutex));

        task->task(task->args);
        free(task);
    }
}

void AddTask(ThreadTask task)
{
    //while(atomic_load(&threadTask) != NULL) continue;

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

    pool.threads = calloc(pool.num_threads, sizeof(pthread_t));

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
