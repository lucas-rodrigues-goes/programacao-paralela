#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>

// Definitions
int list[100];
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

int main(void){
    struct timeval t1, t2;
    srand(time(NULL));

    // Start time capture
    gettimeofday(&t1, NULL);

    // Create the list with random numbers
    int list_length = sizeof(list) / sizeof(list[0]);
    for (int i = 0; i < list_length; i++) {
        list[i] = (rand() % 100) * 134718;
    }
    if (show) {
        printf("Original list:   ");
        print_list(list, list_length);
    }

    // Replace list values with their next primes
    for (int i = 0; i < list_length; i++) {
        list[i] = next_prime(list[i]);
    }
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
