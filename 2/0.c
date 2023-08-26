#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
  pthread_mutex_t* mutex;
  double* numbers;
  int i;
  int N;
  int k;
} data_t;

void* thread_func(data_t* data) {
  double* numbers = data->numbers;
  int i = data->i;
  int N = data->N;
  int k = data->k;
  for (int j = 0; j < N; ++j) {
    pthread_mutex_lock(data->mutex);
    numbers[(i - 1 + k) % k] += 0.99;
    numbers[i] += 1;
    numbers[(i + 1) % k] += 1.01;
    pthread_mutex_unlock(data->mutex);
  }
}


int main(int argc, char* argv[]) {
  int N = strtol(argv[1], NULL, 10);
  int k = strtol(argv[2], NULL, 10);

  double* numbers = malloc(k * sizeof(double));
  pthread_t* threads = malloc(k * sizeof(pthread_t));


  for (int i = 0; i < k; ++i) {
    numbers[i] = 0.0;
  }

  data_t* args = malloc(k * sizeof(data_t));

  pthread_mutex_t mu = PTHREAD_MUTEX_INITIALIZER;

  for (int i = 0; i < k; ++i) {
    args[i].mutex = &mu;
    args[i].numbers = numbers;
    args[i].i = i;
    args[i].N = N;
    args[i].k = k;
    pthread_create(&threads[i], NULL, (void* (*)(void*))thread_func, (void*)&args[i]);
  }

  for (int i = 0; i < k; ++i) {
    pthread_join(threads[i], NULL);
  }

  for (int i = 0; i < k; ++i) {
    printf("%.10g\n", numbers[i]);
  }
  fflush(stdout);

  free(args);
  free(threads);
  free(numbers);

  return 0;
}
