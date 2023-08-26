#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

struct Icmp{
  uint8_t Type;
  uint8_t Code;
  uint16_t Checksum;
  uint16_t Identifier;
  uint16_t Sequence_Number;
} __attribute__((__packed__));

uint16_t RestoredSum(struct Icmp* bucket) {
  uint16_t code = bucket->Code;
  code <<= 8;
  return ~(code + bucket->Type + bucket->Identifier + bucket->Sequence_Number);
}

struct header_bucket{
  uint8_t header[20]; // header (20 bytest in IPv4)
  struct Icmp bucket;
} __attribute__((__packed__)) ;

size_t first_time = 0;
size_t get_time() {
  return time(NULL) - first_time;
}

int main(int argc, char** argv) {
  first_time = time(NULL);
  uint32_t address = inet_addr(argv[1]);
  size_t timeout = atoi(argv[2]);
  size_t interval = atoi(argv[3]);

  int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
  struct sockaddr_in addr;
  addr.sin_addr.s_addr=address;
  addr.sin_family=AF_INET;

  struct Icmp request;
  request.Type = 8; // Echo Request Type
  request.Code = 0; // Echo Request Code
  request.Checksum = 0;
  request.Identifier = getpid(); // unique number
  request.Sequence_Number = 0;

  size_t req_num = 0;

  while ((get_time()) < timeout) {
    request.Checksum = RestoredSum(&request);
    if (sendto(sock, &request, sizeof(request), 0, (struct sockaddr*) &addr, sizeof(addr)) < 1) {
      break;
    }
    struct header_bucket headered_ans;
    memset(&headered_ans, 0, sizeof(headered_ans)); // to set all bytes = 0;
    if (recvfrom(sock, &headered_ans, sizeof(headered_ans), 0, NULL, NULL) < 1) {
      break;
    }
    struct Icmp bucket = headered_ans.bucket;
    if (bucket.Identifier == getpid() && bucket.Type == 0 && bucket.Code == 0) {
      ++req_num;
    }
    usleep(interval);
  }
  shutdown(sock, SHUT_RDWR);
  close(sock);
  printf("%lu\n", req_num);
  return 0;
}
