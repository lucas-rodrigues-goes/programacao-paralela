#include "thread_pool.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

//==================================
// Task Queue
//==================================

// Init
// Inicializa a estrutura de fila de tarefas
void task_queue_init(task_queue* q, int capacity) {
    q->queue = calloc(capacity, sizeof(task)); // Aloca memória para a queue
    q->capacity = capacity;
    q->head = 0;
    q->tail = 0;
    q->count = 0;

    // Mutex
    pthread_mutex_init(&q->mutex, NULL);

    // Sinalizadores
    pthread_cond_init(&q->not_empty, NULL);
    pthread_cond_init(&q->not_full, NULL);
}

// Push
// Insere uma tarefa na queue (espera se estiver cheia)
void task_queue_push(task_queue* q, task t) {
    // Bloqueia acesso a fila (por outras threads)
    pthread_mutex_lock(&q->mutex);

    // Aguarda sinalização de fila não cheia
    while (q->count == q->capacity) {
        pthread_cond_wait(&q->not_full, &q->mutex);
    }

    q->queue[q->tail] = t; // Insere tarefa na posição final
    q->tail = (q->tail + 1) % q->capacity; // Avança o índice circularmente
    q->count++;

    // Sinaliza que a fila não está vazia e destrava o acesso
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
}

// Pop
// Remove e retorna uma tarefa da queue (espera se estiver vazia)
task task_queue_pop(task_queue* q) {
    // Bloqueia acesso a fila (por outras threads)
    pthread_mutex_lock(&q->mutex);

    // Aguarda sinalização de fila não vazia
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->mutex);
    }

    task t = q->queue[q->head]; // Obtém a tarefa da posição inicial
    q->head = (q->head + 1) % q->capacity; // Avança índice circularmente
    q->count--;

    // Sinaliza que a fila não está cheia e destrava o acesso
    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->mutex);

    return t; // Retorna a tarefa
}

//==================================
// Thread Pool
//==================================

// Worker
// Função executada por cada thread (trabalhador) do pool
void* pool_worker(void* arg) {
    pool* p = (pool*)arg;

    while (true) {
        pthread_mutex_lock(&p->queue.mutex);   // Protege acesso à queue

        // Aguarda enquanto não há tarefas e o pool não está sendo encerrado
        while (p->queue.count == 0 && !p->stop) {
            pthread_cond_wait(&p->queue.not_empty, &p->queue.mutex);
        }

        // Sai do loop se o pool estiver parando e não houver mais tarefas
        if (p->stop && p->queue.count == 0) {
            pthread_mutex_unlock(&p->queue.mutex);
            break;
        }

        // Destrava a fila
        pthread_mutex_unlock(&p->queue.mutex);

        // Remove a tarefa da queue
        task t = task_queue_pop(&p->queue);

        // Executa a tarefa fora do lock
        t.function(t.arg);
    }

    return NULL;
}

// Init
// Inicializa o thread pool
void pool_init(pool* target_pool, int thread_count, int queue_capacity) {
    target_pool->size = thread_count;
    target_pool->threads = calloc(thread_count, sizeof(pthread_t)); // Aloca vetor de threads
    target_pool->stop = false;
    task_queue_init(&target_pool->queue, queue_capacity); // Inicializa a queue de tarefas

    // Cria threads e associa a função worker
    for (int i = 0; i < thread_count; i++) {
        pthread_create(&target_pool->threads[i], NULL, pool_worker, target_pool);
    }
}

// Run
// Adiciona uma nova tarefa ao pool
void pool_run(pool* p, void (*function)(void*), void* arg) {
    if (p->stop) {  // Se o pool já foi desligado, não permite novas tarefas
        printf("Can't add more tasks to pool after it has been shutdown");
        return;
    }

    task t = { function, arg }; // Cria tarefa
    task_queue_push(&p->queue, t); // Adiciona na queue
}

// Shutdown
// Encerra o pool, aguardando todas as tarefas concluírem
void pool_shutdown(pool* p) {
    pthread_mutex_lock(&p->queue.mutex);

    // Aguarda a queue ficar vazia antes de parar
    while (p->queue.count > 0) {
        pthread_cond_wait(&p->queue.not_full, &p->queue.mutex);
    }

    p->stop = true; // Sinaliza Pool para não receber mais tarefas
    pthread_cond_broadcast(&p->queue.not_empty); // Desbloqueia threads em espera
    pthread_mutex_unlock(&p->queue.mutex);

    // Aguarda todas as threads terminarem
    for (int i = 0; i < p->size; i++) {
        pthread_join(p->threads[i], NULL);
    }

    // Libera memória e destrói mutex/condições
    free(p->threads);
    free(p->queue.queue);
    pthread_mutex_destroy(&p->queue.mutex);
    pthread_cond_destroy(&p->queue.not_empty);
    pthread_cond_destroy(&p->queue.not_full);
}
