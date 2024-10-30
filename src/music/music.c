#include "music/music.h"

int mary[27] = {
    E, D_NOTE, C, D_NOTE, E, E, E, D_NOTE, D_NOTE, D_NOTE, D_NOTE, E, G, G, E, D_NOTE, C, D_NOTE, E, E, E, C, D_NOTE, D_NOTE, E, D_NOTE, C
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
    SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;

    PORTE_PCR(31) &= ~PORT_PCR_MUX_MASK;
    PORTE_PCR(31) |= PORT_PCR_MUX(3);
}

void initMusicTimer(void) {
    SIM_SCGC6 |= SIM_SCGC6_TPM0_MASK;

    SIM_SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
    SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1);

    TPM0_SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
    TPM0_SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));  // ps=128
    TPM0_SC &= ~TPM_SC_CPWMS_MASK;

    TPM0_C4SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK | (TPM_CnSC_MSA_MASK)));
    TPM0_C4SC |= (TPM_CnSC_ELSB(1) | (TPM_CnSC_MSB(1)));
}

bool isMary = true;

void music_thread(void* argument) {
    for (;;) {
        if (isMary) {
            for (int i = 0; i < 27 && isMary; i++) {
                TPM0_MOD = mary[i];
                TPM0_C4V = mary[i] / 2;
                osDelay(300);
                TPM0_C4V = 0;
                osDelay(25);
            }
        } else {
            for (int i = 0; i < 25 && !isMary; i++) {
                TPM0_MOD = birthday[i];
                TPM0_C4V = birthday[i] / 2;
                osDelay(500);
                TPM0_C4V = 0;
                osDelay(50);
            }
        }
    }
}

void initMusicRTOS(void) {
    osThreadNew(music_thread, NULL, NULL);
}
