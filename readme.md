# Parallel Programming in C

## Introduction

In this project, three execution versions were created to solve the same problem: given a list of size **N** filled with randomly generated numbers, a function is executed on all values of this list, replacing each item with the next prime number.

The **sequential version** simply iterates over the list, executing the next-prime calculation function on each element. The **parallel version** splits the list into equal parts among multiple threads, and each thread processes a fraction of the list.

The **library version** uses a thread pool implementation, which provides a higher-level programming interface for assigning tasks to threads.

## Execution

Below are the compilation instructions using GCC for each version.

######

###### Sequential Program

* `gcc linear_program.c -o linear_program`

###### Parallel Program

* `gcc parallel_program.c -o parallel_program`

###### Library Program (Thread Pool)

* `gcc pool_program.c library/thread_pool.o -o pool_program -lpthread`