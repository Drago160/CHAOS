#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

struct Item {
  int value;
  uint32_t next_pointer;
} Item;

int main(int argc, char** argv) {
  int input = open(argv[1], O_RDONLY);
  if (input == -1) {
    close(input);
    return 0;
  }
  struct Item it = {-1, 0};
  int even_reads = 0;
  int shift = 0;

  while(1) {
    even_reads = read(input, &it + shift, sizeof(it) - shift);
    if (even_reads <= 0) {
      close(input);
      return 0;
    }
    shift += even_reads;
    if (sizeof(it) == shift) {
      break;
    }
  }

  while (1) {
    int shift = 0;
    int even_writes = 0;

   // /////////////////////////////////////
    char str[4096];
    int str_size = sprintf(str, "%d", it.value);

    while ((even_writes = write(1, str + shift, str_size - shift)) > 0) {
      shift += even_writes;
    }
    char space = ' ';
    write(1, &space, sizeof(space));
    // /////////////////////////////////////////

    if (it.next_pointer == 0) {
      return 0;
    }

   int even_reads = 0;
   shift = 0;

   lseek(input, it.next_pointer, SEEK_SET);

   while ((even_reads = read(input, &it + shift, sizeof(it) - shift)) > 0) {
     if (even_reads == 0) {
       break;
     }
     shift += even_reads;
   }
  }

  close(input);
  return 0;
}
