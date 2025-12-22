#include "chat.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CLIENTS 10

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(1);
  }

  int port = atoi(argv[1]);

  /* TODO: Tamara's Task */

  /* 1. Implement socket(), bind(), and listen() */

  /* 2. Use a select() loop to manage multiple clients simultaneously */

  /* 3. Handshake Logic: After accept(), immediately read() the client's name */

  /* 4. Add placeholders for 'Normal' (broadcast) and 'Whisper' (@name) message
   * parsing */

  return 0;
}
