#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "malloc.h"
#include "stdlib.h"
#include <limits.h>

static void* thread_func(void* arg) {
  int64_t partial_sum = 0;
  int value;
  while (scanf("%d", &value) != EOF) {
    partial_sum += value;
  }
  return (void*)partial_sum;
}

int main(int argc, char** argv) {
  int num_of_threads = strtol(argv[1], NULL, 10);
  pthread_t* all_threads = malloc(num_of_threads * sizeof(pthread_t));
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setguardsize(&attr, 0);
  pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);

  for (int i = 0; i < num_of_threads; ++i) {
    pthread_create(all_threads + i, NULL, thread_func, NULL);
  }
  int64_t global_sum = 0;
  for (int i = 0; i < num_of_threads; ++i) {
    void* partial_sum = NULL;
    pthread_join(all_threads[i], &partial_sum);
    global_sum += (int64_t)partial_sum;
  }
  pthread_attr_destroy(&attr);
  free(all_threads);
  printf("%ld", global_sum);
  return 0;
}
