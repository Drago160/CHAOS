#define _FILE_OFFSET_BITS 64
#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int main (int argc, char* argv[])
{
  int filename = open (argv[1], O_RDONLY);
  struct stat S;
  fstat (filename, &S);
  size_t size = S.st_size;
  char* contents = (char*)mmap(NULL, size, PROT_READ, MAP_PRIVATE, filename, 0);

  char* pos = contents;
  while ( (pos = memmem (pos, size, argv[2], strlen(argv[2]))) != NULL ) {
    size = S.st_size - (pos-contents) - 1;
    printf ("%llu ", pos - contents);
    ++pos;
  }
  munmap(contents, S.st_size);
  close(filename);
  return 0;
}
