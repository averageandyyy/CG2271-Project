#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <stdlib.h>

#define PACKET_SIZE 24

typedef enum {
    PACKET_OK = 0,
    PACKET_INCOMPLETE = 1,
    PACKET_COMPLETE = 2
} TResult;

typedef struct packet_t {
    unsigned char x;
    unsigned char y;
    unsigned char command;
} packet_t;

int serialize(char *buffer, void *dataStructure, size_t size);

TResult deserialize(const char *buffer, int len, void *output);
#endif
