#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h> // for sleep() in example

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

// Init
void task_queue_init(task_queue* q, int capacity) {
    q->queue = calloc(capacity, sizeof(task));
    q->capacity = capacity;
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

// Push
void task_queue_push(task_queue* q, task t) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == q->capacity) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }

    q->queue[q->tail] = t;
    q->tail = (q->tail + 1) % q->capacity;
    q->count++;

    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

// Pop
task task_queue_pop(task_queue* q) {
    pthread_mutex_lock(&q->mutex);
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }

    task t = q->queue[q->head];
    q->head = (q->head + 1) % q->capacity;
    q->count--;

    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);

    return t;
}

//==================================
// Pool
//==================================
typedef struct {
    pthread_t* threads;
    int size;
    task_queue queue;
    bool stop;
} pool;

// Worker
void* pool_worker(void* arg) {
    pool* p = (pool*)arg;

    while (true) {
        pthread_mutex_lock(&p->queue.mutex);
        while (p->queue.count == 0 && !p->stop) {
            pthread_cond_wait(&p->queue.not_empty, &p->queue.mutex);
        }

        if (p->stop) {
            pthread_mutex_unlock(&p->queue.mutex);
            break;
        }

        task t = p->queue.queue[p->queue.head];
        p->queue.head = (p->queue.head + 1) % p->queue.capacity;
        p->queue.count--;

        pthread_cond_signal(&p->queue.not_full);
        pthread_mutex_unlock(&p->queue.mutex);

        // Execute outside lock
        t.function(t.arg);
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
void pool_run(pool* p, void (*function)(void*), void* arg) {
    task t = { function, arg };
    task_queue_push(&p->queue, t);
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

//==================================
// Example
//==================================
void print_message(void* arg) {
    char* msg = (char*)arg;
    printf("[Thread] %s\n", msg);
}

int main() {
    pool my_pool;
    pool_init(&my_pool, 3, 10);

    char* msgs[] = {
        "Task 1 running",
        "Task 2 running",
        "Task 3 running",
        "Task 4 running",
        "Task 5 running",
        "Task 6 running"
    };

    for (int i = 0; i < 6; i++) {
        pool_run(&my_pool, print_message, msgs[i]);
    }

    pool_shutdown(&my_pool);
    return 0;
}
