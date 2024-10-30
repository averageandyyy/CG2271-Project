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

void initMotors(void) {
    initMotorGPIO();
    initMotorTimers();
}

void initMotorGPIO(void) {
    // Enable clock to Ports A and B
    SIM->SCGC5 |= (SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK);

    // Set MUX to ALT3 for TPM
    PORTA->PCR[RIGHT_GREEN_FORWARD_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTA->PCR[RIGHT_GREEN_FORWARD_PIN] |= PORT_PCR_MUX(3);
    PORTA->PCR[RIGHT_BLUE_BACK_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTA->PCR[RIGHT_BLUE_BACK_PIN] |= PORT_PCR_MUX(3);

    PORTB->PCR[LEFT_GREEN_FORWARD_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[LEFT_GREEN_FORWARD_PIN] |= PORT_PCR_MUX(3);
    PORTB->PCR[LEFT_BLUE_BACK_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[LEFT_BLUE_BACK_PIN] |= PORT_PCR_MUX(3);
}

void initMotorTimers(void) {
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

void stop(void) {
    TPM2_C0V = 0;
    TPM2_C1V = 0;
    TPM1_C0V = 0;
    TPM1_C1V = 0;
}

void parsePacket(packet_t* packet, motor_t* settings) {
    int x = normalise((int) packet->x);
    int y = normalise((int) packet->y);

    int lMotorVelocity = constrain(x - y, -128, 127);
    int rMotorVelocity = constrain(-x - y, -128, 127);

    settings->lSpeed = map(abs(lMotorVelocity), 0, 127, 0, 100);
    settings->rSpeed = map(abs(rMotorVelocity), 0, 127, 0, 100);

    settings->lDir = (lMotorVelocity >= 0) ? FORWARD : BACKWARD;
    settings->rDir = (rMotorVelocity >= 0) ? FORWARD : BACKWARD;
}

void moveRightSide(Direction dir, unsigned char speed) {
    uint16_t pwmValue = speed * PWM_PERIOD / 100;

    if (dir == FORWARD) {
        TPM2_C0V = pwmValue;
        TPM2_C1V = 0;
    } else {
        TPM2_C1V = pwmValue;
        TPM2_C0V = 0;
    }
}

void moveLeftSide(Direction dir, unsigned char speed) {
    uint16_t pwmValue = speed * PWM_PERIOD / 100;

    if (dir == FORWARD) {
        TPM1_C0V = pwmValue;
        TPM1_C1V = 0;
    } else {
        TPM1_C1V = pwmValue;
        TPM1_C0V = 0;
    }
}

void moveRobot(motor_t* settings) {
    moveLeftSide(settings->lDir, settings->lSpeed);
    moveRightSide(settings->rDir, settings->rSpeed);
    osDelay(10); // To be tested
}

osMessageQueueId_t motorMsg;

void motor_control_thread(void* argument) {
    motor_t myMotor;
    for (;;) {
        // Get motor message from queue, blocks and allows other threads to run if no message is received
        osMessageQueueGet(motorMsg, &myMotor, NULL, osWaitForever);
        isMoving = true;
        moveRobot(&myMotor);
    }
}

void initMotorControlRTOS(void) {
    // Initialize motor message queue
    motorMsg = osMessageQueueNew(MSG_COUNT, sizeof(motor_t), NULL);
    // Initialize motor control thread
    osThreadNew(motor_control_thread, NULL, NULL);
}
