#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

// Uso de uma matriz grande, que nao cabe integralmente na cache
#define MATRIX_SIZE 20000
#define NUM_THREADS 4

// Matriz (array 2D) acessado pelas threads simulada por uma estrutura de 1D (memoria contigua)
// ex. elemento [i][j] eh equivalente aa matrix[i * MATRIX_SIZE + j].
long *matrix;
int thread_id;

// Versao 1: Boa utilizacao da cache (explora localidade de dados)
// Itera pelas linhas da matriz
// -----------------------------------------------------------------
void* cache_friendly_thread(void* arg) {
    int id = *(int *)arg;
    
    // Cada thread trabalha em uma seccao horizontal da matriz
    int chunk_size = MATRIX_SIZE / NUM_THREADS;
    int start_row = id * chunk_size;
    int end_row = (id == NUM_THREADS - 1) ? MATRIX_SIZE : start_row + chunk_size;

    long sum = 0;

    // laco externo itera pelas linhas da matriz
    // laco interno itera nas colunas
    for (int i = start_row; i < end_row; i++) {
                for (int j = 0; j < MATRIX_SIZE; j++) {
            sum += matrix[i * MATRIX_SIZE + j];
        }
    }
    return (void*)sum;
}

// Versao 2: Mal uso da cache
// -----------------------------------------------------------------
void* cache_unfriendly_thread(void* arg) {
    int id = *(int *)arg;
    
    // Cada thread atua em uma fatia horizontal da matriz
    int chunk_size = MATRIX_SIZE / NUM_THREADS;
    int start_row = id * chunk_size;
    int end_row = (id == NUM_THREADS - 1) ? MATRIX_SIZE : start_row + chunk_size;

    long sum = 0;

    // laco externo itera pelas colunas da matriz
    // laco interno itera nas linhas 
    for (int j = 0; j < MATRIX_SIZE; j++) {
        for (int i = start_row; i < end_row; i++) {
            sum += matrix[i * MATRIX_SIZE + j];
        }
    }
    return (void*)sum;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int tid[NUM_THREADS];
    //struct timespec t1, t2;
    struct timeval t1, t2;

    double latency;

    matrix = (long*)malloc(sizeof(long) * MATRIX_SIZE * MATRIX_SIZE);
    
    for (long i = 0; i < MATRIX_SIZE * MATRIX_SIZE; i++) {
        matrix[i] = i;
    }

    printf("Starting bad cache performance test (Column-major access)...\n");
    //clock_gettime(CLOCK_MONOTONIC, &t1);
    gettimeofday(&t1, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        tid[i] = i;
        pthread_create(&threads[i], NULL, cache_unfriendly_thread, &tid[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    //clock_gettime(CLOCK_MONOTONIC, &t2);
    //latency = (t2.tv_sec - t1.tv_sec) * 1000.0;
    //latency += (t2.tv_nsec - t1.tv_nsec) / 1000000.0;
    gettimeofday(&t2, NULL);
    latency = (t2.tv_sec - t1.tv_sec)*1000 + ((t2.tv_usec - t1.tv_usec)/1000.0);
    printf("Bad cache version finished in: %.2f ms\n", latency);


    printf("Starting good cache performance test (Row-major access)...\n");
    //clock_gettime(CLOCK_MONOTONIC, &t1);
    gettimeofday(&t1, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        tid[i] = i;
        pthread_create(&threads[i], NULL, cache_friendly_thread, &tid[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

      //clock_gettime(CLOCK_MONOTONIC, &t2);
    //latency = (t2.tv_sec - t1.tv_sec) * 1000.0;
    //latency += (t2.tv_nsec - t1.tv_nsec) / 1000000.0;
    gettimeofday(&t2, NULL);
    latency = (t2.tv_sec - t1.tv_sec)*1000 + ((t2.tv_usec - t1.tv_usec)/1000.0);
    printf("Good cache version finished in: %.2f ms\n\n", latency);      
  

    free(matrix);
    return 0;
}

