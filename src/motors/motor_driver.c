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

void initMotors(void)
{
    initGPIO();
    initTimers();
}

void initGPIO(void)
{
    // Enable clock to Ports A and B
    SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK);

    // Set MUX to ALT3 for TPM
    // A1 and A2
    PORTA->PCR[1] &= ~PORT_PCR_MUX_MASK;
    PORTA->PCR[1] |= PORT_PCR_MUX(3);
    PORTA->PCR[2] &= ~PORT_PCR_MUX_MASK;
    PORTA->PCR[2] |= PORT_PCR_MUX(3);

    // B0 and B1
    PORTB->PCR[0] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[0] |= PORT_PCR_MUX(3);
    PORTB->PCR[1] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[1] |= PORT_PCR_MUX(3);
}

void initTimers(void)
{
    // Enable clocks to TPM1 and TPM2
    SIM->SCGC6 |= (SIM_SCGC6_TPM1_MASK | SIM_SCGC6_TPM2_MASK);

    // Select internal clock source
    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    // Configure TPM1
    TPM1->MOD = PWM_PERIOD;
    TPM1->SC &= ~(TPM_SC_CMOD_MASK | TPM_SC_PS_MASK);
    TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
    TPM1->SC &= ~TPM_SC_CPWMS_MASK;

    TPM1_C0SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSA_MASK);
    // Set to edge-aligned PWM (Clear output on match, set output on reload)
    TPM1_C0SC |= (TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1));

    TPM1_C1SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSA_MASK);
    TPM1_C1SC |= (TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1));

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

void stop(void)
{
    TPM2_C0V = 0;
    TPM2_C1V = 0;
    TPM1_C0V = 0;
    TPM1_C1V = 0;
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
        TPM1_C0V = pwmValue;
        TPM1_C1V = 0;
    }
    else
    {
        TPM1_C1V = pwmValue;
        TPM1_C0V = 0;
    }
}

void moveTest(void)
{
    moveForward(FAST);
    delay(DELAY_DURATION);
    moveBackward(FAST);
    delay(DELAY_DURATION);

    rotateLeft(FAST);
    delay(DELAY_DURATION);
    rotateRight(FAST);
    delay(DELAY_DURATION);
    rotateRight(FAST);
    delay(DELAY_DURATION);
    rotateLeft(FAST);
    delay(DELAY_DURATION);

    curveLeft(FAST);
    delay(DELAY_DURATION);
    curveRight(FAST);
    delay(DELAY_DURATION);
}
