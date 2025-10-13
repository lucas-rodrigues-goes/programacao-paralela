#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <stdbool.h>

//==================================
// Task
//==================================
typedef struct {
    void (*function)(void*);
    void* arg;
} task;

//==================================
// Task Queue
//==================================
typedef struct {
    task* queue;
    int capacity;
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} task_queue;

// Initialize task queue
void task_queue_init(task_queue* q, int capacity);

// Create new task if queue not empty
void task_queue_push(task_queue* q, task t);

// Remove task from the queue
task task_queue_pop(task_queue* q);

//==================================
// Pool
//==================================
typedef struct {
    pthread_t* threads;
    int size;
    task_queue queue;
    bool stop;
} pool;

// Initialize a thread pool
void pool_init(pool* target_pool, int thread_count, int queue_capacity);

// Adds a new task to the pool
void pool_run(pool* p, void (*function)(void*), void* arg);

// Waits for all tasks in queue to end and shuts down
void pool_shutdown(pool* p);

#endif
