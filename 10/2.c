#define _GNU_SOURCE
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  int output = open(argv[1], O_RDWR | O_CREAT, 0640);
  int N = atoi(argv[2]);
  int W = atoi(argv[3]);
  if (W == 0) {
    return 0;
  }
  ftruncate(output, N*(N * W + 1));
  char* contents = (char*)mmap(NULL, N*(N * W + 1), PROT_WRITE, MAP_SHARED, output, 0);
  char* file_begin = contents;
  int* matr[N];
  for (size_t i = 0; i < N; ++i) {
    matr[i] = (int*)malloc(N * sizeof(int));
  }
  size_t size = N;
  int pivot = 1;
  for (int order = 0; order < size - 1; ++order) {
    for (int j = order; j < size - order; ++j) {
      matr[order][j] = pivot++;
    }
    for (int i = order + 1; i < size - order; ++i) {
      matr[i][size - 1 - order] = pivot++;
    }
    for (int j = size - order - 2; j > order; --j) {
      matr[size - 1 - order][j] = pivot++;
    }
    for (int i = size - 1 - order; i >= order + 1; --i) {
      matr[i][order] = pivot++;
    }
  }
  for (size_t i = 0; i < N; ++i) {
    for (size_t j = 0; j < N; ++j) {
      sprintf(contents, "%*d", W, matr[i][j]);
      contents += W;
    }
    sprintf(contents, "\n");
    ++contents;
  }

  for (size_t i = 0; i < N; ++i) {
    free(matr[i]);
  }

  munmap(file_begin, N*(N * W + 1));
  close(output);
  return 0;
}
