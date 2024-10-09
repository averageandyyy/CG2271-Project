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
#include "utils.h"
/**
 * @brief Motor pin and channel definitions
 * Right side will use A1 and A2, corresponding to TPM2 C0 and C1
 * Left side will use C1 and C2, correspondint to TPM0 C0 and C1
 */
typedef enum
{
    STOP = 0,
    SLOW = 25,
    MEDIUM = 50,
    FAST = 100
} Speed;

typedef enum
{
    FORWARD,
    BACKWARD
} Direction;

/** @brief Defines the PWM period for a 500 Hz signal */
#define PWM_PERIOD 749

void initMotors(void);
void initGPIO(void);
void initTimers(void);

void stop(void);

void moveRightSide(Direction dir, Speed speed);
void moveLeftSide(Direction dir, Speed speed);

void moveForward(Speed speed);
void moveBackward(Speed speed);
void rotateLeft(Speed speed);
void rotateRight(Speed speed);
void curveLeft(Speed speed);

void moveTest(void);

#endif
