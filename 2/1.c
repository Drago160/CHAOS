#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
  uint32_t N;
  uint64_t A;
  uint64_t B;
  pthread_mutex_t* mutex;
  pthread_cond_t* to_main;
  pthread_cond_t* from_main;
  uint64_t val;
  int cnt;
  int is_ready;
} data_t;

int is_prime(uint64_t value) {
  if (value == 1 || value == 0) {
    return 0;
  }
  if (value == 3 || value == 2) {
    return 1;
  }
  for (uint64_t i = 2; i*i <= value; ++i) {
    if (value % i == 0) {
      return 0;
    }
  }
  return 1;
}

void* thread_func(void* data) {
  data_t* real_data = (data_t*)data;
  pthread_mutex_lock(real_data->mutex);
  while (1) {
    while (!is_prime(real_data->val)) {
      ++(real_data->val);
    }
    real_data->is_ready = 1;
    ++real_data->cnt;
    pthread_cond_signal(real_data->to_main);
    while (real_data->is_ready) {
      pthread_cond_wait(real_data->from_main, real_data->mutex);
    }
    ++real_data->val;
  }
  return NULL;
}

int main(int argc, char* argv[]) {
  data_t data;
  data.A = strtol(argv[1], NULL, 10);
  data.N = strtol(argv[3], NULL, 10);

  data.val = data.A;
  data.cnt = 0;
  data.is_ready = 0;

  pthread_mutex_t mutex;
  pthread_cond_t to_main;
  pthread_cond_t from_main;

  data.from_main = &to_main;
  data.to_main = &from_main;
  data.mutex = &mutex;

  pthread_mutex_init(data.mutex, NULL);
  pthread_cond_init(data.to_main, NULL);
  pthread_cond_init(data.from_main, NULL);

  pthread_t thread;
  pthread_mutex_lock(data.mutex);
  pthread_create(&thread, NULL, thread_func, (void*)&data);

  int64_t out;
  while (data.cnt < data.N) {
    while (!data.is_ready) {
      pthread_cond_wait(data.to_main, data.mutex);
    }
    out = data.val;
    data.is_ready = 0;
    pthread_cond_signal(data.from_main); // to count prime while std out
    printf("%lu\n", out);
  }
  pthread_mutex_unlock(data.mutex);
  exit(0);
}
