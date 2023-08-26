#include <unistd.h>
#include <stdio.h>
#include "string.h"
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>

int main() {

  char buff[4096];
  char to_exec[4096];
  fgets(buff, sizeof(buff), stdin);

  if (strlen(buff) == 0) {
	  return 0;
  }
  if (buff[strlen(buff) - 1] == '\n') {
    buff[strlen(buff) - 1] = '\0';
  }

  snprintf(to_exec, sizeof(to_exec), "#include<stdio.h>\n int main() {\n int x = (%s);\n printf(\"%%d\" ,x); \n return 0; }", buff);
  int pid = fork();
  if (pid == 0) {
    FILE* file = fopen("to_exec.c", "w");
    fputs(to_exec, file);
    fclose(file);
    execlp("gcc", "gcc", "to_exec.c", NULL);
  } else {
    wait(NULL);
    execlp("./a.out", "./a.out", NULL);
    perror("exec");
  }
  return 1;
}
