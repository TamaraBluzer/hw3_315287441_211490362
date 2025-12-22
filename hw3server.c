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

#define MAX_CLIENTS 100

typedef struct {
  int fd;
  char name[MAX_NAME_LEN];
} client_t;

client_t clients[MAX_CLIENTS];
int num_clients = 0;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(1);
  }

  int port = atoi(argv[1]);

  /* TODO: Tamara's Task */

  /* 1. Implement socket(), bind(), and listen() */
  int server_fd;
  struct sockaddr_in server_addr;

  // create socket
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(1);
  }

  // to avoid address in use errors
  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    perror("setsockopt");
    exit(1);
  }

  // bind
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("bind");
    exit(1);
  }

  // set to listen state
  if (listen(server_fd, 5) < 0) {
    perror("listen");
    exit(1);
  }

  printf("Server listening on port %d...\n", port);

  /* 2. Select Loop to accecpt simultany multipe clients  */
  fd_set read_fds;
  int max_fd;

  while (1) {
    // Clear the set
    FD_ZERO(&read_fds);

    // Add server socket (to listen for new connections)
    FD_SET(server_fd, &read_fds);
    max_fd = server_fd;

    // Add all active client sockets
    for (int i = 0; i < num_clients; i++) {
      FD_SET(clients[i].fd, &read_fds);
      if (clients[i].fd > max_fd) {
        max_fd = clients[i].fd;
      }
    }

    // Wait for activity
    if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
      perror("select");
      continue;
    }

    /* 3. Handshake Logic */
    if (FD_ISSET(server_fd, &read_fds)) {
      struct sockaddr_in client_addr;
      socklen_t client_len = sizeof(client_addr);
      int new_fd =
          accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

      if (new_fd >= 0) {
        chat_handshake_t handshake;
        // Crucial: Wait for the name immediately!
        if (read(new_fd, &handshake, sizeof(handshake)) > 0) {
          // Determine if we have space
          if (num_clients < MAX_CLIENTS) {
            clients[num_clients].fd = new_fd;
            // Ensure safety (in case name is perfectly max length)
            handshake.name[MAX_NAME_LEN - 1] = '\0';
            strncpy(clients[num_clients].name, handshake.name, MAX_NAME_LEN);

            printf("client %s connected from %s\n", clients[num_clients].name,
                   inet_ntoa(client_addr.sin_addr));

            num_clients++;
          } else {
            printf("Server full, rejecting client.\n");
            close(new_fd);
          }
        } else {
          // Connection failed instantly or empty read
          close(new_fd);
        }
      }
    }

    /* 4. Handle Client Messages */
    for (int i = 0; i < num_clients; i++) {
      if (FD_ISSET(clients[i].fd, &read_fds)) {
        char buffer[MAX_BUFFER];
        ssize_t bytes_read = read(clients[i].fd, buffer, sizeof(buffer) - 1);

        if (bytes_read <= 0) {
          // Client disconnected or error
          // We need to implement remove_client() properly, but for now let's
          // just close and move the last one here
          printf("client %s disconnected\n", clients[i].name);
          close(clients[i].fd);
          clients[i] = clients[num_clients - 1]; // Move last to current
          num_clients--;
          i--; // Re-check this index since we swapped
        } else {
          buffer[bytes_read] = '\0';

          // Check if Whisper
          if (buffer[0] == '@') {
            // Format: @Name Message
            char *target_name = buffer + 1;
            char *msg_content = strchr(target_name, ' ');

            if (msg_content) {
              *msg_content = '\0'; // Split name and message
              msg_content++;       // Point to message

              // Find Target
              for (int j = 0; j < num_clients; j++) {
                if (strcmp(clients[j].name, target_name) == 0) {
                  char out[MAX_BUFFER + 50];
                  snprintf(out, sizeof(out), "%s: %s", clients[i].name,
                           msg_content);
                  write(clients[j].fd, out, strlen(out));
                  break;
                }
              }
            }
          } else {
            // Normal Broadcast
            char out[MAX_BUFFER + 50];
            snprintf(out, sizeof(out), "%s: %s", clients[i].name, buffer);
            for (int j = 0; j < num_clients; j++) {
              if (i != j) { // Don't echo to self
                write(clients[j].fd, out, strlen(out));
              }
            }
          }
        }
      }
    }

  } // End of while(1)

  return 0;
} // End of main
