#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char** argv) {

  uint32_t address = inet_addr(argv[1]);
  uint16_t sin_port = htons(atoi(argv[2]));

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = sin_port;
  addr.sin_addr.s_addr = address;

  int sock;
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("error");
    exit(1);
  }

  int conn;
  if ((conn = connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in))) == -1) {
    close(sock);
    perror("error");
    exit(1);
  }

  int curr_num, get_from_server;
  while(scanf("%d", &curr_num) != EOF) {
    if (write(sock, &curr_num, sizeof(int)) == 0){
      break;
    }
    if (read(sock, &get_from_server, sizeof(int)) == 0) {
      break;
    }
    printf("%d\n", get_from_server);
  }

  shutdown(sock, SHUT_RDWR);
  close(sock);
  return 0;
}
