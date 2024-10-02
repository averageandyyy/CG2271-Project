/**
 * @file motor_driver.h
 * @brief Header file for motor functions for the KL25Z with DRV8833 Motor Driver.
 *
 * This file provides the necessary declarations and macros to control
 * the motors connected to the KL25Z board using PWM. Each motor is
 * controlled through specific GPIO pins and TPM channels.
 *
 * @author
 * Cheng Jia Wei Andy
 */
#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "MKL25Z4.h"

/**
 * @brief Motor pin and channel definitions
 *
 * FRONT_RIGHT  - Connected to PTA1, uses TPM2 Channel 0
 * FRONT_LEFT   - Connected to PTC1, uses TPM0 Channel 0
 * BACK_RIGHT   - Connected to PTA2, uses TPM2 Channel 1
 * BACK_LEFT    - Connected to PTC2, uses TPM0 Channel 1
 */
#define FRONT_RIGHT 1
#define FRONT_LEFT 1
#define BACK_RIGHT 2
#define BACK_LEFT 2

/** @brief Motor IDs for control purposes */
#define FRONT_RIGHT_ID 1
#define FRONT_LEFT_ID 2
#define BACK_RIGHT_ID 3
#define BACK_LEFT_ID 4

/** @brief Defines the PWM period for a 500 Hz signal */
#define PWM_PERIOD 749

void initMotors(void);
void initGPIO(void);
void initTimers(void);

void setMotorSpeed(uint8_t motor_id, uint8_t speed);
void stop(void);
#endif
