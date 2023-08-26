#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// | - realized in one.c
// main(main, 1, 2, 3, 4, ...)=     (1 | main(main, 2, 3, 4, ...))
// main(main, 1) = 1
  int main(int argc, char *argv[]) {
  if (argc == 2) {
    execlp(argv[1], argv[1], NULL);
  }
  char* cmd1 = argv[1];
  int fds_pair[2];

  if(pipe(fds_pair) < 0) {
    perror("pipe error");
    exit(1);
  }

  pid_t pid1 = fork();
  if (pid1 == 0) {
    close(fds_pair[0]);
    dup2(fds_pair[1], 1);
    close(fds_pair[1]);
    if (execlp(cmd1, cmd1, NULL) == -1) {
      perror("exec error");
      exit(1);
    }
  }

  pid_t pid2 = fork();
  if (pid2 == 0) {
    close(fds_pair[1]);
    dup2(fds_pair[0], 0);
    close(fds_pair[0]);
    argv[1] =  argv[0];
    if (execvp(argv[1], argv + 1) == -1) {
      perror("exec error");
      exit(1);
    }
  }
  close(fds_pair[0]);
  close(fds_pair[1]);
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);
}
