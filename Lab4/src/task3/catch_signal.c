#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void signal_handler(int sig) {
  switch (sig) {
    case SIGINT:
      printf("\nGet a signal: SIGINT. You pressed CTRL+C.\n");
      break;
    case SIGQUIT:
      printf("\nGet a signal: SIGQUIT. You pressed CTRL+\\.\n");
      break;
    case SIGTSTP:
      printf("\nGet a signal: SIGTSTP. You pressed CTRL+Z.\n");
      break;
  }
  exit(0);
}

int main() {
  printf("Current process ID is %d\n", getpid());
  signal(SIGINT, signal_handler);
  signal(SIGQUIT, signal_handler);
  signal(SIGTSTP, signal_handler);
  for (;;) {
  }
}
