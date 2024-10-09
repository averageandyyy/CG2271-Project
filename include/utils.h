#ifndef UTILS_H
#define UTILS_H
#include <cstdint>

#define DELAY_DURATION 0x800000

static void delay(volatile uint32_t nof)
{
    while (nof != 0)
    {
        __asm("NOP");
        nof--;
    }
}
#endif