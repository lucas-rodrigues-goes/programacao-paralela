#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

struct timeval t1, t2;
gettimeofday(&t1, NULL);
// trecho a ser medido
gettimeofday(&t2, NULL);
double t_total = (t2.tv_sec - t1.tv_sec) + ((t2.tv_usec -
t1.tv_usec)/1000000.0);
printf("tempo total = %f\n", t_total);