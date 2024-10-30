#ifndef LIGHTS_H
#define LIGHTS_H

#include <stdbool.h>

#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"
#include "utils/utils.h"

#define PRTE 'E'
#define PRTC 'C'

typedef struct {
    char port;
    uint8_t pin;
} PortPin;

void initLEDGPIO(void);

void onELight(uint8_t id);
void offELight(uint8_t id);

void onCLight(uint8_t id);
void offCLight(uint8_t id);

void onLight(PortPin portPin);
void offLight(PortPin portPin);

void onAllLights(PortPin lights[], size_t size);
void offAllLights(PortPin lights[], size_t size);

extern PortPin greenLights[10];
extern PortPin redLights;

extern bool isMoving;

void green_lights_thread(void *argument);
void red_light_thread(void *argument);

void initLightsRTOS();
#endif
