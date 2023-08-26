#define FUSE_USE_VERSION 30
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <fuse3/fuse.h>
#include <sys/mman.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

size_t dir_cnt = 1;
char** dir_names;

void DelimDirs(char* dirs) {
  dir_names = malloc(dir_cnt * sizeof(char*));
  char* curr = strtok(dirs, ":");
  dir_names[0] = realpath(curr, NULL);

  size_t i = 1;
  while ((curr = strtok(NULL, ":")) != NULL) {
    dir_names[i] = realpath(curr, NULL);
    if (curr == NULL) {
      break;
    }
    ++i;
  }
}

void SetDirs(char* dirs) {
  // set number of ":" + 1 = dir_cnt
  for (size_t i = 0; dirs[i] != '\0'; ++i) {
    if (dirs[i] == ':') {
      ++dir_cnt;
    }
  }
  // set dirs
  DelimDirs(dirs);
}

int earlier(const struct timespec* time1, const struct timespec* time2) {
  if (time1->tv_sec == time2->tv_sec) {
    return time1->tv_nsec < time2->tv_nsec;
  }
  return time1->tv_sec < time2->tv_sec;
}

void make_path(size_t dir_idx, const char* path, char out[BUFSIZ]) {
  strcpy(out, dir_names[dir_idx]);
  strcat(out, "/");
  strcat(out, path);
}

// find first dir(its idx) that contains file on path
size_t FindFileDirIdx(size_t idx, const char* path, struct stat* st) {
  while (idx < dir_cnt) {
    char full_path[BUFSIZ] = {};
    struct stat other_st;
    make_path(idx, path, full_path);
    if (stat(full_path, &other_st) != -1) {
      // update st if exists
      if (st != NULL) {
        *st = other_st;
      }
      return idx;
    }
    ++idx;
  }
  return dir_cnt;
}

void path_concat(const char* path, char* filename, char* buff) {
  strcpy(buff, path);
  strcat(buff, "/");
  strcat(buff, filename);
}

void iter_files(DIR* dir,
                const char* path,
                size_t pos,
                void* out,
                fuse_fill_dir_t filler) {
  struct dirent* file = NULL;
  while ((file = readdir(dir)) != NULL) {
    char curr_path[BUFSIZ];
    path_concat(path, file->d_name, curr_path);
    size_t other_pos = FindFileDirIdx(0, curr_path, NULL);
    if (other_pos != pos) { // if it's first entry in catalogs with same path
      continue;
    }
    filler(out, file->d_name, NULL, 0, 0);
  }
}

int fuse_readdir(const char* path, void* out, fuse_fill_dir_t filler, off_t off,
                 struct fuse_file_info* fi, enum fuse_readdir_flags flags) {
  struct stat curr;
  size_t pos = FindFileDirIdx(0, path, &curr);
  if (pos == dir_cnt) {
    return -ENOENT;
  }
  if (!S_ISDIR(curr.st_mode)) {
    return -ENOTDIR;
  }
  filler(out, ".", NULL, 0, 0);
  filler(out, "..", NULL, 0, 0);

  do {
    char full_dir_path[BUFSIZ] = {};
    make_path(pos, path, full_dir_path);

    DIR* dir = opendir(full_dir_path);
    iter_files(dir, path, pos, out, filler);
    closedir(dir);

  } while ((pos = FindFileDirIdx(pos + 1, path, NULL)) != dir_cnt);

  return 0;
}

int fuse_read(const char* path, char* out, size_t size, off_t off,
              struct fuse_file_info* fi) {
  int fd = fi->fh;
  struct stat st;
  if (fstat(fd, &st) == -1) {
    return -errno;
  }
  if (st.st_size <= off) {
    return 0;
  }
  lseek(fd, off, SEEK_SET);
  if (off + size >= st.st_size) {
    size = st.st_size - off;
  }

  size_t offset = 0;
  ssize_t also_read = read(fd, out + offset, size - offset);
  offset += also_read;
  while (also_read > 0) {
    also_read = read(fd, out + offset, size - offset);
    if (also_read == -1) {
      return -errno;
    }
    offset += also_read;
  }
  return offset;
}

void bind_dir(struct stat* st) {
  st->st_mode = S_IFDIR | 0555;
}

void bind_file(struct stat* st) {
  st->st_mode = S_IFREG | 0444;
}

int fuse_stat(const char* path, struct stat* st, struct fuse_file_info* fi) {
  size_t idx = FindFileDirIdx(0, path, st);
  if (idx == dir_cnt) { // if no such dir_idx
    return -ENOENT;
  }
  if (S_ISDIR(st->st_mode)) {
    bind_dir(st);
    return 0;
  }
  // iter over dirs and find latest modif date
  struct stat curr;
  while ((idx = FindFileDirIdx(idx + 1, path, &curr)) != dir_cnt) {
    if (!earlier(&curr.st_mtim, &st->st_mtim)) {
      *st = curr;
    }
  }

  bind_file(st);
  return 0;
}

int fuse_open(const char* path, struct fuse_file_info* fi) {
  struct stat last_st;
  size_t idx = FindFileDirIdx(0, path, &last_st);
  if (idx == dir_cnt) {
    return -ENOENT;
  }
  if ((fi->flags & O_ACCMODE) != O_RDONLY) {
    return -EACCES;
  }
  size_t last_idx = idx;
  struct stat other_st;
  while ((idx = FindFileDirIdx(idx + 1, path, &other_st)) != dir_cnt) {
    if (!earlier(&other_st.st_mtim, &last_st.st_mtim)) {
      last_st = other_st;
      last_idx = idx;
    }
  }
  char full_path[BUFSIZ];
  make_path(last_idx, path, full_path);
  int fd = open(full_path, O_RDONLY);
  fi->fh = fd;
  return 0;
}

int fuse_release(const char* path, struct fuse_file_info* fi) {
  close(fi->fh);
  return 42;
}

struct fuse_operations callbacks = {
    .readdir = fuse_readdir,
    .getattr = fuse_stat,
    .open = fuse_open,
    .read = fuse_read,
    .release = fuse_release
};

typedef struct {
  char* src;
} fuse_options_t;

int main(int argc, char** argv) {
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
  fuse_options_t fuse_options = {};

  struct fuse_opt opt_specs[] = {
      {"--src %s", offsetof(fuse_options_t, src), 0},
      {NULL, 0, 0}};
  fuse_opt_parse(&args, &fuse_options, opt_specs, NULL);
  if (!fuse_options.src) {
    perror("no src catalogs");
    exit(1);
  }
  SetDirs(fuse_options.src);
  int fuse_main_result = fuse_main(args.argc, args.argv, &callbacks, NULL);
  return fuse_main_result;
}
