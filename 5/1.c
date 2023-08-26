#include <stdio.h>
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>

volatile int end_flag = 0;
volatile int accept_end_flag = 0;

void SIGHandler() {
  end_flag = 1;
  if (accept_end_flag == 1) {
    exit(0);
  }
}

void MakeFullFilePath(char* path, char* name_of_file, char* path_to_file) {
  strcat(path_to_file, path);
  strcat(path_to_file, "/");
  strcat(path_to_file, name_of_file);
}

void PrintLen(char* path_to_file, int req_socket) {
  struct stat st;
  lstat(path_to_file, &st);
  int size_of_file = st.st_size;

  char size_str[1024];
  sprintf(size_str, "Content-Length: %d\r\n\r\n", size_of_file);
  write(req_socket, size_str, strlen(size_str));
}

void ReWriteFile(char* path_to_file, int req_socket) {
  char buff[1024] = {};
  int curr_file = open(path_to_file, O_RDONLY);
  int len = 0;
  while ((len = read(curr_file, buff, 1024)) != 0) {
    write(req_socket, buff, len);
  }
}

int main(int argc, char* argv[]) {
  uint16_t port = htons(strtol(argv[1], NULL, 10));
  char* path = argv[2];

  struct sigaction sig_handler;
  memset(&sig_handler, 0, sizeof(sig_handler));
  sig_handler.sa_flags = SA_RESTART,
  sig_handler.sa_handler = SIGHandler;
  sigaction(SIGTERM, &sig_handler, NULL);
  sigaction(SIGINT, &sig_handler, NULL);
  

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = port;
  addr.sin_addr.s_addr = inet_addr("127.0.0.1");


  int sock = socket(AF_INET, SOCK_STREAM, 0);

  bind(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
  listen(sock, SOMAXCONN);


  while (end_flag == 0) {
    socklen_t size_of_addr = sizeof(addr);

    accept_end_flag = 1;
    int req_socket = accept(sock, (struct sockaddr*) &addr, &size_of_addr);
    accept_end_flag = 0;

    char name_of_file[512];

    int for_file_fd = dup(req_socket); // to read from file
    FILE* read_fd = fdopen(for_file_fd, "r");
    fscanf(read_fd, "GET %s HTTP/1.1", name_of_file);
    fclose(read_fd);

    char path_to_file[1024] = {};
    MakeFullFilePath(path, name_of_file, path_to_file);

    if (access(path_to_file, F_OK) != 0) {
      write(req_socket, "HTTP/1.1 404 Not Found\r\n", sizeof("HTTP/1.1 404 Not Found\r\n"));
      write(req_socket, "Content-Length: 0\r\n\r\n", sizeof("Content-Length: 0\r\n\r\n"));
      close(req_socket);
    } else
    if (access(path_to_file, R_OK) != 0) {
      write(req_socket, "HTTP/1.1 403 Forbidden\r\n", sizeof("HTTP/1.1 403 Forbidden\r\n"));
      write(req_socket, "Content-Length: 0\r\n\r\n", sizeof("Content-Length: 0\r\n\r\n"));
      close(req_socket);
    } else {
      write(req_socket, "HTTP/1.1 200 OK\r\n", strlen("HTTP/1.1 200 OK\r\n"));

      PrintLen(path_to_file, req_socket);

      ReWriteFile(path_to_file, req_socket);

      close(req_socket);
    }
  }

  close(sock);
  return 0;
}
