#ifndef _CW_ENCODER_H
#define _CW_ENCODER_H

void CWEncoderInit(void);
void CWEncoder(const char* txt, int ppm);
void CWTone(unsigned short ms);   //gerar um tom continuo - memso que segurar manipulador apertado

void CWTxPowerON(void);    //muda o led de status de idle para transmitindo (piscando CW codificado) - tambem pode acionar uma porta digitar
void CWTxPowerOFF(void);   //iden so que desliga 

extern volatile unsigned char cw_use_led;


#endif
