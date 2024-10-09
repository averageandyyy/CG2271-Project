/**
 * @file motor_driver.c
 * @brief Motor driver implementation for controlling four motors with the KL25Z and DRV8833.
 *
 * This file contains the implementation details for initializing the GPIO, timers,
 * and controlling motor speed using PWM signals on the KL25Z microcontroller.
 *
 * @author
 * Cheng Jia Wei Andy
 */

#include "motors/motor_driver.h"

/**
 * @brief Initializes all motors by setting up GPIO and timers.
 *
 * This function calls the necessary sub-functions to initialize the GPIO
 * pins and configure the TPM timers for PWM-based motor control.
 */
void initMotors(void)
{
    initGPIO();
    initTimers();
}

/**
 * @brief Configures the GPIO pins for motor control.
 *
 * This function enables the clocks for Ports A and C, and configures the pin
 * multiplexing to use the TPM (Timer/PWM Module) for generating PWM signals.
 */
void initGPIO(void)
{
    // Enable clock to Ports A and C
    SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTC_MASK);

    // Set MUX to ALT3 for TPM
    // A1 and A2
    PORTA->PCR[1] &= ~PORT_PCR_MUX_MASK;
    PORTA->PCR[1] |= PORT_PCR_MUX(3);
    PORTA->PCR[2] &= ~PORT_PCR_MUX_MASK;
    PORTA->PCR[2] |= PORT_PCR_MUX(3);

    // C1 and C2
    PORTC->PCR[1] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[1] |= PORT_PCR_MUX(3);
    PORTC->PCR[2] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[2] |= PORT_PCR_MUX(3);
}

/**
 * @brief Configures TPM timers for PWM generation.
 *
 * This function sets up the TPM0 and TPM2 timers with a prescaler of 128,
 * configures the PWM period, and enables edge-aligned PWM on the specified channels.
 */
void initTimers(void)
{
    // Enable clocks to TPM0 and TPM2
    SIM->SCGC6 |= (SIM_SCGC6_TPM0_MASK | SIM_SCGC6_TPM2_MASK);

    // Select internal clock source
    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    // Configure TPM0
    TPM0->MOD = PWM_PERIOD;
    TPM0->SC &= ~(TPM_SC_CMOD_MASK | TPM_SC_PS_MASK);
    TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));                     // Internal clock and prescaler of 128
    TPM0->SC &= ~TPM_SC_CPWMS_MASK;                                  // Up-counting
    TPM0->CONTROLS[0].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK; // Edge-aligned PWM on channel 0
    TPM0->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK; // Edge-aligned PWM on channel 1

    // Configure TPM2
    TPM2->MOD = PWM_PERIOD;
    TPM2->SC &= ~(TPM_SC_CMOD_MASK | TPM_SC_PS_MASK);
    TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));                     // Internal clock and prescaler of 128
    TPM2->SC &= ~TPM_SC_CPWMS_MASK;                                  // Up-counting
    TPM2->CONTROLS[0].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK; // Edge-aligned PWM on channel 0
    TPM2->CONTROLS[1].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSB_MASK; // Edge-aligned PWM on channel 1

    stop();
}

/**
 * @brief Stops all motors.
 *
 * This function stops all the motors by setting their PWM duty cycles to 0.
 */
void stop(void)
{
    TPM0->CONTROLS[0].CnV = 0;
    TPM0->CONTROLS[1].CnV = 0;
    TPM2->CONTROLS[0].CnV = 0;
    TPM2->CONTROLS[1].CnV = 0;
}

void moveForward(Speed speed)
{
    moveLeftSide(FORWARD, speed);
    moveRightSide(FORWARD, speed);
}

void moveBackward(Speed speed)
{
    moveLeftSide(BACKWARD, speed);
    moveRightSide(BACKWARD, speed);
}

void rotateLeft(Speed speed)
{
    moveRightSide(FORWARD, speed);
    moveLeftSide(BACKWARD, speed);
}

void rotateRight(Speed speed)
{
    moveRightSide(BACKWARD, speed);
    moveLeftSide(FORWARD, speed);
}

void curveLeft(Speed speed)
{
    moveRightSide(FORWARD, speed);
    moveLeftSide(FORWARD, speed / 2);
}

void curveRight(Speed speed)
{
    moveRightSide(FORWARD, speed / 2);
    moveLeftSide(FORWARD, speed);
}

void moveRightSide(Direction dir, Speed speed)
{
    uint16_t pwmValue = speed * PWM_PERIOD / 100;

    if (dir == FORWARD)
    {
        TPM2->CONTROLS[0].CnV = pwmValue;
        TPM2->CONTROLS[1].CnV = 0;
    }
    else
    {
        TPM2->CONTROLS[1].CnV = pwmValue;
        TPM2->CONTROLS[0].CnV = 0;
    }
}

void moveLeftSide(Direction dir, Speed speed)
{
    uint16_t pwmValue = speed * PWM_PERIOD / 100;

   if (dir == FORWARD)
    {
        TPM0->CONTROLS[0].CnV = pwmValue;
        TPM0->CONTROLS[1].CnV = 0;
    }
    else
    {
        TPM0->CONTROLS[1].CnV = pwmValue;
        TPM0->CONTROLS[0].CnV = 0;
    }
}

void moveTest(void)
{
    moveForward(MEDIUM);
    delay(DELAY_DURATION);
    moveBackward(MEDIUM);
    delay(DELAY_DURATION);

    rotateLeft(MEDIUM);
    delay(DELAY_DURATION);
    rotateRight(MEDIUM);
    delay(DELAY_DURATION);
    rotateRight(MEDIUM);
    delay(DELAY_DURATION);
    rotateLeft(MEDIUM);

    curveLeft(MEDIUM);
    delay(DELAY_DURATION);
    curveRight(MEDIUM);
    delay(DELAY_DURATION);
}
