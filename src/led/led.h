#ifndef LED_H
#define LED_H

#include <stdbool.h>

#include "MKL25Z4.h"

#define RED_LED_PIN 18    // PortB Pin 18
#define GREEN_LED_PIN 19  // PortB Pin 19
#define BLUE_LED_PIN 1    // PortD Pin 1
#define MASK(x) (1 << (x))

typedef enum colour {
    RED,
    GREEN,
    BLUE
} colour_t;

// Init RGB LED
void initRGBGPIO(void);

void onLed(colour_t colour);

void offLed(colour_t colour);

void initLeds(void);
#endif
