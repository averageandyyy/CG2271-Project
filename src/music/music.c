#include "music/music.h"

int mary[27] = {
    E, D, C, D, E, E, E, D, D, D, D, E, G, G, E, D, C, D, E, E, E, C, D, D, E, D, C
};

int birthday[25] = {
    G4, G4, A4, G4, C5, B4,  
    G4, G4, A4, G4, D5, C5,  
    G4, G4, G5, E5, C5, B4, A4,  
    F5, F5, E5, C5, D5, C5
};

void initMusic(void) {
    initMusicGPIO();
    initMusicTimer();
}

void initMusicGPIO(void) {
    // Enable clock to PORTE
    SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;

    // Reset MUX and choose ALT3 for TPM0_CH4
    PORTE->PCR[MUSIC_PIN] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[MUSIC_PIN] |= PORT_PCR_MUX(3);
}

void initMusicTimer(void) {
    // Enable clock to TPM0
    SIM_SCGC6 |= SIM_SCGC6_TPM0_MASK;

    SIM_SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1);

    TPM0->SC &= ~(TPM_SC_CMOD_MASK | TPM_SC_PS_MASK);
    TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
    TPM0->SC &= ~TPM_SC_CPWMS_MASK;

    TPM0_C4SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSA_MASK);
    // Set to edge-aligned PWM (Clear output on match, set output on reload)
    TPM0_C4SC |= (TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1));
}

bool isMary = true;

void music_thread(void* argument) {
    for (;;) {
        if (isMary) {
            for (int i = 0; i < 27 && isMary; i++) {
                TPM0_MOD = mary[i];
                TPM0_C4V = mary[i] / 2;
                osDelay(100);
            }
        } else {
            for (int i = 0; i < 25 && !isMary; i++) {
                TPM0_MOD = birthday[i];
                TPM0_C4V = birthday[i] / 2;
                osDelay(100);
            }
        }
    }
}

void initMusicRTOS(void) {
    osThreadNew(music_thread, NULL, NULL);
}
