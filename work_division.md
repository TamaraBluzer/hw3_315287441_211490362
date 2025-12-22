# Work Division: Tamara & Itamar

This document outlines the division of labor for the TCP Chat System and the project structure.

To ensure parallel work, we will use the **TCP 3-way handshake** (managed by the OS) followed by an **Application-Level Handshake** (managed by code) where the client sends its name.

## 🛠️ Tamara: Server Architect

**Goal:** Build the central "hub" that manages connections and routes traffic.

1. **TCP Setup:**
   - Create a socket, `bind` it to the port, and call `listen` to wait for the 3-way handshake.

2. **Acceptance & Handshake:**
   - Call `accept()` to complete the connection.
   - **The Handshake:** Immediately `read()` from the new socket to receive the client's name.
   - Print `"client <name> connected from <address>"`.

3. **Connection Tracking:**
   - Maintain a list (or array of structs) mapping each socket to a client name.

4. **Multiplexing:**
   - Use `select()` to monitor all client sockets at once.

5. **Message Logic:**
   - **Normal:** When a message arrives, broadcast it to everyone as `sourcename: message`.
   - **Whisper:** If it starts with `@`, find the "friend" in your list and send only to them.

6. **Cleanup:**
   - Detect disconnections and print `"client <name> disconnected"`.

---

## 💻 Itamar: Client & UI Developer

**Goal:** Build the interface that handles simultaneous user input and server messages.

1. **TCP Connection:**
   - Create a socket and call `connect()` to initiate the 3-way handshake with the server.

2. **The Handshake:**
   - Immediately after `connect()` succeeds, `write()` the client's `name` (from the command line) to the server.

3. **Non-Blocking UI:**
   - Use `select()` to monitor **two** things: `stdin` (keyboard) and the server socket.

4. **Input Handling:**
   - Read from `stdin`.
   - If the user types `!exit`, send it to the server, print `"client exiting"`, and close.
   - Otherwise, send the text to the server.

5. **Output Handling:**
   - When the server sends data, display it "as is" on the screen.

---

## 📂 File Structure & Skeleton

Directory: `hw3_id1_id2`

### 1. `chat.h` (Shared Structs)

Both sides need to agree on buffer sizes.

```c
#ifndef CHAT_H
#define CHAT_H

#define MAX_BUFFER 1024
#define MAX_NAME_LEN 50

/* While you can just send strings, a struct ensures the server 
   knows exactly how many bytes to read during the handshake. */
typedef struct {
    char name[MAX_NAME_LEN];
} RegistrationPacket;

#endif
```

### 2. `Makefile`

```makefile
CC = gcc
CFLAGS = -Wall -Wextra

all: hw3server hw3client

hw3server: hw3server.c chat.h
	$(CC) $(CFLAGS) -o hw3server hw3server.c

hw3client: hw3client.c chat.h
	$(CC) $(CFLAGS) -o hw3client hw3client.c

clean:
	rm -f hw3server hw3client
```

### 3. `hw3server.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "chat.h"

// Tamara: Start by implementing the socket, bind, listen loop here.
int main(int argc, char *argv[]) {
    return 0;
}
```

### 4. `hw3client.c`

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "chat.h"

// Itamar: Start by implementing connect and the select() loop here.
int main(int argc, char *argv[]) {
    return 0;
}
```
