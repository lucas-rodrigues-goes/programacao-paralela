#include "thread_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

//==================================
// Task Queue
//==================================

// Init
// Initializes the task queue structure
void task_queue_init(task_queue* q, int capacity) {
    q->queue = calloc(capacity, sizeof(task)); // Allocate memory for the queue
    q->capacity = capacity;
    q->head = 0;
    q->tail = 0;
    q->count = 0;

    // Mutex
    pthread_mutex_init(&q->mutex, NULL);

    // Condition variables
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

// Push
// Inserts a task into the queue (waits if full)
void task_queue_push(task_queue* q, task t) {
    // Lock queue access (for other threads)
    pthread_mutex_lock(&q->mutex);

    // Wait for signal that queue is not full
    while (q->count == q->capacity) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }

    q->queue[q->tail] = t; // Insert task at the end
    q->tail = (q->tail + 1) % q->capacity; // Advance index circularly
    q->count++;

    // Signal that queue is not empty and unlock access
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

// Pop
// Removes and returns a task from the queue (waits if empty)
task task_queue_pop(task_queue* q) {
    // Lock queue access (for other threads)
    pthread_mutex_lock(&q->mutex);

    // Wait for signal that queue is not empty
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }

    task t = q->queue[q->head]; // Get task from the front
    q->head = (q->head + 1) % q->capacity; // Advance index circularly
    q->count--;

    // Signal that queue is not full and unlock access
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);

    return t; // Return the task
}

//==================================
// Thread Pool
//==================================

// Worker
// Function executed by each pool thread (worker)
void* pool_worker(void* arg) {
    pool* p = (pool*)arg;

    while (true) {
        pthread_mutex_lock(&p->queue.mutex);   // Protect access to queue

        // Wait while there are no tasks and pool is not stopping
        while (p->queue.count == 0 && !p->stop) {
            pthread_cond_wait(&p->queue.not_empty, &p->queue.mutex);
        }

        // Exit loop if pool is stopping and no more tasks
        if (p->stop && p->queue.count == 0) {
            pthread_mutex_unlock(&p->queue.mutex);
            break;
        }

        // Unlock the queue
        pthread_mutex_unlock(&p->queue.mutex);

        // Remove the task from the queue
        task t = task_queue_pop(&p->queue);

        // Execute the task outside the lock
        t.function(t.arg);
    }

    return NULL;
}

// Init
// Initializes the thread pool
void pool_init(pool* target_pool, int thread_count, int queue_capacity) {
    target_pool->size = thread_count;
    target_pool->threads = calloc(thread_count, sizeof(pthread_t)); // Allocate thread array
    target_pool->stop = false;
    task_queue_init(&target_pool->queue, queue_capacity); // Initialize task queue

    // Create threads and associate worker function
    for (int i = 0; i < thread_count; i++) {
        pthread_create(&target_pool->threads[i], NULL, pool_worker, target_pool);
    }
}

// Run
// Adds a new task to the pool
void pool_run(pool* p, void (*function)(void*), void* arg) {
    if (p->stop) {  // If pool has been shut down, no more tasks allowed
        printf("Can't add more tasks to pool after it has been shutdown");
        return;
    }

    task t = { function, arg }; // Create task
    task_queue_push(&p->queue, t); // Add to queue
}

// Shutdown
// Shuts down the pool, waiting for all tasks to finish
void pool_shutdown(pool* p) {
    pthread_mutex_lock(&p->queue.mutex);

    // Wait for queue to become empty before stopping
    while (p->queue.count > 0) {
        pthread_cond_wait(&p->queue.not_full, &p->queue.mutex);
    }

    p->stop = true; // Signal pool to stop receiving tasks
    pthread_cond_broadcast(&p->queue.not_empty); // Unlock waiting threads
    pthread_mutex_unlock(&p->queue.mutex);

    // Wait for all threads to finish
    for (int i = 0; i < p->size; i++) {
        pthread_join(p->threads[i], NULL);
    }

    // Free memory and destroy mutex/conditions
    free(p->threads);
    free(p->queue.queue);
    pthread_mutex_destroy(&p->queue.mutex);
    pthread_cond_destroy(&p->queue.not_empty);
    pthread_cond_destroy(&p->queue.not_full);
}
