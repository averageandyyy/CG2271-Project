#include "labs/blinky.h"

volatile color_t color_to_show = RED;

void initBlinkyGPIO(void)
{
    // Enable clocks to ports B and D
    SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));

    // Reset pin MUX and set them to GPIO. Refer to Pinout for ALT1
    PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1); // 1 for GPIO

    PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
    PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1); // 1 for GPIO

    PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1); // 1 for GPIO

    // Set data direction
    PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
    PTD->PDDR |= MASK(BLUE_LED);

    // Off all lights
    PTB->PCOR |= (MASK(RED_LED) | MASK(GREEN_LED));
    PTD->PCOR |= MASK(BLUE_LED);
}

void initBlinkyTimer(void)
{
    // Enable clock for TPM1
    SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;

    // Select clock source for TPM1
    SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); // MGCFLLCLK at 48MHZ

    // Reset clock mode(?) and prescaler
    TPM1->SC &= ~(TPM_SC_CMOD_MASK | TPM_SC_PS_MASK);
    // Select LPTPM Counter increments on every LPTPM counter clock and prescaler of 128
    TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));

    // Upcounting
    TPM1->SC &= TPM_SC_CPWMS_MASK;

    // Enable timer overflow interrupt
    TPM1->SC |= TPM_SC_TOIE(1);

    // Set MOD
    TPM1->MOD = 156250;

    // Enable TPM1 interrupt in NVIC
    NVIC_EnableIRQ(TPM1_IRQn);
}

void TPM1_IRQHandler(void)
{
        NVIC_ClearPendingIRQ(TPM1_IRQn);
        if (TPM1->SC & TPM_SC_TOF(1))
        {
                color_to_show = (color_to_show + 1) % NUM_COLORS;

                // Clear interrupt flag by writing 1 to it
                TPM1->SC |= TPM_SC_TOF(1);
        }

}

void blinky_control(color_t color)
{
        if (color == RED)
        {
                PTB->PCOR |= MASK(RED_LED);
                PTB->PSOR |= MASK(GREEN_LED);
                PTD->PSOR |= MASK(BLUE_LED);
        }
        else if (color == GREEN)
        {
                PTB->PCOR |= MASK(GREEN_LED);
                PTB->PSOR |= MASK(RED_LED);
                PTD->PSOR |= MASK(BLUE_LED);
        }
        else if (color == BLUE)
        {
                PTD->PCOR |= MASK(BLUE_LED);
                PTB->PSOR |= MASK(GREEN_LED);
                PTB->PSOR |= MASK(RED_LED);
        }
}
