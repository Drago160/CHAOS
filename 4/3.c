#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>

volatile sig_atomic_t exit_trigger = 0;

void Handler(int sig_idx, siginfo_t *info, void *ucontext) {
  if (sig_idx != SIGRTMIN) {
    return;
  }
  pid_t other_pid = info->si_pid;
  union sigval copy;
  if (info->si_value.sival_int == 0) {
    exit_trigger = 1;
    return;
  }
  copy.sival_int = info->si_value.sival_int - 1;
  sigqueue(other_pid, sig_idx, copy);
}


int main(int argc, char** argv) {
  struct sigaction handler;
  memset(&handler, 0, sizeof(handler));
  handler.sa_flags = SA_SIGINFO;
  handler.sa_sigaction = Handler;
  sigaction(SIGRTMIN, &handler, NULL);

  while (!exit_trigger) {
    pause();
  }

  return 0;

}

