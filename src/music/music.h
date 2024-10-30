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
#define C 1431
#define D_NOTE 1275
#define E 1136
#define F 1074
#define G 956
#define A 852
#define B 759

#define C4 1431  
#define D4 1275  
#define E4 1136  
#define F4 1074  
#define G4 956   
#define A4 852   
#define B4 759   
#define C5 717   
#define D5 639   
#define E5 569   
#define F5 537   
#define G5 478   

extern int mary[27];
extern int birthday[25];


void initMusic(void);
void initMusicGPIO(void);
void initMusicTimer(void);

extern bool isMary;

void music_thread(void* argument);

void initMusicRTOS(void);

#endif
