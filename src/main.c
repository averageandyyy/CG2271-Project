#include "MKL25Z4.h"
#include "cirq/cirq.h"
#include "serialize/serialize.h"

#define BAUD_RATE 9600
#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define UART0_INT_PRIO 128
#define RED_LED 18    // PortB Pin 18
#define GREEN_LED 19  // PortB Pin 19
#define BLUE_LED 1    // PortD Pin 1
#define MASK(x) (1 << (x))

typedef enum colour {
    RED,
    GREEN,
    BLUE
} colour_t;

Q_t transmitQ, receiveQ;
volatile char user_input_key; /* User input key read from serial port*/

// Init UART Interrupt
void initIntUART0(uint32_t baud_rate) {
    // UART0 clock gate enable
    SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;

    // select UART0 clock source
    SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
    SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1);

    // configure UART0 TX/RX pins
    SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
    PORTA_PCR1 = PORT_PCR_MUX(2);
    PORTA_PCR2 = PORT_PCR_MUX(2);

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

    Q_init(&transmitQ);
    Q_init(&receiveQ);

    NVIC_SetPriority(UART0_IRQn, UART0_INT_PRIO);
    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_EnableIRQ(UART0_IRQn);

    // UART0_C2 |= UART_C2_TIE_MASK | UART_C2_RIE_MASK;
    UART0_C2 |= UART_C2_RIE_MASK;
}

// Init RGB LED
void initGPIO(void) {
    // Enable Clock to PORTB and PORTD
    SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));

    // Configure MUX settings to make all 3 pins GPIO
    PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);

    PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);

    PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);

    // Set Data Direction Registers for PortB and PortD
    PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
    PTD->PDDR |= MASK(BLUE_LED);
}

void onLed(colour_t colour) {
    switch (colour) {
    case RED:
        GPIO_PCOR_REG(PTB) = MASK(RED_LED);
        break;
    case GREEN:
        GPIO_PCOR_REG(PTB) = MASK(GREEN_LED);
        break;
    case BLUE:
        GPIO_PCOR_REG(PTD) = MASK(BLUE_LED);
        break;
    default:
        break;
    }
}

void offLed(colour_t colour) {
    switch (colour) {
    case RED:
        GPIO_PSOR_REG(PTB) = MASK(RED_LED);
        break;
    case GREEN:
        GPIO_PSOR_REG(PTB) = MASK(GREEN_LED);
        break;
    case BLUE:
        GPIO_PSOR_REG(PTD) = MASK(BLUE_LED);
        break;
    default:
        break;
    }
}

void initLeds() {
    offLed(RED);
    offLed(GREEN);
    offLed(BLUE);
}

void UART0_IRQHandler() {
    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_DisableIRQ(UART0_IRQn);
    // Transmit
    if (UART0_S1 & UART_S1_TDRE_MASK) {
        if (!Q_isEmpty(&transmitQ)) {
            UART0_D = Q_dequeue(&transmitQ);
        } else {
            UART0_C2 &= ~UART_C2_TIE_MASK;  // stop transmissions
        }
    }
    // Receive
    if (UART0_S1 & UART_S1_RDRF_MASK) {
        if (!Q_isFull(&receiveQ)) {
            Q_enqueue(&receiveQ, UART0_D);
        } else {
            Q_dequeue(&receiveQ);           // remove oldest data
            Q_enqueue(&receiveQ, UART0_D);  // add new data
        }
    }
    // Error
    if (UART0_S1 & (UART_S1_OR_MASK | UART_S1_NF_MASK | UART_S1_FE_MASK | UART_S1_PF_MASK)) {
        // handle error
        // clear flag
    }

    NVIC_EnableIRQ(UART0_IRQn);
}

// Delay routine
static void delay(volatile uint32_t nof) {
    while (nof != 0) {
        __asm("NOP");
        nof--;
    }
}

#pragma region LedControl

static bool is_menu_displayed = false; /* Flag indicating menu status */

// Local function prototypes
static void transmit_data(char *pdata, size_t size);

void controlLed(void) {
    user_input_key = Q_dequeue(&receiveQ);

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

    // // Do periodic logging (every 5sec)
    // if (logging_started) {
    //     do_log_count++;
    //     if (do_log_count >= 5) {
    //         do_log_count = 0;
    //         transmit_data("My UART\r\n", 10);
    //     }
    // }
}

static void transmit_data(char *pdata, size_t size) {
    // char buffer[70];
    // int len = serialize(buffer, pdata, size);
    // for (int i = 0; i < len; i++) {
    while (*pdata) {
        __disable_irq();
        Q_enqueue(&transmitQ, *pdata);
        pdata++;
        __enable_irq();
    }
    UART0_C2 |= UART_C2_TIE_MASK;

    // // Wait until complete string is transmitted on serial port
    // // and every byte is shifted out of Transmit buffer before
    // // loading new byte
    // while (*pdata) {
    //     __disable_irq();
    //     UART0->D = *pdata;

    //     // Wait until byte is transmitted from Tx Buffer
    //     while (!(UART0->S1 & UART_S1_TDRE_MASK)) {
    //     }
    //     __enable_irq();

    //     pdata++;
    // }
}

#pragma endregion LedControl

int main(void) {
    // uint8_t rx_data = 0x0;
    SystemCoreClockUpdate();

    initIntUART0(BAUD_RATE);
    initGPIO();
    initLeds();

    transmit_data("\033[0H\033[0J", 9);

    while (1) {
        controlLed();
        // UART0_Print("x");
        // delay(0xC000);
    }
}
