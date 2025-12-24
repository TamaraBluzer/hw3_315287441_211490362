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

static int write_all(int fd, const void *buf, size_t n) {
  const char *p = (const char *)buf;
  size_t off = 0;
  while (off < n) {
    ssize_t w = write(fd, p + off, n - off);
    if (w < 0)
      return -1;
    off += (size_t)w;
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc != 4) {
    fprintf(stderr, "Usage: %s <host> <port> <name>\n", argv[0]);
    exit(1);
  }

  char *host = argv[1];
  int port = atoi(argv[2]);
  char *name = argv[3];

  // socket and connect
  struct hostent *hp = gethostbyname(host); // find IP address
  if (!hp) {
    fprintf(stderr, "%s: unknown host\n", host);
    exit(1);
  }

  int sock_fd;
  if ((sock_fd = socket(PF_INET, SOCK_STREAM, 0)) <
      0) {            // create socket endpoint using v4 and tcp
    perror("socket"); // PF_INET - IPv4, SOCK_STREAM - TCP, 0 - default protocol
    exit(1);
  }

  struct sockaddr_in server_addr;               // server address
  memset(&server_addr, 0, sizeof(server_addr)); // init to 0
  server_addr.sin_family = AF_INET;
  server_addr.sin_port =
      htons(port); // convert port to network byte order big endian
  memcpy(&server_addr.sin_addr, hp->h_addr, hp->h_length); // copy IP address

  if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) { // connect to server
    perror("connect");
    close(sock_fd);
    exit(1);
  }

  // Handshake Logic: immediately after connect(), write() the name to the
  // server
  chat_handshake_t handshake;
  // Ensure the name fits
  strncpy(handshake.name, name, MAX_NAME_LEN - 1);
  handshake.name[MAX_NAME_LEN - 1] = '\0';

  if (write_all(sock_fd, &handshake, sizeof(handshake)) <
      0) { // send name to server first so it can be added to the list
    perror("write handshake name");
    close(sock_fd);
    exit(1);
  }

  // select() loop to monitor both keyboard and the server socket
  fd_set read_fds; // set of file descriptors to monitor
  int max_fd = sock_fd > STDIN_FILENO
                   ? sock_fd
                   : STDIN_FILENO; // highest fd to pass to select

  while (1) {
    FD_ZERO(&read_fds);              // clear the set
    FD_SET(STDIN_FILENO, &read_fds); // add keyboard input
    FD_SET(sock_fd, &read_fds);      // add server socket

    if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) <
        0) { // wait for activity
      perror("select");
      break;
    }

    // Handle Server Message
    if (FD_ISSET(sock_fd, &read_fds)) { // if server socket is ready
      char buffer[MAX_BUFFER];
      ssize_t bytes_read =
          read(sock_fd, buffer, sizeof(buffer) - 1); // read from server
      if (bytes_read <= 0) {                         // if server disconnected
        printf("Server disconnected.\n");
        break;
      }
      buffer[bytes_read] = '\0';
      printf("%s\n", buffer);
    }

    // Handle User Input
    if (FD_ISSET(STDIN_FILENO, &read_fds)) { // if keyboard input is ready
      char buffer[MAX_BUFFER];

      if (fgets(buffer, sizeof(buffer), stdin) == NULL) { // EOF or error
        break;
      }

      // send to server
      size_t len = strlen(buffer);
      if (write_all(sock_fd, buffer, len) < 0) {
        perror("write");
        break;
      }

      // !exit command: compare without modifying buffer
      if (strcmp(buffer, "!exit\n") == 0 || strcmp(buffer, "!exit") == 0) {
        printf("client exiting\n");
        break;
      }
    }
  }

  close(sock_fd);
  return 0;
}
