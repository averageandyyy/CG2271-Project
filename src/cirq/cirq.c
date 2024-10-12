#include "cirq/cirq.h"

void Q_init(Q_t* q) {
    unsigned int i;
    for (i = 0; i < Q_SIZE; i++) q->Data[i] = 0;  // to simplify our lives when debugging
    q->Head = 0;
    q->Tail = 0;
    q->Size = 0;
}

bool Q_isEmpty(Q_t* q) { return q->Size == 0; }

bool Q_isFull(Q_t* q) { return q->Size == Q_SIZE; }

int Q_enqueue(Q_t* q, unsigned char d) {
    // What if queue is full?
    if (!Q_isFull(q)) {
        q->Data[q->Tail++] = d;
        q->Tail %= Q_SIZE;
        q->Size++;
        return 1;  // success
    } else
        return 0;  // failure
}
unsigned char Q_dequeue(Q_t* q) {
    // Must check to see if queue is empty before dequeueing
    unsigned char t = 0;
    if (!Q_isEmpty(q)) {
        t = q->Data[q->Head];
        q->Data[q->Head++] = 0;  // to simplify debugging
        q->Head %= Q_SIZE;
        q->Size--;
    }
    return t;
}
