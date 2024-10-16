#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <stdlib.h>

#define PACKET_SIZE 8

typedef enum {
    PACKET_OK = 0,
    PACKET_INCOMPLETE = 1,
    PACKET_COMPLETE = 2
} TResult;

int serialize(char *buffer, void *dataStructure, size_t size);

TResult deserialize(const char *buffer, int len, void *output);
#endif
