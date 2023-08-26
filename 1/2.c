#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "stdlib.h"

void* thread_func2 (void *arg)
{
  int val;
  int fd = *(int*)arg;
  while (1)
  {
    if (read(fd, &val, sizeof(int)) == 0) {
      return NULL;
    }
    val += 5;
    printf("%d ", val);
    if (val > 100 || val == 0) {
      shutdown(fd, SHUT_RDWR);
      return NULL;
    }
    write(fd, &val, sizeof(int));
  }

  return NULL;
}

struct pair {
  int first;
  int second;
};

void* thread_func1 (void* arg)
{
  int val = ((struct pair*)arg)->first;
  int fd = ((struct pair*)arg)->second;
  while (1)
  {
    val -= 3;
    printf("%d ", val);
    if (val > 100 || val == 0) {
      shutdown(fd, SHUT_RDWR);
      return NULL;
    }
    write(fd, &val, sizeof(int));
    if (read(fd, &val, sizeof(int)) == 0) {
      return NULL;
    }
  }

  return NULL;
}


int main(int argc, char **argv)
{
  int64_t num = strtol(argv[1], NULL, 10);
  int sockets[2];

  socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);

  struct pair pair;
  pair.first = num;
  pair.second = sockets[0];

  pthread_t thread1;
  pthread_create(&thread1, NULL, thread_func1, &pair);

  pthread_t thread2;
  pthread_create(&thread2, NULL, thread_func2, (void *)(&sockets[1]));

  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  close(sockets[0]);
  close(sockets[1]);

  return 0;
}
