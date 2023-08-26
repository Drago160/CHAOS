#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main() {
  setbuf(stdin, NULL);
  int ret_status = 1;
  int counter = 0;
  char word[4096];
  while (ret_status != 0) {
    int pid = fork();
    if (pid == 0) {
      return scanf("%s", word) != -1;
    } else {
      int status;
      waitpid(pid, &status, 0);
      ret_status = WEXITSTATUS(status);
      counter += ret_status;
    }
  }
  printf("%d", counter);
  return 0;
}
