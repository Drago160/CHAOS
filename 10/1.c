#define _GNU_SOURCE
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

struct Item {
  int value;
  uint32_t next_pointer;
} Item;

int main(int argc, char** argv) {
  int input = open(argv[1], O_RDONLY);
  struct stat S;
  fstat (input, &S);
  size_t size = S.st_size;
  if (size == 0) {
    return 0;
  }
  if (input == -1) {
    return 0;
  }
  char* contents = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, input, 0);

  uint32_t idx = 0;
  while (((struct Item*)(contents+idx))->next_pointer != 0) {
    printf("%d ",((struct Item*)(contents+idx))->value);
    idx = ((struct Item*)(contents+idx))->next_pointer;
  }

  printf("%d ",((struct Item*)(contents+idx))->value);
  munmap(contents, S.st_size);
  close(input);

  return 0;
}
