#include<stdio.h>

void erase(char* path, int i) {
  while (*(path + i + 1) != '\0') {
   *(path + i) = *(path+i+1);
   ++i;
  }
  *(path + i) = '\0';
}

int erase_nonused(char* path, int i) {
  int counter = 0;
  while (*(path + i) == '.' || *(path + i) == '/') {
    if (*(path + i) == '/') {
    } else {
      ++counter;
    }
    erase(path, i);
  }
  return counter;
}

void delete_slashs(char* path) {
  int i = 0;
  while (*(path + i) != '\0') {
    if (*(path + i) == '/' && *(path + i + 1) == '/') {
      erase(path, i);
      continue;
    }
    ++i;
  }
}

extern void normalize_path(char* path) {
  int i = 0;
  delete_slashs(path);
  while (*(path + i) == '.') {
    ++i;
  }
  while (*(path + i) != '\0') {
    if (*(path + i) == '\0') {
      break;
    }
    if (*(path + i) == '/') {
      ++i;
    }
    while (*(path + i) == '.') {
      int counter = erase_nonused(path, i);
      if (counter >= 3) {
        i+=3;
        continue;
      }
      i-=2;
      --counter;
      while (counter != 0) {
        while (*(path + i) != '/' && i!=-1) {
          erase(path, i);
          --i;
        }
        if (i == -1) {
          i = 1;
        }
        --i;
        --counter;
      }
    }
    ++i;
  }
  delete_slashs(path);
}
/*
int main() {
  char path[] = "/var/log/../lib/./ejexec";
  normalize_path(path);
  printf("%s", path);
  return 0;
}
*/
