#include "led/led.h"

// Init RGB LED
void initRGBGPIO(void) {
    // Enable Clock to PORTB and PORTD
    SIM_SCGC5 |= SIM_SCGC5_PORTB(1) | SIM_SCGC5_PORTD(1);

    // Configure MUX settings to make all 3 pins GPIO
    PORTB_PCR(RED_LED_PIN) &= ~PORT_PCR_MUX_MASK;
    PORTB_PCR(RED_LED_PIN) |= PORT_PCR_MUX(1);

    PORTB_PCR(GREEN_LED_PIN) &= ~PORT_PCR_MUX_MASK;
    PORTB_PCR(GREEN_LED_PIN) |= PORT_PCR_MUX(1);

    PORTD_PCR(BLUE_LED_PIN) &= ~PORT_PCR_MUX_MASK;
    PORTD_PCR(BLUE_LED_PIN) |= PORT_PCR_MUX(1);

    // Set Data Direction Registers for PortB and PortD
    GPIOB_PDDR |= MASK(RED_LED_PIN) | MASK(GREEN_LED_PIN);
    GPIOD_PDDR |= MASK(BLUE_LED_PIN);
}

void onLed(colour_t colour) {
    switch (colour) {
    case RED:		
		GPIOB_PCOR = MASK(RED_LED_PIN);
        break;
    case GREEN:
        GPIOB_PCOR = MASK(GREEN_LED_PIN);
        break;
    case BLUE:
        GPIOD_PCOR = MASK(BLUE_LED_PIN);
        break;
    default:
        break;
    }
}

void offLed(colour_t colour) {
    switch (colour) {
    case RED:
        GPIOB_PSOR = MASK(RED_LED_PIN);
        break;
    case GREEN:
        GPIOB_PSOR = MASK(GREEN_LED_PIN);
        break;
    case BLUE:
        GPIOD_PSOR = MASK(BLUE_LED_PIN);
        break;
    default:
        break;
    }
}

void initLeds(void) {
    offLed(RED);
    offLed(GREEN);
    offLed(BLUE);
}
