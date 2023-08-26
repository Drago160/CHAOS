#include <stdio.h>
#include <signal.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <ctype.h>


volatile int end_flag = 0;
volatile int accept_end_flag = 0;

void SIGHandler() {
  end_flag = 1;
  if (accept_end_flag == 1) {
    exit(0);
  }
}

static void make_non_blocking(int fd) {
  int flags = fcntl(fd, F_GETFD);
  flags |= O_NONBLOCK;
  fcntl(fd, F_SETFL, flags);
}

void create_event(int epoll_fd, int fd) {
  make_non_blocking(fd);
  struct epoll_event event_ready_read;
  event_ready_read.events = EPOLLIN;
  event_ready_read.data.fd = fd;
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event_ready_read);
}

void handle_fd(int fd) {
  char buff[1024];
  int len = 0;
  len = read(fd, buff, 1024);
  for (int i = 0; i < len; ++i) {
    buff[i] = toupper(buff[i]);
  }
  write(fd, buff, len);
  close(fd);
}

int main(int argc, char* argv[]) {
  uint16_t port = htons(strtol(argv[1], NULL, 10));

  int epoll_fd = epoll_create(1);
  struct epoll_event pending[65536];

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

  create_event(epoll_fd, sock);

  while (end_flag == 0) {
    int n = epoll_wait(epoll_fd, pending, 65536, -1);
    for (uint16_t i = 0; i < n; ++i) {
      if (pending[i].data.fd == sock) {
        socklen_t size_of_addr = sizeof(addr);
        accept_end_flag = 1;
        int req_socket = accept(sock, (struct sockaddr*) &addr, &size_of_addr);
        accept_end_flag = 0;
        create_event(epoll_fd, req_socket);
      } else {
        handle_fd(pending[i].data.fd);
      }
    }
  }

  close(sock);

  return 0;
}
