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

  uint32_t address = inet_addr("127.0.0.1");
  uint16_t sin_port = htons(atoi(argv[1]));

  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = sin_port;
  addr.sin_addr.s_addr = address;

  int sock;
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    perror("error");
    exit(1);
  }

  /*
  int conn;
  FOR TCP
  conn = connect(sock, (struct sockaddr*)&addr, sizeof(struct sockaddr_in))
  */


  int curr_num, get_from_server;
  while(scanf("%d", &curr_num) != EOF) {
    if (sendto(sock, &curr_num, sizeof(int), 0,  (const struct sockaddr*) &addr, sizeof(addr)) < 1) {
      break;
    }
    if (recvfrom(sock, &get_from_server, sizeof(int), 0, NULL, NULL) < 1){
      break;
    }
    printf("%d\n", get_from_server);
  }

  shutdown(sock, SHUT_RDWR);
  close(sock);
  return 0;

