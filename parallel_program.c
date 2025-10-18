#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include <pthread.h>

// Definitions
int list[100];
int num_threads = 24;
bool show = false;

// Helper functions
void print_list(int list[], int length) {
    int i = 0;
    printf("[");
    while (i < length) {
        printf("%d", list[i]);
        if (i != (length - 1)) printf(", ");
        i = i + 1;
    }
    printf("]\n");
}

int next_prime(int number) {
    while (true) {
        number = number + 1;

        if (number <= 2) return 2;
        if (number % 2 == 0) continue;

        int i = 2;
        while (i <= number) {
            if (i > number / 2) return number;

            // If the number is divisible by any number other than 1 or itself, it is not prime
            else if ((number % i) == 0) break;
            i = i + 1;
        }
    }
}

// Replacement function (to be executed by the thread)
void* update_list(void* arg) {
    int id = *(int*)arg;
    int list_length = sizeof(list) / sizeof(list[0]);
    for (int i = id; i < list_length; i += num_threads) {
        list[i] = next_prime(list[i]);
    }
}

int main(void){
    struct timeval t1, t2;
    srand(time(NULL));

    // Start time capture
    gettimeofday(&t1, NULL);

    // Initialize the list with random numbers
    int list_length = sizeof(list) / sizeof(list[0]);
    for (int i = 0; i < list_length; i++) {
        list[i] = (rand() % 100) * 134718;
    }
    if (show) {
        printf("Original list:   ");
        print_list(list, list_length);
    }

    // Thread initialization
    pthread_t threads[num_threads];
    int tid[num_threads];
    for (int i = 0; i < num_threads; i++) {
        tid[i] = i;
        pthread_create(&threads[i], NULL, update_list, &tid[i]);
    }
    
    // Synchronized thread completion
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // Display updated list values
    if (show) {
        printf("Updated list:    ");
        print_list(list, list_length);
    }

    // End time capture
    gettimeofday(&t2, NULL);
    double total_time = (t2.tv_sec - t1.tv_sec) + ((t2.tv_usec - t1.tv_usec)/1000000.0);
    printf("\nTotal time = %fs\n", total_time);

    return 0;
}