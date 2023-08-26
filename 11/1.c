#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  size_t N = atoi(argv[1]);
  for (size_t i = 1; i <= N; ++i) {
    int pid = fork();
    if (pid == 0) {
      printf("%d", i);
      if (i == N) {
        printf("\n");
      } else {
        printf(" ");
      }
      return 0;
    }
    wait(NULL);
  }
}
