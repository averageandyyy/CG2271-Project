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

#include <stdint.h>

#include "MKL25Z4.h"
#include "serialize/serialize.h"
#include "utils/utils.h"

typedef enum {
    FORWARD,
    BACKWARD
} Direction;

typedef struct motor_t {
    Direction lDir;
    unsigned char lSpeed;
    Direction rDir;
    unsigned char rSpeed;
} motor_t;

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
void initMotorGPIO(void);

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

void parsePacket(packet_t* packet, motor_t* settings);

void moveRobot(motor_t* motor_settings);
void moveRightSide(Direction dir, unsigned char speed);
void moveLeftSide(Direction dir, unsigned char speed);

#endif
