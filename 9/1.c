#include <stdint.h>
#include <sys/stat.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <elf.h>

int isExecutable(char* file) {
  struct stat S;
  int is_worked = lstat(file, &S);
  if (is_worked == -1) {
    return -1;
  }
  if ((S.st_mode & S_IXUSR) != S_IXUSR) {
    return 0;
  }
  return 1;
}

int isRightShebang(char* filename) {
  char buff[PATH_MAX];
  char path[PATH_MAX];
  FILE* file = fopen(filename, "r");
  fscanf(file, "%s", buff);
  if (buff[0] != '#') {
    fclose(file);
    return -1;
  }
  if (buff[1] != '!') {
    fclose(file);
    return -1;
  }
  int i = 0;
  while (buff[i + 2] != '\0') {
    path[i] = buff[i + 2];
    ++i;
  }
  int ret = isExecutable(path);
  fclose(file);
  return ret;
}

int isCorrectElf(char* filename) {
  char buff[PATH_MAX];
  FILE* file = fopen(filename, "r");
  fscanf(file, "%s", buff);
  if (buff[0] == ELFMAG0 && buff[1] == ELFMAG1 && buff[2] == ELFMAG2 && buff[3] == ELFMAG3) {

    fclose(file);
    return 1;
  }

  fclose(file);
  return -1;
}

int main() {

  char filename[PATH_MAX];
  struct stat S;
  uint64_t size = 0;

  while (fgets(filename, PATH_MAX, stdin) != NULL) {
    strtok(filename, "\n");
    if (lstat(filename, &S) != -1) {
      if (isExecutable(filename) == 1) {
        if (!(isRightShebang(filename) == 1) && !(isCorrectElf(filename) == 1)) {
          printf("%s\n", filename);
        }
      }
    }
  }

  return 0;
}
