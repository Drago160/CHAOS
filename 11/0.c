#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
 int counter = 1;
 pid_t pid;
 do {
  pid = fork();
  if (-1 != pid) {
   counter++;
  }
 } while (0 == pid);

 if (pid == 1) {
  int status;
  waitpid(pid, &status, 0);
 }

 else if (pid == -1) {
  printf("%d", counter);
 }

 return 0;
}
