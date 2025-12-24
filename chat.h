#ifndef CHAT_H
#define CHAT_H

#include <stdint.h>

/* Required Definitions */
#define MAX_BUFFER 256
#define MAX_NAME_LEN 256

/* Application Handshake Structure */
typedef struct {
  char name[MAX_NAME_LEN];
} chat_handshake_t;

#endif /* CHAT_H */
