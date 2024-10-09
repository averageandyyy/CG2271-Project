#ifndef BLINKY_H
#define BLINKY_H

#include "MKL25Z4.h"                    // Device header
#define RED_LED 18 // PTB18
#define GREEN_LED 19 // PTB19
#define BLUE_LED 1 // PTD1

#define MASK(x) (1 << (x)) // Bit shifting

#define NUM_COLORS 3


typedef enum
{
        RED,
        GREEN,
        BLUE
} color_t;

void initBlinkyGPIO(void);
void initBlinkyTimer(void);
void TPM1_IRQHandler(void);

void blinky_control(color_t color);

extern volatile color_t color_to_show;
#endif
