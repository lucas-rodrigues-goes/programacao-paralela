#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <stdbool.h>

//==================================
// Future / Task
//==================================
typedef struct future {
    void* (*function)(void*);
    void* arg;
    void* result;
    bool ready;
    bool temporary;         // new flag
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} future;

// Init
void future_init(future* f, void* (*function)(void*), void* arg, bool temporary);

// Get
void* future_get(future* f);

//==================================
// Task Queue
//==================================
typedef struct {
    future** queue;
    int capacity;
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} task_queue;

// Init
void task_queue_init(task_queue* q, int capacity);

// Push
void task_queue_push(task_queue* q, future* f);

// Pop
future* task_queue_pop(task_queue* q);

//==================================
// Thread Pool
//==================================
typedef struct {
    pthread_t* threads;
    int size;
    task_queue queue;
    bool stop;
} pool;

// Init
void pool_init(pool* target_pool, int thread_count, int queue_capacity);

// Run
future* pool_run(pool* p, void* (*function)(void*), void* arg, bool temporary);

// Shutdown
void pool_shutdown(pool* p);

#endif