#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "packet/packet.h"

int serialize(char *buffer, void *dataStructure, size_t size);

result_t deserialize(const char *buffer, int len, void *output);
#endif
