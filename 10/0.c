#define FUSE_USE_VERSION 30
#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <fuse3/fuse.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

// Global pointer for usage in callbacks
static char *filesystem_source = NULL;

// Callback for stat syscall
int fuse_stat(const char *path, struct stat *st, struct fuse_file_info *fi)
{
  // Check if it is root directory
  if (strcmp(path, "/") == 0)
  {
    // Set file mode
    st->st_mode = 0555 | S_IFDIR;
    st->st_nlink = 2;

    return 0;
  }

  // Make temp pointer for iterating through data
  char *temp = filesystem_source;

  // Get file count
  size_t file_count = 0;
  sscanf(temp, "%d", &file_count);

  // Move to file list
  temp = strchr(temp, '\n');
  ++temp;

  // Iterate through file list
  int is_found = 0;
  off_t file_size = 0;
  for (size_t i = 0; i < file_count; ++i)
  {
    // Get file path and its size
    char curr_path[PATH_MAX];
    off_t curr_size = 0;
    sscanf(temp, "%s %d", curr_path, &curr_size);

    // Move to next file
    temp = strchr(temp, '\n');
    ++temp;

    // Check if its our file
    if (strcmp(curr_path, path + 1) == 0)
    {
      is_found = 1;
      file_size = curr_size;

      break;
    }
  }

  // If search failed
  if (!is_found)
  {
    return -ENOENT;
  }

  // Set file mode
  st->st_mode = S_IFREG | 0444;
  st->st_nlink = 1;
  st->st_size = file_size;

  return 0;
}

// Callback for readdir syscall
int fuse_readdir(const char *path, void *out, fuse_fill_dir_t filler, off_t off,
                 struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
  // Only directory is root
  if (strcmp(path, "/") != 0)
  {
    return -ENOENT;
  }

  // Directory and its parent
  filler(out, ".", NULL, 0, 0);
  filler(out, "..", NULL, 0, 0);

  // Make temp pointer for iterating through data
  char *temp = filesystem_source;

  // Get file count
  size_t file_count = 0;
  sscanf(temp, "%d\n", &file_count);

  // Move to file list
  temp = strchr(temp, '\n');
  ++temp;

  // Iterate through file list
  for (size_t i = 0; i < file_count; ++i)
  {
    // Get file path and its size
    char curr_path[PATH_MAX];
    off_t curr_size = 0;
    sscanf(temp, "%s %d\n", curr_path, &curr_size);

    // Move to next file
    temp = strchr(temp, '\n');
    ++temp;

    // Add file to readdir result
    filler(out, curr_path, NULL, 0, 0);
  }

  return 0;
}

// Callback for open syscall
int fuse_open(const char *path, struct fuse_file_info *fi)
{
  // Make temp pointer for iterating through data
  char *temp = filesystem_source;

  // Get file count
  size_t file_count = 0;
  sscanf(temp, "%d\n", &file_count);

  // Get file count
  temp = strchr(temp, '\n');
  ++temp;

  // Iterate through file list
  int is_found = 0;
  for (size_t i = 0; i < file_count; ++i)
  {
    // Get file path
    char curr_path[PATH_MAX];
    sscanf(temp, "%s\n", curr_path);

    // Move to next file
    temp = strchr(temp, '\n');
    ++temp;

    // Check if its our file
    if (strcmp(curr_path, path + 1) == 0)
    {
      is_found = 1;

      break;
    }
  }

  // If search failed
  if (!is_found)
  {
    return -ENOENT;
  }

  // Files can be opened only for read
  if (O_RDONLY != (fi->flags & O_ACCMODE))
  {
    return -EACCES;
  }

  return 0;
}

// Callback for read syscall
int fuse_read(const char *path, char *out, size_t size, off_t off, struct fuse_file_info *fi)
{
  // Make temp pointer for iterating through data
  char *temp = filesystem_source;

  // Get file count
  size_t file_count = 0;
  sscanf(temp, "%d\n", &file_count);

// Move to file list
  temp = strchr(temp, '\n');
  ++temp;

  // Iterate through file list
  int is_found = 0;
  off_t file_size = 0;
  off_t data_offset = 0;
  for (size_t i = 0; i < file_count; ++i)
  {
    // Get file path and its size
    char curr_path[PATH_MAX];
    off_t curr_size = 0;
    sscanf(temp, "%s %d\n", curr_path, &curr_size);

    // Move to next file
    temp = strchr(temp, '\n');
    ++temp;

    // Check if its our file
    if (strcmp(curr_path, path + 1) == 0)
    {
      is_found = 1;
      file_size = curr_size;
    }

    // If we haven't found file, we update its offset in data block
    if (!is_found)
    {
      data_offset += curr_size;
    }
  }

  // We are currently on \n symbol before data
  ++temp;
  // Now we're on data start

  // Offset can't be bigger than filesize
  if (off > file_size)
  {
    return 0;
  }

  // Read till the end if off + size is bigger than file size
  if (off + size > file_size)
  {
    size = file_size - off;
  }

  // Write file contents to out
  const void *data = temp + data_offset + off;
  memcpy(out, data, size);

  return size;
}

// Callbacks for FUSE daemon
struct fuse_operations callbacks = {
    .readdir = fuse_readdir,
    .getattr = fuse_stat,
    .open = fuse_open,
    .read = fuse_read,
};

// Struct for storing user-specified args
typedef struct
{
  char *src;
} fuse_options_t;

off_t get_file_size(int fd)
{
  struct stat st;
  int fstat_result = fstat(fd, &st);
  if (fstat_result == -1)
    perror("fstat");

  off_t file_size = st.st_size;

  return file_size;
}

int main(int argc, char *argv[]) {
  // Preprocess arguments for FUSE daemon
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

  //! Get user-specified command-line arguments =============================
  // Prepare fuse_options_t object
  fuse_options_t fuse_options;
  memset(&fuse_options, 0, sizeof(fuse_options));

  // Define patterns
  struct fuse_opt opt_specs[] = {
      {"--src %s", offsetof(fuse_options_t, src), 0},
      {NULL, 0, 0}};

  // Find patterns in {argc, argv} and remove them from it
  fuse_opt_parse(&args, &fuse_options, opt_specs, NULL);

  // Process user-specified arguments
  off_t file_size = 0;
  if (fuse_options.src) {
    int filesystem_source_fd = open(fuse_options.src, O_RDONLY);
    if (filesystem_source_fd == -1)
      perror("open");

    file_size = get_file_size(filesystem_source_fd);

    filesystem_source = mmap(NULL, file_size,
                             PROT_READ, MAP_SHARED,
                             filesystem_source_fd, 0);

    if (filesystem_source == MAP_FAILED)
      perror("mmap");

    int close_result = close(filesystem_source_fd);
    if (close_result == -1)
      perror("close");
  }
  //! =======================================================================

  // Run FUSE daemon
  int fuse_main_result = fuse_main(args.argc, args.argv, &callbacks, NULL);

  // Unmap mapped file
  int munmap_result = munmap(filesystem_source, file_size);
  if (munmap_result == -1)
    perror("munmap");
}
