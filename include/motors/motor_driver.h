/**
 * @file motor_driver.h
 * @brief Header file for motor functions for the KL25Z with DRV8833 Motor Driver.
 *
 * This file provides the necessary declarations and macros to control
 * the motors connected to the KL25Z board using PWM. Each motor is
 * controlled through specific GPIO pins and TPM channels.
 *
 * Current setup:
 * Right wheels operating on TPM2_CH0 and TPM2_CH1 which are PTA1 and PTA2 respectively
 * Left wheels operating on TPM1_CH0 and TPM1_CH1 which are PTB0 and PTB1 respectively.
 *
 * @author
 * Cheng Jia Wei Andy
 */
#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "RTE_Components.h"
#include CMSIS_device_header
#include "cmsis_os2.h"
#include "utils.h"

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

/**
 * @brief Initializes all motors by setting up GPIO and timers.
 *
 * This function calls the necessary sub-functions to initialize the GPIO
 * pins and configure the TPM timers for PWM-based motor control.
 */
void initMotors(void);

/**
 * @brief Configures the GPIO pins for motor control.
 *
 * This function enables the clocks for Ports A and B, and configures the pin
 * multiplexing to use the TPM (Timer/PWM Module) for generating PWM signals.
 */
void initGPIO(void);

/**
 * @brief Configures TPM timers for PWM generation.
 *
 * This function sets up the TPM1 and TPM2 timers with a prescaler of 128,
 * configures the PWM period, and enables edge-aligned PWM on the specified channels.
 */
void initTimers(void);

/**
 * @brief Stops all motors.
 *
 * This function stops all the motors by setting their PWM duty cycles to 0.
 */
void stop(void);

void moveRightSide(Direction dir, Speed speed);
void moveLeftSide(Direction dir, Speed speed);

void moveForward(Speed speed);
void moveBackward(Speed speed);
void rotateLeft(Speed speed);
void rotateRight(Speed speed);
void curveLeft(Speed speed);

void moveTest(void);

// Dummy, to be further updated
typedef struct
{
    uint8_t x;
    uint8_t y;
} Movement;

// Global queue id, create this in main()
osMessageQueueId_t movementMsg;
extern bool isMoving = false;

void motorControlThread(void *argument)
{
    Movement movement;
    for (;;)
    {
        osMessageQueueGet(movementMsg, &movement, NULL, osWaitForever);
        // If we have non zero values somewhere, isMoving set to true
    }
}
#endif
