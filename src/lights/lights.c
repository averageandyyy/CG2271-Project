#include "lights/lights.h"

void initLEDGPIO(void) {
    SIM->SCGC5 |= (SIM_SCGC5_PORTE_MASK | SIM_SCGC5_PORTC_MASK);

    // Set MUX for E ports 20 to 30 (6)
    PORTE->PCR[20] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[20] |= PORT_PCR_MUX(1);
    PORTE->PCR[21] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[21] |= PORT_PCR_MUX(1);
    PORTE->PCR[22] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[22] |= PORT_PCR_MUX(1);
    PORTE->PCR[23] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[23] |= PORT_PCR_MUX(1);
    PORTE->PCR[29] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[29] |= PORT_PCR_MUX(1);
    PORTE->PCR[30] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[30] |= PORT_PCR_MUX(1);

    // Set MUX for C ports (5)
    PORTC->PCR[6] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[6] |= PORT_PCR_MUX(1);
    PORTC->PCR[5] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[5] |= PORT_PCR_MUX(1);
    PORTC->PCR[4] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[4] |= PORT_PCR_MUX(1);
    PORTC->PCR[3] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[3] |= PORT_PCR_MUX(1);
    PORTC->PCR[0] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[0] |= PORT_PCR_MUX(1);

    // Set output directions
    PTE->PDDR |= (MASK(20) | MASK(21) | MASK(22) | MASK(23) | MASK(29) | MASK(30));

    PTC->PDDR |= (MASK(6) | MASK(5) | MASK(4) | MASK(3) | MASK(0));
}

void onELight(uint8_t id) { PTE->PSOR |= MASK(id); }

void offELight(uint8_t id) { PTE->PCOR |= MASK(id); }

void onCLight(uint8_t id) { PTC->PSOR |= MASK(id); }

void offCLight(uint8_t id) { PTC->PCOR |= MASK(id); }

void onLight(PortPin portPin) {
    if (portPin.port == PRTE) {
        onELight(portPin.pin);
    } else if (portPin.port == PRTC) {
        onCLight(portPin.pin);
    }
}

void offLight(PortPin portPin) {
    if (portPin.port == PRTE) {
        offELight(portPin.pin);
    } else if (portPin.port == PRTC) {
        offCLight(portPin.pin);
    }
}

PortPin greenLights[10] = {{PRTE, 20}, {PRTE, 21}, {PRTE, 22}, {PRTE, 23}, {PRTE, 29},
                           {PRTE, 30}, {PRTC, 6},  {PRTC, 5},  {PRTC, 4},  {PRTC, 3}};

PortPin redLight = {PRTC, 0};

bool isMoving = false;

void green_lights_thread(void *argument) {
    for (;;) {
        for (int i = 0; i < 10; i++) {
            if (i == 0) {
                offLight(greenLights[9]);
                onLight(greenLights[i]);
            } else {
                // Off the previous light
                offLight(greenLights[i - 1]);
                onLight(greenLights[i]);
            }

            // To test and confirm
            if (isMoving) {
                osDelay(250);
            } else {
                osDelay(500);
            }
        }
    }
}
