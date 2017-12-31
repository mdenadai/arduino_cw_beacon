#include <Arduino.h>
#include "util.h"
#include "cwencoder.h"

//// ------


/************************************************************
 *  config.h => configuracoes das portas
 *      CW_TXPWR_PIN (2)
 *         esse pino pode ser usado para controlar alimentacao do radio
 *      CW_KEY_PIN (3)
 *         pino para acionar o manipulador
 *      CW_SIDETONE_PIN (4)
 *         saida de audio
 *      
 *  Funcoes:
 *  
 *     WEncoder(const char* txt, int ppm) 
 *        codifica CW 
 *            txt: texto entre aspas em MAIUSCULO
 *            ppp: velocidade palavras por minuto
 *            
 *     CWTone(unsigned short ms)
 *         ativa o pindo do manipulador CW_KEY_PIN (3)
 *            ms: tempo em milisegundos
 *            
 *     aguarda(unsigned long ms)
 *         ms: tempo em milisegundos
 *         
 *     CWTxPowerON()/CWTxPowerOFF()
 *         liga/desliga o pino CW_TXPWR_PIN (2)
 *         
 *     
 *     
 * 
 * 
 ***********************************************************/



//--------------------------------------------------------------------------------------------------------------------
/***************************
 * Alteracoes nessa funcao
 * 
 ***************************/
void loop(void) {
  
  aguarda(5000);

  CWTxPowerON();
  
  //traço longo de 3s
  CWTone(3000);
  
  //silencio 1s 
  aguarda(1000);
   
  CWEncoder("PY2MEB/B   PY2MEB/B   GG68HC   GG68HC   PSE QSL VIA QRZ", 12);
    
  CWTxPowerOFF();
  
}



//--------------------------------------------------------------------------------------------------------------------
void setup(void) {
  inicializa();
  CWEncoderInit();
}

