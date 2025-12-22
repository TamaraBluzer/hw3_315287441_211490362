#include "chat.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <host> <port> <name>\n", argv[0]);
    exit(1);
  }

  char *host = argv[1];
  int port = atoi(argv[2]);
  char *name = argv[3];

  /* TODO: Itamar's Task */

  /* 1. Implement socket() and connect() using command line arguments */

  /* 2. Handshake Logic: Immediately after connect(), write() the name to the
   * server */

  /* 3. Use a select() loop to monitor both stdin (keyboard) and the server
   * socket simultaneously */

  /* 4. Add a check for the !exit command to print 'client exiting' and close */

  return 0;
}
