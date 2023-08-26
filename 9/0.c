#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

int main(int argc, char* argv[]) {
  char filename[PATH_MAX];
  struct stat S;
  uint64_t size = 0;
  while (fgets(filename, PATH_MAX, stdin) != NULL) {
    strtok(filename, "\n");
    if (lstat(filename, &S) != -1) {
      if (S_ISREG(S.st_mode)) {
        size += S.st_size;
      }
    }
  }
  printf("%lu\n", size);
  return 0;
}
