#ifndef CHAT_H
#define CHAT_H

#include <stdint.h>

/* Required Definitions */
#define MAX_BUFFER 1024
#define MAX_NAME_LEN 50

/* Application Handshake Structure */
typedef struct {
    char name[MAX_NAME_LEN];
} chat_handshake_t;

#endif /* CHAT_H */
