#include "thread_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

//==================================
// Future / Task
//==================================

// Init
void future_init(future* f, void* (*function)(void*), void* arg, bool temporary) {
    f->function = function;
    f->arg = arg;
    f->result = NULL;
    f->ready = false;
    f->temporary = temporary;   // store flag
    pthread_mutex_init(&f->mutex, NULL);
    pthread_cond_init(&f->cond, NULL);
}

// Get
// Wait for completion, get result, and destroy the future
void* future_get(future* f) {
    pthread_mutex_lock(&f->mutex);

    while (!f->ready) {
        pthread_cond_wait(&f->cond, &f->mutex);
    }

    void* result = f->result;
    pthread_mutex_unlock(&f->mutex);

    pthread_mutex_destroy(&f->mutex);
    pthread_cond_destroy(&f->cond);
    free(f);

    return result;
}

// Set result
static void future_set_result(future* f, void* result) {
    pthread_mutex_lock(&f->mutex);
    f->result = result;
    f->ready = true;
    pthread_cond_signal(&f->cond);
    pthread_mutex_unlock(&f->mutex);
}

//==================================
// Task Queue
//==================================

// Init
void task_queue_init(task_queue* q, int capacity) {
    q->queue = calloc(capacity, sizeof(future*));
    q->capacity = capacity;
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

// Push
void task_queue_push(task_queue* q, future* f) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == q->capacity) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }
    q->queue[q->tail] = f;
    q->tail = (q->tail + 1) % q->capacity;
    q->count++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

// Pop
future* task_queue_pop(task_queue* q) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }
    future* f = q->queue[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->count--;
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);
    return f;
}

//==================================
// Thread Pool
//==================================

// Worker
static void* pool_worker(void* arg) {
    pool* p = (pool*)arg;

    while (true) {
        pthread_mutex_lock(&p->queue.mutex);
        while (p->queue.count == 0 && !p->stop) {
            pthread_cond_wait(&p->queue.not_empty, &p->queue.mutex);
        }
        if (p->stop && p->queue.count == 0) {
            pthread_mutex_unlock(&p->queue.mutex);
            break;
        }
        pthread_mutex_unlock(&p->queue.mutex);

        future* f = task_queue_pop(&p->queue);

        // Run the function
        void* result = f->function(f->arg);
        future_set_result(f, result);

        // If the future is temporary, immediately call future_get to free it
        if (f->temporary) {
            future_get(f); 
        }
    }

    return NULL;
}

// Init
void pool_init(pool* target_pool, int thread_count, int queue_capacity) {
    target_pool->size = thread_count;
    target_pool->threads = calloc(thread_count, sizeof(pthread_t));
    target_pool->stop = false;
    task_queue_init(&target_pool->queue, queue_capacity);

    for (int i = 0; i < thread_count; i++) {
        pthread_create(&target_pool->threads[i], NULL, pool_worker, target_pool);
    }
}

// Run
future* pool_run(pool* p, void* (*function)(void*), void* arg, bool temporary) {
    if (p->stop) {
        printf("Can't add more tasks to pool after it has been shutdown\n");
        return NULL;
    }

    future* f = malloc(sizeof(future));
    future_init(f, function, arg, temporary);
    task_queue_push(&p->queue, f);

    return f;
}

// Shutdown
void pool_shutdown(pool* p) {
    pthread_mutex_lock(&p->queue.mutex);
    while (p->queue.count > 0) {
        pthread_cond_wait(&p->queue.not_full, &p->queue.mutex);
    }
    p->stop = true;
    pthread_cond_broadcast(&p->queue.not_empty);
    pthread_mutex_unlock(&p->queue.mutex);

    for (int i = 0; i < p->size; i++) {
        pthread_join(p->threads[i], NULL);
    }

    free(p->threads);
    free(p->queue.queue);
    pthread_mutex_destroy(&p->queue.mutex);
    pthread_cond_destroy(&p->queue.not_empty);
    pthread_cond_destroy(&p->queue.not_full);
}