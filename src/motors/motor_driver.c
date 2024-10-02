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
    PORTA->PCR[FRONT_RIGHT] &= ~PORT_PCR_MUX_MASK;
    PORTA->PCR[FRONT_RIGHT] |= PORT_PCR_MUX(3);
    PORTA->PCR[BACK_RIGHT] &= ~PORT_PCR_MUX_MASK;
    PORTA->PCR[BACK_RIGHT] |= PORT_PCR_MUX(3);
    PORTC->PCR[FRONT_LEFT] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[FRONT_LEFT] |= PORT_PCR_MUX(3);
    PORTC->PCR[BACK_LEFT] &= ~PORT_PCR_MUX_MASK;
    PORTC->PCR[BACK_LEFT] |= PORT_PCR_MUX(3);
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
}

/**
 * @brief Sets the speed of the specified motor.
 * 
 * This function adjusts the PWM duty cycle to control the speed of the motor.
 * The `motor_id` corresponds to a specific motor, and `speed` sets the duty cycle
 * from 0 (stopped) to 100 (full speed).
 * 
 * @param motor_id The ID of the motor (use defined motor IDs).
 * @param speed The speed percentage (0 to 100).
 */
void setMotorSpeed(uint8_t motor_id, uint8_t speed)
{
    uint16_t pwmValue = speed * PWM_PERIOD / 100;

    switch (motor_id)
    {
    case FRONT_RIGHT_ID:
        TPM2->CONTROLS[0].CnV = pwmValue;
        break;
    case FRONT_LEFT_ID:
        TPM0->CONTROLS[0].CnV = pwmValue;
        break;
    case BACK_RIGHT_ID:
        TPM2->CONTROLS[1].CnV = pwmValue;
        break;
    case BACK_LEFT_ID:
        TPM0->CONTROLS[1].CnV = pwmValue;
        break;
    default:
        stop();
    }
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
