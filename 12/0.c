#include <unistd.h>
#include <stdio.h>
#include "string.h"
#include "stdlib.h"

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

  snprintf(to_exec, sizeof(to_exec), "x = %s; print(x)", buff);
  execlp("python3", "python3", "-c", to_exec, NULL);
  perror("exec");
  return 1; 
}
