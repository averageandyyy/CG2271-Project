/*
 * Motors are using TPM1 and TPM2, so music PWM must use TPM0
 * Use PTE31 corresponds to TPM0_CH4 under ALT3
 */

#ifndef MUSIC_H
#define MUSIC_H

#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"
#include "utils/utils.h"
// For global isMoving variable
#include "lights/lights.h"

#define MUSIC_PIN 31
#define C 2294
#define D_NOTE 2044
#define E 1820
#define F 1717
#define G 1531
#define A 1364
#define B 1215

#define C4 2294
#define D4 2044
#define E4 1820
#define F4 1717
#define G4 1531
#define A4 1364
#define B4 1215
#define C5 1145
#define D5 1022
#define E5 912
#define F5 859
#define G5 765

extern int mary[27];
extern int birthday[25];

void initMusic(void);
void initMusicGPIO(void);
void initMusicTimer(void);

extern bool isMary;

void music_thread(void *argument);

void initMusicRTOS(void);

#endif
