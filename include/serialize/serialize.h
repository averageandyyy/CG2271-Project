#ifndef SERIALIZE_H
#define SERIALIZE_H

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

int serialize(char *buffer, void *dataStructure, size_t size);

result_t deserialize(const char *buffer, int len, void *output);
#endif
