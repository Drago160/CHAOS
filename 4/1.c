#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>


volatile sig_atomic_t counter = 0;
volatile sig_atomic_t exit_trigger = 0;


void SIGUSR1Handler() {
  ++counter;
  char str[42];
  write(1, str, sprintf(str, "%d\n", counter));
}

void SIGUSR2Handler() {
  counter *= -1;
  char str[42];
  write(1, str, sprintf(str, "%d\n", counter));
}

void SIGTERMINTHandler() {
  exit_trigger = 1;
}

int main () {
  struct sigaction sigusr1_handler;
  memset(&sigusr1_handler, 0, sizeof(sigusr1_handler));
  sigusr1_handler.sa_handler = SIGUSR1Handler;
  sigaction(SIGUSR1, &sigusr1_handler, NULL);

  struct sigaction sigusr2_handler;
  memset(&sigusr2_handler, 0, sizeof(sigusr2_handler));
  sigusr2_handler.sa_handler = SIGUSR2Handler;
  sigaction(SIGUSR2, &sigusr2_handler, NULL);

  struct sigaction sigtermint_handler;
  memset(&sigtermint_handler, 0, sizeof(sigtermint_handler));
  sigtermint_handler.sa_handler = SIGTERMINTHandler;
  sigaction(SIGTERM, &sigtermint_handler, NULL);
  sigaction(SIGINT, &sigtermint_handler, NULL);

  pid_t pid = getpid();
  char str[42];
  write(1, str, sprintf(str, "%d\n", pid));

  scanf("%d", &counter);

  while (!exit_trigger) {
    pause();
  }
  return 0;
}
