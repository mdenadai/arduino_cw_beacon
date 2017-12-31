#ifndef __Config_H
#define __Config_H

// pino digital usado para ligar o transmissor - comentar para desabilitar
#define CW_TXPWR_PIN       2
#define CW_TXPWR_PIN_ON    HIGH   //txpwr ligado em HIGH ou LOW

// pino digital usado para o manipularo - comentar para desabilitar
#define CW_KEY_PIN         3     
#define CW_KEY_PIN_ON      HIGH     //key ligado em HIGH ou LOW

// pino digital usado para gerar audio do cw - comentar para desabilitar
#define CW_SIDETONE_PIN    4
#define CW_SIDETONE  980          // frequencia do ton em Hz  OBS: 980 RESULTA EM 1000 Hz *verificar porque ????


// pino digitak usado para lede de status - comentar para desabilitar
#define STATUS_LED_PIN     LED_BUILTIN


#endif
