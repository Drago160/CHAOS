#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char** argv) {

  int input = open(argv[1], O_RDONLY);
  if (input == -1) {
    close(input);
    return 1;
  }
  int output_digit = open(argv[2], O_WRONLY | O_CREAT, 0640);
  int output_other = open(argv[3], O_WRONLY | O_CREAT, 0640);
  if (output_digit == -1 || output_other == -1) {
    close(input);
    close(output_digit);
    close(output_other);
    return 2;
  }

  size_t buff_reads_size = 0;
  char buff[4096];
  char digit_buff[4096];
  char other_buff[4096];
  size_t buff_max_size = 4096;
  size_t digit_size = 0;
  size_t other_size = 0;


  while ((buff_reads_size = read(input, buff, buff_max_size)) > 0) {
    digit_size = 0;
    other_size = 0;
    for (int i = 0; i < buff_reads_size; ++i) {
      if (buff[i] >= '0' && buff[i] <= '9') {
        digit_buff[digit_size] = buff[i];
        digit_size += 1;
      } else {
        other_buff[other_size] = buff[i];
        other_size += 1;
      }
    }

    int even_writes = 0;

    while (1) {
      even_writes = write(output_digit, digit_buff + even_writes, digit_size);
      if (even_writes == -1) {
        close(input);
        close(output_digit);
        close(output_other);
        return 3;
      }
      if (even_writes == 0) {
        break;
      }
      digit_size -= even_writes;
    }

    even_writes = 0;

    while (1) {
      even_writes = write(output_other, other_buff + even_writes, other_size);
      if (even_writes == -1) {
        close(input);
        close(output_digit);
        close(output_other);
        return 3;
      }
      if (even_writes == 0) {
        break;
      }
      other_size -= even_writes;
    }
  }
  if (buff_reads_size == -1) {
    return 3;
  }
  close(input);
  close(output_other);
  close(output_digit);
  return 0;
}
