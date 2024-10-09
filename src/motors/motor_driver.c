/**
 * @file motor_driver.c
 * @brief Motor driver implementation for controlling four motors with the KL25Z and DRV8833.
 *
 * This file contains the implementation details for initializing the GPIO, timers,
 * and controlling motor speed using PWM signals on the KL25Z microcontroller.
 *
 * Current setup:
 * Right wheels operating on TPM2_CH0 and TPM2_CH1 which are PTA1 and PTA2 respectively
 * Left wheels operating on TPM0_CH0 and TPM0_CH1 which are PTC1 and PTC2 respectively.
 * 
 * To look into alternative pinouts for TPM1 as PTC1 seems to be default high before the code runs
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
    PORTC->PCR[1] |= PORT_PCR_MUX(4);
    PORTC->PCR[2] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[2] |= PORT_PCR_MUX(4);
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
    SIM->SCGC6 |= (SIM_SCGC6_TPM2_MASK | SIM_SCGC6_TPM0_MASK);

    // Select internal clock source
    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    // Configure TPM0
    TPM0->MOD = PWM_PERIOD;
    TPM0->SC &= ~(TPM_SC_CMOD_MASK | TPM_SC_PS_MASK);
    TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
    TPM0->SC &= ~TPM_SC_CPWMS_MASK;

    TPM0_C0SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSA_MASK);
    // Set to edge-aligned PWM (Clear output on match, set output on reload)
    TPM0_C0SC |= (TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1));

    TPM0_C1SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSA_MASK);
    TPM0_C1SC |= (TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1));

    // Configure TPM2
    TPM2->MOD = PWM_PERIOD;
    TPM2->SC &= ~(TPM_SC_CMOD_MASK | TPM_SC_PS_MASK);
    TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
    TPM2->SC &= ~TPM_SC_CPWMS_MASK;

    TPM2_C0SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSA_MASK);
    // Set to edge-aligned PWM (Clear output on match, set output on reload)
    TPM2_C0SC |= (TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1));

    TPM2_C1SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSA_MASK);
    TPM2_C1SC |= (TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1));

    // For debugging
    // TPM2_C0V = 500;
    // TPM2_C1V = 0;
    // TPM1_C0V = 0;
    // TPM1_C1V = 0;
}

/**
 * @brief Stops all motors.
 *
 * This function stops all the motors by setting their PWM duty cycles to 0.
 */
void stop(void)
{
    TPM2_C0V = 0;
    TPM2_C1V = 0;
    TPM0_C0V = 0;
    TPM0_C1V = 0;
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
        TPM2_C0V = pwmValue;
        TPM2_C1V = 0;
    }
    else
    {
        TPM2_C1V = pwmValue;
        TPM2_C0V = 0;
    }
}

void moveLeftSide(Direction dir, Speed speed)
{
    uint16_t pwmValue = speed * PWM_PERIOD / 100;

    if (dir == FORWARD)
    {
        TPM0_C0V = pwmValue;
        TPM0_C1V = 0;
    }
    else
    {
        TPM0_C1V = pwmValue;
        TPM0_C0V = 0;
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
