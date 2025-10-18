#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include "library/thread_pool.h"

// Compute the next prime after a given number, compatible with future
void* next_prime(void* arg) {
    int number = *(int*)arg;

    while (true) {
        number = number + 1;

        if (number <= 2) {
            // Allocate memory for the result
            int* result = malloc(sizeof(int));
            *result = 2;
            return result; // Return pointer to result for future
        }
        if (number % 2 == 0) continue;

        int i = 2;
        while (i <= number) {
            if (i > number / 2) {
                // Allocate memory for the result
                int* result = malloc(sizeof(int));
                *result = number;
                return result; // Return pointer to result for future
            }

            // If the number is divisible by any number other than 1 or itself, it is not prime
            else if ((number % i) == 0) break;
            i = i + 1;
        }
    }
}

int main(void){
    int number = 10;

    pool p;
    pool_init(&p, 6, 10);
    future* prime_from_number = pool_run(&p, next_prime, &number, false);

    number = *(int*)future_get(prime_from_number);

    printf("%d", number);

    return 0;
}
