#ifndef UTILS_H
#define UTILS_H
#include <stdint.h>
#include <stdlib.h>

#define DELAY_DURATION 0x80000

void delay(volatile uint32_t nof);

int normalise(int val);

int constrain(int val, int low, int high);

int map(int x, int in_min, int in_max, int out_min, int out_max);

#endif
