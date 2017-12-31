#ifndef __UTIL_AVR_H
#define __UTIL_AVR_H

//#define DEBUG

#define TRUE  1
#define FALSE 0
#define ON    1
#define OFF   0

void inicializa(void);
void aguarda(unsigned long ms);
void beacon_ativo(unsigned char on_off);

#ifdef DEBUG
 void DBG(const char *fmt, ...);
#else
 #define DBG(...) 
#endif



#endif