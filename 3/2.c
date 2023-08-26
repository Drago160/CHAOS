

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

int is_pred_str(char* filename, char* buff) {
  int i = 0;
  while (filename[i] != '\0') {
    if (buff[i] == '\0' || filename[i] != buff[i]) {
      return -1;
    }
    ++i;
  }
  if (buff[i] != ':' || buff[i + 1] == '\0') {
    return -1;
  }
  long int str_num = strtol(buff + i + 1, NULL, 10);
  return str_num;
}

int main(int argc, char *argv[]) {
  char* prog = argv[1];
  int fds_pair[2];

  if(pipe(fds_pair) < 0) {
    perror("pipe error");
    exit(1);
  }

  pid_t pid = fork();
  if (pid == 0) {
    close(fds_pair[0]);
    dup2(fds_pair[1], 2);
    close(fds_pair[1]);

    if (execlp("gcc", "gcc", prog, NULL) == -1) {
      perror("exec error");
      exit(1);
    }
  }
  else {
    close(fds_pair[1]);
    dup2(fds_pair[0], 0);
    close(fds_pair[0]);

    uint64_t error_count = 0;
    uint64_t warning_count = 0;

    char buff[4096];
    int pred_flag = 0;
    int last_error_str = -1;
    int last_warn_str = -1;
    int pred_str = -1;
    int pred_pred_str = -1;
    while (scanf("%s", buff)  > 0) {
      pred_str = is_pred_str(prog, buff);
      if (pred_str != -1) {
        pred_pred_str = pred_str;
        pred_flag = 1;
      } else {
        if (pred_flag == 1 && buff[0] != '\0' && buff[1] != '\0' && buff[2] != '\0' && buff[3] != '\0') {
          if (buff[0] == 'w' && buff[1] == 'a' && buff[2] == 'r' && buff[3] == 'n') {
            if (pred_pred_str != last_warn_str) {
              ++warning_count;
              last_warn_str = pred_pred_str;
            }
          } else if (buff[0] == 'e' && buff[1] == 'r' && buff[2] == 'r' && buff[3] == 'o') {
            if (pred_pred_str != last_error_str) {
              ++error_count;
              last_error_str = pred_pred_str;
            }
          }
        }
        pred_flag = 0;
      }
    }
    waitpid(pid, NULL, 0);
    printf("%ld %ld", error_count, warning_count);
  }
}
