#include "utils.h"

void delay(volatile uint32_t nof) {
    while (nof != 0) {
        __asm("NOP");
        nof--;
    }
}

int normalise(int val) {
    return val - 128;  // 128 == mid-point of joystick values; value when joystick is centred
}

int constrain(int val, int low, int high) {
    if (val < low) {
        return low;
    }
    if (val > high) {
        return high;
    }
    return val;
}

int map(int x, int in_min, int in_max, int out_min, int out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
