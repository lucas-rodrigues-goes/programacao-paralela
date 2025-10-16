# Thread Pool

## Implementation

The thread pool manages multiple worker threads that execute tasks from a queue.

**Worker (`pool_worker`)**

* Each thread runs this function in a continuous loop.
* The thread waits while the queue is empty and the pool is not being shut down.
* When a task is available, it removes it from the queue (`task_queue_pop`) and executes its function.
* If the pool is shutting down (`stop`) and there are no more tasks, the thread exits the loop.

**Pool Initialization (`pool_init`)**

* Sets the number of threads and allocates the thread array.
* Initializes the task queue with the desired capacity.
* Creates the threads, assigning each to the `pool_worker` function.

**Adding Tasks (`pool_run`)**

* Creates a `task` structure with the provided function and argument this task can also behave as a future.
* Adds the task to the queue using `task_queue_push`.
* If the pool is already shut down, no new tasks are allowed.

**Pool Shutdown (`pool_shutdown`)**

* Locks the queue mutex.
* Waits for the queue to become empty, ensuring all running tasks finish.
* Signals the pool to stop accepting new tasks (`stop = true`) and wakes up waiting threads.
* Waits for all threads to finish (`pthread_join`).
* Frees allocated memory and destroys mutexes and condition variables.

**Notes**

* Using **mutexes** ensures that only one thread manipulates the queue at a time.
* **Condition variables** allow threads to wait efficiently without consuming CPU.
* The pool guarantees all tasks are completed before shutdown.
* The circular queue avoids memory waste by reusing already allocated slots.

This implementation provides a safe and efficient foundation for parallel task execution in C using **POSIX threads (pthreads)**.

---

## Examples

In the project, three execution versions were created to solve the same problem. Given a list of size **N**, filled with random numbers, a function is applied to all values in the list, replacing each element with the next prime number.

* The **sequential version** iterates over the list, executing the next-prime calculation function on each element. (Does not use Thread Pool)
* The **parallel version** splits the list into equal parts among multiple threads, and each thread processes a fraction of the list. (Does not use Thread Pool)
* The **library version** uses the **Thread Pool** implemented as described above.
* the **future version** shows an example where the result from a task is later retrieved using futures.

Below are the compilation instructions using **GCC** for each version:

### Sequential Program

```bash
gcc linear_program.c -o linear_program
```

### Parallel Program

```bash
gcc parallel_program.c -o parallel_program
```

### Library Program (Thread Pool)

```bash
gcc pool_program.c library/thread_pool.o -o pool_program -lpthread
```

### Future Program (Thread Pool)

```bash
gcc future_program.c library/thread_pool.o -o future_program -lpthread
```
