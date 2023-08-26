#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>


volatile sig_atomic_t counter = 0;
volatile sig_atomic_t exit_trigger = 0;

FILE* files[33];

void SIGHandler(int idx) {
  if (idx < SIGRTMIN || idx > SIGRTMAX) {
    return;
  }
  idx -= SIGRTMIN;
  if (idx == 0) {
    exit_trigger = 1;
    return;
  }
  char str[4096] = {};
  fscanf(files[idx-1], "%[^\n]%*c", str);
  printf("%s\n", str);
  fflush(stdout);
}

int main(int argc, char** argv) {
  struct sigaction sig_handler;
  memset(&sig_handler, 0, sizeof(sig_handler));
  sig_handler.sa_handler = SIGHandler;
  for (size_t i = 0; i <= 64; ++i) {
    sigaction(i, &sig_handler, NULL);
  }

  int i = 0;
  while (argv[i+1] != NULL) {
    files[i] = fopen(argv[i+1], "r");
    ++i;
  }

  while (!exit_trigger) {
    pause();
  }
	
  i = 0;
  while (argv[i+1] != NULL) {
    fclose(files[i]);
    ++i;
  }

  return 0;
}
