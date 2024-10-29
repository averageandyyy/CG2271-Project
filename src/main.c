#include <stdio.h>

#include "MKL25Z4.h"
#include "cirq/cirq.h"
#include "led/led.h"
#include "motors/motor_driver.h"
#include "packet/packet.h"
#include "serialize/serialize.h"
#include "utils/utils.h"

#define BAUD_RATE 9600
#define UART0_RX_PIN 1  // PortA Pin 1
#define UART0_TX_PIN 2  // PortA Pin 2
#define UART0_INT_PRIO 128
#define UART1_RX_PIN 1  // PortE Pin 1
#define UART1_TX_PIN 0  // PortE Pin 0
#define UART1_INT_PRIO 128
#define MASK(x) (1 << (x))

Q_t transmit0Q, receive0Q;
Q_t transmit1Q, receive1Q;
volatile char user_input_key; /* User input key read from serial port*/

// Init UART0 Interrupt
void initIntUART0(uint32_t baud_rate) {
    // UART0 clock gate enable
    SIM_SCGC4 |= SIM_SCGC4_UART0(1);

    // select UART0 clock source
    SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
    SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1);

    // configure UART0 TX/RX pins
    SIM_SCGC5 |= SIM_SCGC5_PORTA(1);
    PORTA_PCR(UART0_RX_PIN) = PORT_PCR_MUX(2);
    PORTA_PCR(UART0_TX_PIN) = PORT_PCR_MUX(2);

    // disable UART0
    UART0_C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

    uint32_t bus_clock = DEFAULT_SYSTEM_CLOCK;  // for some reason, no need to divide by 2 when system clock is 48MHz
    uint32_t divisor = bus_clock / (baud_rate * 16);
    UART0_BDH = UART_BDH_SBR(divisor >> 8);
    UART0_BDL = UART_BDL_SBR(divisor);

    UART0_C1 = 0;
    UART0_S2 = 0;
    UART0_C3 = 0;

    // enable UART0
    UART0_C2 |= UART_C2_TE_MASK | UART_C2_RE_MASK;

    Q_init(&transmit0Q);
    Q_init(&receive0Q);

    NVIC_SetPriority(UART0_IRQn, UART0_INT_PRIO);
    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART0_IRQn);

    // UART0_C2 |= UART_C2_TIE_MASK | UART_C2_RIE_MASK;
    UART0_C2 |= UART_C2_RIE_MASK;
}

// Init UART1 Interrupt
void initIntUART1(uint32_t baud_rate) {
    // UART1 clock gate enable
    SIM_SCGC4 |= SIM_SCGC4_UART1(1);

    // configure UART1 TX/RX pins
    SIM_SCGC5 |= SIM_SCGC5_PORTE(1);
    PORTE_PCR(UART1_RX_PIN) = PORT_PCR_MUX(3);
    PORTE_PCR(UART1_TX_PIN) = PORT_PCR_MUX(3);

    // disable UART1
    UART1_C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

    uint32_t bus_clock = DEFAULT_SYSTEM_CLOCK / 2;
    uint32_t divisor = bus_clock / (baud_rate * 16);
    UART1_BDH = UART_BDH_SBR(divisor >> 8);
    UART1_BDL = UART_BDL_SBR(divisor);

    UART1_C1 = 0;
    UART1_S2 = 0;
    UART1_C3 = 0;

    // enable UART1
    UART1_C2 |= UART_C2_TE_MASK | UART_C2_RE_MASK;

    Q_init(&transmit1Q);
    Q_init(&receive1Q);

    NVIC_SetPriority(UART1_IRQn, UART1_INT_PRIO);
    NVIC_ClearPendingIRQ(UART1_IRQn);
    NVIC_EnableIRQ(UART1_IRQn);

    UART1_C2 |= UART_C2_RIE_MASK;
}

void UART0_IRQHandler() {
    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_DisableIRQ(UART0_IRQn);
    // Transmit
    if (UART0_S1 & UART_S1_TDRE_MASK) {
        if (!Q_isEmpty(&transmit0Q)) {
            UART0_D = Q_dequeue(&transmit0Q);
        } else {
            UART0_C2 &= ~UART_C2_TIE_MASK;  // stop transmissions
        }
    }
    // Receive
    if (UART0_S1 & UART_S1_RDRF_MASK) {
        if (!Q_isFull(&receive0Q)) {
            Q_enqueue(&receive0Q, UART0_D);
        } else {
            // remove oldest packet and add new packet
            for (int i = 0; i < PACKET_SIZE; i++) {
                Q_dequeue(&receive1Q);
                Q_enqueue(&receive1Q, UART1_D);
            }
        }
    }
    // Error
    if (UART0_S1 & (UART_S1_OR_MASK | UART_S1_NF_MASK | UART_S1_FE_MASK | UART_S1_PF_MASK)) {
        // handle error
        // clear flag
    }

    NVIC_EnableIRQ(UART0_IRQn);
}

void UART1_IRQHandler() {
    NVIC_ClearPendingIRQ(UART1_IRQn);
    NVIC_DisableIRQ(UART1_IRQn);
    // Transmit
    if (UART1_S1 & UART_S1_TDRE_MASK) {
        if (!Q_isEmpty(&transmit1Q)) {
            UART1_D = Q_dequeue(&transmit1Q);
        } else {
            UART1_C2 &= ~UART_C2_TIE_MASK;  // stop transmissions
        }
    }
    // Receive
    if (UART1_S1 & UART_S1_RDRF_MASK) {
        if (!Q_isFull(&receive1Q)) {
            Q_enqueue(&receive1Q, UART1_D);
        } else {
            // remove oldest packet and add new packet
            for (int i = 0; i < PACKET_SIZE; i++) {
                Q_dequeue(&receive1Q);
                Q_enqueue(&receive1Q, UART1_D);
            }
        }
    }
    // Error
    if (UART1_S1 & (UART_S1_OR_MASK | UART_S1_NF_MASK | UART_S1_FE_MASK | UART_S1_PF_MASK)) {
        // handle error
        // clear flag
    }

    NVIC_EnableIRQ(UART1_IRQn);
}

static bool is_menu_displayed = false; /* Flag indicating menu status */

static void transmit_data(void* pdata, size_t size);

void controlLed(void) {
    user_input_key = Q_dequeue(&receive0Q);

    // Display menu on power-up
    if (!is_menu_displayed) {
        transmit_data("1: red\r\n2: green\r\n3: blue\r\n\r\n", 30);
        is_menu_displayed = true;
    }

    if (user_input_key == '1') {
        initLeds();
        onLed(RED);
        transmit_data("Red ON\r\n", 9);
    } else if (user_input_key == '2') {
        initLeds();
        onLed(GREEN);
        transmit_data("Green ON\r\n", 11);
    } else if (user_input_key == '3') {
        initLeds();
        onLed(BLUE);
        transmit_data("Blue ON\r\n", 10);
    } else if (user_input_key == '0') {
        initLeds();
        transmit_data("LEDs OFF\r\n", 11);
    }
}

static void transmit_data(void* pdata, size_t size) {
    // char buffer[70];
    // int len = serialize(buffer, pdata, size);
    // for (int i = 0; i < len; i++) {
    char* data = (char*)pdata;
    for (int i = 0; i < size; i++) {
        NVIC_DisableIRQ(UART0_IRQn);
        Q_enqueue(&transmit0Q, data[i]);
        NVIC_EnableIRQ(UART0_IRQn);
    }
    UART0_C2 |= UART_C2_TIE_MASK;
}

static void printString(char* str) {
    while (*str) {
        NVIC_DisableIRQ(UART0_IRQn);
        Q_enqueue(&transmit0Q, *str++);
        NVIC_EnableIRQ(UART0_IRQn);
    }
    UART0_C2 |= UART_C2_TIE_MASK;
}

static void printPacket(packet_t* packet) {
    char strX[4] = {0};
    char strY[4] = {0};
    char strCommand[4] = {0};
    sprintf(strX, "%03d", packet->x);
    sprintf(strY, "%03d", packet->y);
    sprintf(strCommand, "%03d", packet->command);

    printString(strX);
    printString(" ");
    printString(strY);
    printString(" ");
    printString(strCommand);
    printString("\r\n");
}

static void printMotor(motor_t* motor) {
    char lSpeedString[4] = {0};
    char rSpeedString[4] = {0};
    sprintf(lSpeedString, "%03d", motor->lSpeed);
    sprintf(rSpeedString, "%03d", motor->rSpeed);

    if (motor->lDir == FORWARD) {
        printString("F ");
    } else if (motor->lDir == BACKWARD) {
        printString("B ");
    }
    printString(lSpeedString);
    printString(" | ");

    if (motor->rDir == FORWARD) {
        printString("F ");
    } else if (motor->rDir == BACKWARD) {
        printString("B ");
    }
    printString(rSpeedString);
    printString("\r\n");
}

void receiveEspTest(void) {
    char buffer[PACKET_SIZE] = {};
    int count = 0;

    while (!Q_isEmpty(&receive1Q) && count < PACKET_SIZE) {
        buffer[count++] = Q_dequeue(&receive1Q);
    }

    packet_t packet;
    result_t result;
    if (count == 0) {
        result = PACKET_INCOMPLETE;
    } else {
        result = deserialize(buffer, count, &packet);
    }

    if (result == PACKET_OK) {
        motor_t motor;
        parsePacket(&packet, &motor);
        
		// printMotor(&motor);
		moveRobot(&motor);
    }
}

int main(void) {
    SystemCoreClockUpdate();

    initIntUART0(BAUD_RATE);
    initIntUART1(BAUD_RATE);
    // initRGBGPIO();
    // initLeds();
    
    initMotors();

    printString("\033[0H\033[0J");

    while (1) {
        // controlLed();
        receiveEspTest();
    }
}
