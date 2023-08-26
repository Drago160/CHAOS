#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>
#include <limits.h>

typedef struct Item {
  struct Item *next;
  int64_t      value;
} item_t;

item_t* HEAD = NULL;
int64_t N = 0;
int64_t k = 0;

void pop() {
  while (1) {
    item_t* curr = HEAD;
    int can_echange = atomic_compare_exchange_strong(&HEAD, &curr, HEAD->next);
    if (can_echange) {
      return;
    }
    sched_yield();
  }
}

void push(int64_t val) {
  item_t* curr = malloc(sizeof(item_t));
  curr->value = val;
  while(1) {
    curr->next = HEAD;
    int can_exchange_ = atomic_compare_exchange_strong(&HEAD, &curr->next, curr);
    if (can_exchange_) {
      return;
    }
    sched_yield();
  }
}

void clear() {
  item_t* curr = HEAD;
  while (HEAD != NULL) {
    curr = HEAD->next;
    free(HEAD);
    HEAD = curr;
  }
}

void print() {
  item_t* curr = HEAD;
  while (curr != NULL) {
    printf("%lu ", curr->value);
    curr = curr->next;
  }
}

void* thread_func(void* idx_ptr) {
  int idx = *(int*)idx_ptr;
  for (int i = 0; i < k; ++i) {
    push(idx*k + i);
  }
}

int main(int argc, char* argv[]) {
  N = strtol(argv[1], NULL, 10);
  k = strtol(argv[2], NULL, 10);
  pthread_t* threads = malloc(N * sizeof(pthread_t));
  int* idxs = malloc(N * sizeof(int));

  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setguardsize(&attr, 0);
  pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);

  for (int i = 0; i < N; ++i) {
    idxs[i] = i;
    pthread_create(threads + i, NULL, thread_func, (void*)(idxs+i));
  }

  for (int i = 0; i < N; ++i) {
    pthread_join(threads[i], NULL);
  }

  print();
  clear();
  free(idxs);
  free(threads);
}
