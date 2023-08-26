#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <netdb.h>

off_t fsize(const char *filename) {
  struct stat st;

  if (stat(filename, &st) == 0)
    return st.st_size;

  return -1;
}

int server;

const char CONTENT_LENGTH_STRING[] = "Content-Length: ";
const char CONTENT_DELIMITER[] = "\r\n\r\n";

void safe_print(int fd, char* message, ssize_t len) {
  ssize_t also_written = write(fd, message, len);
  while (also_written < len) {
    ssize_t delta = write(fd, message + also_written, len - also_written);
    also_written += delta;
  }
}

void connect_to(char* host)
{
  struct addrinfo connect_info = {};
  connect_info.ai_family = AF_INET;
  connect_info.ai_socktype = SOCK_STREAM;
  struct addrinfo* addr = NULL;
  getaddrinfo(host, "http", &connect_info, &addr);

  server = socket(connect_info.ai_family, connect_info.ai_socktype, 0);
  connect(server, addr->ai_addr, addr->ai_addrlen);
  freeaddrinfo(addr);
}

void send_request(char* host, char* path, char* file_path)
{
  int fd = open(file_path, O_RDONLY);
  off_t file_size = fsize(file_path);
  char query[BUFSIZ] = {};
  ssize_t len = snprintf(query, BUFSIZ,
           "POST %s HTTP/1.1\r\n"
           "Host: %s\r\n"
           "Content-Type: multipart/form-data\r\n"
           "Connection: close\r\n"
           "Content-Length: %d\r\n"
           "\r\n",
           path, host, file_size);

  safe_print(server, query, len);
  sendfile(server, fd, NULL, file_size);
  write(server, "\r\n\r\n", 4);
  close(fd);
}

void disconnect()
{
  shutdown(server, 0);
  close(server);
}

void process_response()
{
  char response[BUFSIZ] = {};

  ssize_t recv_size = recv(server, response, BUFSIZ, 0);
  char* data_ptr = strstr(response, CONTENT_DELIMITER) + sizeof(CONTENT_DELIMITER) - 1;
  safe_print(1, data_ptr, recv_size - (data_ptr - response));
}

int main(int argc, char* argv[])
{
  connect_to(argv[1]);
  send_request(argv[1], argv[2], argv[3]);
  process_response();
  disconnect();

  return 0;
}
