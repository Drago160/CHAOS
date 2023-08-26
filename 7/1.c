#include <unistd.h>
#include <sys/syscall.h>

long syscall(long number, ...);

void _start()
{
  char buffer[100];
  int cnt = 0;
  while(1) {
    cnt = syscall(SYS_read, 0, buffer, 100); 
    if (cnt == 0) {
	break; 
    }
    syscall(SYS_write, 1, buffer, cnt);
  }

  syscall(SYS_exit, 0);
}
