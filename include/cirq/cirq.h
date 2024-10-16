#ifndef CIRQ_H
#define CIRQ_H

#include <stdbool.h>
#include <stdint.h>

#define Q_SIZE 50

typedef struct Q_t {
    unsigned char Data[Q_SIZE];
    unsigned int Head;  // points to oldest data element
    unsigned int Tail;  // points to next free space
    unsigned int Size;  // quantity of elements in queue
} Q_t;

// Initialize circular queue
void Q_init(Q_t* q);

// Checks if the circular queue is empty
bool Q_isEmpty(Q_t* q);

// Checks if the circular queue is full
bool Q_isFull(Q_t* q);

int Q_enqueue(Q_t* q, unsigned char d);

unsigned char Q_dequeue(Q_t* q);

#endif
