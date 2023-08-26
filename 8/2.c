#define _FILE_OFFSET_BITS 64
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

uint32_t ReadByIdx(int file, uint32_t idx) {
  uint32_t value;
  lseek(file, sizeof(uint32_t) * idx, SEEK_SET);
  read(file, &value, sizeof(uint32_t));
  return value;
}

void WriteByIdx(int file, uint32_t value, uint32_t idx) {
  lseek(file, sizeof(uint32_t) * idx, SEEK_SET);
  write(file, &value, sizeof(uint32_t));
}

int main(int argc, char** argv) {
  int input = open(argv[1], O_RDWR);
  if (input == -1) {
    return 0;
  }

  uint32_t size = lseek(input, 0, SEEK_END) / sizeof(uint32_t);

  uint32_t val1;
  uint32_t val2;
  for (uint32_t j = size - 1; j > 0; --j) {
    for (uint32_t i = 0; i < j; ++i) {
      val1 = ReadByIdx(input, i);
      val2 = ReadByIdx(input, i + 1);
      if (val1 > val2) {
        uint32_t tmp = val1;
        val1 = val2;
        val2 = tmp;
      }
      WriteByIdx(input, val1, i);
      WriteByIdx(input, val2, i + 1);
    }
  }

  close(input);
  return 0;
}
