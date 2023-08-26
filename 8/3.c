#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <windows.h>

struct Item {
  int value;
  uint32_t next_pointer;
} Item;

int main(int argc, char** argv) {
  HANDLE input = CreateFile(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
  //int input = open(argv[1], O_RDONLY);
  if (input == INVALID_HANDLE_VALUE) {
    return 0;
  }
  struct Item it = {-1, 0};
  DWORD even_reads;
  int shift = 0;

  while(1) {
    ReadFile(input, &it + shift, sizeof(it) - shift, &even_reads, NULL);
    if (even_reads <= 0) {
      CloseHandle(input);
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

    char str[4096];

    printf("%d ", it.value);

    if (it.next_pointer == 0) {
      CloseHandle(input);
      return 0;
    }

    DWORD even_reads;
    shift = 0;

    SetFilePointer(input, it.next_pointer, NULL, FILE_BEGIN);
    //lseek(input, it.next_pointer, SEEK_SET);

    while (1) {
      ReadFile(input, &it + shift, sizeof(it) - shift, &even_reads, NULL);
      shift += even_reads;
      if (sizeof(it) == shift) {
        break;
      }
    }
  }

  CloseHandle(input);
  return 0;
}

