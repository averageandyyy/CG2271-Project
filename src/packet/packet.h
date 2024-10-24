#ifndef PACKET_H
#define PACKET_H

#include <stdlib.h>

#define PACKET_SIZE 3

typedef struct packet_t {
    unsigned char x;
    unsigned char y;
    unsigned char command;
} packet_t;

typedef enum {
    PACKET_OK = 0,
    PACKET_INCOMPLETE = 1,
    PACKET_COMPLETE = 2
} result_t;
#endif
