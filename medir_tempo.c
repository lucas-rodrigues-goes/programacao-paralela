#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int main(void){
  struct timeval t1, t2;
  gettimeofday(&t1, NULL);

  printf("água\n");

  gettimeofday(&t2, NULL);
  double t_total = (t2.tv_sec - t1.tv_sec) + ((t2.tv_usec - t1.tv_usec)/1000000.0);
  printf("tempo total = %fs\n", t_total);

  return 0;
}