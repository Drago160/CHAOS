#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
  char filename[PATH_MAX];
  char symlink_name[PATH_MAX];
  char real_path[PATH_MAX];
  struct stat S;
  while (fgets(filename, PATH_MAX, stdin) != NULL) {
    strtok(filename, "\n");
    lstat(filename, &S);
    if (S_ISLNK(S.st_mode)) {
      realpath(filename, real_path);
      printf("%s\n", real_path);
    }
    if (S_ISREG(S.st_mode)) {
      sprintf(symlink_name, "link_to_%s", filename);
      symlink(filename, symlink_name);
    }
  }
  return 0;
}
