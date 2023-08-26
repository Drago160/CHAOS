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



int main(int argc, char *argv[]) {
  char* cmd1 = argv[1];
  char* cmd2 = argv[2];
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
    if (execlp(cmd2, cmd2, NULL) == -1) {
      perror("exec error");
      exit(1);
    }
  }
  close(fds_pair[0]);
  close(fds_pair[1]);
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);
}
