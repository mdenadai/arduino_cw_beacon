#include <arduino.h>

#include "util.h"
#include "config.h"
#include "cwencoder.h"

#ifdef DEBUG
 //#define CW_VERBOSE
#endif

#define CW_BITMAP_SIZE 3

typedef struct {
   char caracter;
   unsigned char length;
   unsigned char bitmap[CW_BITMAP_SIZE];
} TMorseCode;

static const TMorseCode tab[] =
{
{'A', 5,{0xB8,0x00,0x00}},  // 101110000000000000000000 .-
{'B', 9,{0xEA,0x80,0x00}},  // 111010101000000000000000 -...
{'C',11,{0xEB,0xA0,0x00}},  // 111010111010000000000000 -.-.
{'D', 7,{0xEA,0x00,0x00}},  // 111010100000000000000000 -..
{'E', 1,{0x80,0x00,0x00}},  // 100000000000000000000000 .
{'F', 9,{0xAE,0x80,0x00}},  // 101011101000000000000000 ..-.
{'G', 9,{0xEE,0x80,0x00}},  // 111011101000000000000000 --.
{'H', 7,{0xAA,0x00,0x00}},  // 101010100000000000000000 ....
{'I', 3,{0xA0,0x00,0x00}},  // 101000000000000000000000 ..
{'J',13,{0xBB,0xB8,0x00}},  // 101110111011100000000000 .---
{'K', 9,{0xEB,0x80,0x00}},  // 111010111000000000000000 -.-
{'L', 9,{0xBA,0x80,0x00}},  // 101110101000000000000000 .-..
{'M', 7,{0xEE,0x00,0x00}},  // 111011100000000000000000 --
{'N', 5,{0xE8,0x00,0x00}},  // 111010000000000000000000 -.
{'O',11,{0xEE,0xE0,0x00}},  // 111011101110000000000000 ---
{'P',11,{0xBB,0xA0,0x00}},  // 101110111010000000000000 .--.
{'Q',13,{0xEE,0xB8,0x00}},  // 111011101011100000000000 --.-
{'R', 7,{0xBA,0x00,0x00}},  // 101110100000000000000000 .-.
{'S', 5,{0xA8,0x00,0x00}},  // 101010000000000000000000 ...
{'T', 3,{0xE0,0x00,0x00}},  // 111000000000000000000000 -
{'U', 7,{0xAE,0x00,0x00}},  // 101011100000000000000000 ..-
{'V', 9,{0xAB,0x80,0x00}},  // 101010111000000000000000 ...-
{'W', 9,{0xBB,0x80,0x00}},  // 101110111000000000000000 .--
{'X',11,{0xEA,0xE0,0x00}},  // 111010101110000000000000 -..-
{'Y',13,{0xEB,0xB8,0x00}},  // 111010111011100000000000 -.--
{'Z',11,{0xEE,0xA0,0x00}},  // 111011101010000000000000 --..
{'1',17,{0xBB,0xBB,0x80}},  // 101110111011101110000000 .----
{'2',15,{0xAE,0xEE,0x00}},  // 101011101110111000000000 ..---
{'3',13,{0xAB,0xB8,0x00}},  // 101010111011100000000000 ...--
{'4',11,{0xAA,0xE0,0x00}},  // 101010101110000000000000 ....-
{'5', 9,{0xAA,0x80,0x00}},  // 101010101000000000000000 .....
{'6',11,{0xEA,0xA0,0x00}},  // 111010101010000000000000 -....
{'7',13,{0xEE,0xA8,0x00}},  // 111011101010100000000000 --...
{'8',15,{0xEE,0xEA,0x00}},  // 111011101110101000000000 ---..
{'9',17,{0xEE,0xEE,0x80}},  // 111011101110111010000000 ----.
{'0',19,{0xEE,0xEE,0xE0}},  // 111011101110111011100000 -----
{'.',17,{0xBA,0xEB,0x80}},  // 101110101110101110000000 .-.-.-
{',',19,{0xEE,0xAE,0xE0}},  // 111011101010111011100000 --..--
{';',17,{0xEB,0xAE,0x80}},  // 111010111010111010000000 -.-.-.
{'?',15,{0xAE,0xEA,0x00}},  // 101011101110101000000000 ..--..
{'%',13,{0xEA,0xE8,0x00}},  // 111010101110100000000000 -..-.
{'\xC7',13,{0xEB,0xA8,0x00}},  // 111010111010100000000000 -.-..     C Cidilia
{'\xC9',11,{0xAE,0xA0,0x00}},  // 101011101010000000000000 ..-..     E acentuado(EH)
{'(',19,{0xEB,0xBA,0xE0}},  // 111010111011101011100000 -.--.-
{')',19,{0xEB,0xBA,0xE0}},  // 111010111011101011100000 -.--.-
{'@',17,{0xBB,0xAE,0x80}},  // 101110111010111010000000 .--.-.
{'/',13,{0xEA,0xE8,0x00}},  // 111010101110100000000000 -..-.
{' ', 1,{0x00,0x00,0x00}}   // espaco tem o tamanho de 7 pontos, porem como o final de cada caracter tem 3 pontos, fica 1 (3 do ultimo caracter 1 do ' ' e 3 do final do ' ')
};


#define HZ_MIN (unsigned short)(F_CPU/(2L*256L*254L))
#define HZ_MAX (unsigned short)(F_CPU/(2L*256L))

static volatile unsigned char cw_ativo;

//------------------------------------------------------------------------------------------------------------------
static void tone_on(void)
{
 #ifdef CW_SIDETONE_PIN
  tone(CW_SIDETONE_PIN,CW_SIDETONE);
 #endif 
/*  
  if ( Hz < HZ_MIN ) Hz=HZ_MIN;
  if ( Hz > HZ_MAX ) Hz=HZ_MAX;
#if defined(__AVR_ATmega164P__) || defined(__AVR_ATmega644P__)
  TCCR2B=(0<<FOC2B)|(0<<FOC2B)|(0<<WGM22)|(1<<CS22)|(1<<CS21)|(0<<CS20);
//  TCCR2A=(0<<COM2A1)|(0<<COM2A0)|(0<<COM2B1)|(1<<COM2B0)|(1<<WGM21)|(0<<WGM20);
  TCCR2A=(0<<COM2A1)|(1<<COM2A0)|(0<<COM2B1)|(0<<COM2B0)|(1<<WGM21)|(0<<WGM20);
  OCR2B=0; // nao faz diferenca
  OCR2A=(unsigned char)(F_CPU/(2L*256L*(long)Hz)-1L);
//  DDRD|=0x08;
  DDRD|=0x80;
#else
  TCCR2B=(0<<FOC2B)|(0<<FOC2B)|(0<<WGM22)|(1<<CS22)|(1<<CS21)|(0<<CS20);
//  TCCR2A=(0<<COM2A1)|(0<<COM2A0)|(0<<COM2B1)|(1<<COM2B0)|(1<<WGM21)|(0<<WGM20);
  TCCR2A=(0<<COM2A1)|(1<<COM2A0)|(0<<COM2B1)|(0<<COM2B0)|(1<<WGM21)|(0<<WGM20);
  OCR2B=0; // nao faz diferenca
  OCR2A=(unsigned char)(F_CPU/(2L*256L*(long)Hz)-1L);
//  DDRD|=0x08;
//  DDRD|=0x80;
  DDRB|=0x08;
#endif
*/
}
//------------------------------------------------------------------------------------------------------------------
static void tone_off(void)
{
 #ifdef CW_SIDETONE_PIN
  noTone(CW_SIDETONE_PIN);
 #endif 
/*
#if defined(__AVR_ATmega164P__) || defined(__AVR_ATmega644P__)
  TCCR2A=(0<<COM2A1)|(0<<COM2A0)|(0<<COM2B1)|(0<<COM2B0)|(1<<WGM21)|(0<<WGM20);
//  PORTD&=~0x08;
  PORTD&=~0x80;
#else
  TCCR2A=(0<<COM2A1)|(0<<COM2A0)|(0<<COM2B1)|(0<<COM2B0)|(1<<WGM21)|(0<<WGM20);
//  PORTD&=~0x08;
//  PORTD&=~0x80;
  PORTB&=~0x08;
#endif
*/
}
//------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------
static void key_on(void)
{
#ifdef CW_KEY_PIN
  digitalWrite(CW_KEY_PIN, CW_KEY_PIN_ON);
#endif
#ifdef STATUS_LED_PIN
  digitalWrite(STATUS_LED_PIN, ON);
#endif      
}
//------------------------------------------------------------------------------------------------------------------
static void key_off(void)
{
#ifdef CW_KEY_PIN
  digitalWrite(CW_KEY_PIN, !CW_KEY_PIN_ON);
#endif  
#ifdef STATUS_LED_PIN
  digitalWrite(STATUS_LED_PIN, OFF);
#endif      
}
//------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------
static int encode(char c, TMorseCode* code)
{
   unsigned i;
   c=toupper(c);
   for ( i=0 ; i < sizeof(tab) / sizeof(TMorseCode) ; i++ ) {
      if ( tab[i].caracter == c ) {
         memcpy(code,&tab[i],sizeof(TMorseCode));
         return TRUE;
      }
   }
   memset(code,0,sizeof(TMorseCode));
   return FALSE;
}
//------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------
static void play(TMorseCode* code, int tempo)
{
   unsigned char i;
   unsigned char id;
   unsigned char bit;
   unsigned char atual;
   if ( code->length < 1 ) {
      return;
   }
   atual=OFF;
#ifdef CW_VERBOSE
   DBG("%c[", code->caracter);
#endif
   for ( i=0 ; i < code->length ; i++ ) {
      id=i/8;
      bit=i-8*id;
      if ( code->bitmap[id] & (0x80 >> bit) ) {
         if ( atual == OFF ) {
            atual=ON;
            key_on();
            tone_on();
         }
#ifdef CW_VERBOSE
         DBG("_");
#endif
      }
      else {
         if ( atual == ON ) {
            atual=OFF;
            key_off();
            tone_off();
         }
#ifdef CW_VERBOSE
         DBG(" ");
#endif
      }
      aguarda(tempo);
   }
#ifdef CW_VERBOSE
   DBG("]\n");
#endif
   key_off();
   tone_off();
   aguarda(tempo*3);
}
//------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------
// calculo do ppm
//    base palavra PARIS que tem 50 unidade de tempo
//   formula= 60000/(50*ppm)
void CWEncoder(const char* txt, int ppm)
{
   int len;
   int i;
   int tempo;
   TMorseCode code[1];
   len=strlen(txt);
   tempo=(int)(60000L/(50L*(long)ppm));
   for ( i=0; i < len ; i++ ) {
      encode(txt[i],code);
      play(code,tempo);
   }
}
//------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------
void CWTone(unsigned short ms)
{
   key_on();
   tone_on();
   aguarda(ms);
   key_off();
   tone_off();
}
//------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------
void CWTxPowerON(void)
{
#ifdef CW_TXPWR_PIN
  digitalWrite(CW_TXPWR_PIN, CW_TXPWR_PIN_ON);
#endif
  beacon_ativo(TRUE);
  cw_ativo=TRUE;
}
//------------------------------------------------------------------------------------------------------------------
void CWTxPowerOFF(void)
{
#ifdef CW_TXPWR_PIN
  digitalWrite(CW_TXPWR_PIN, !CW_TXPWR_PIN_ON);
#endif
  beacon_ativo(FALSE);
  cw_ativo=FALSE;
}
//------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------------
void CWEncoderInit(void)
{
#ifdef CW_TXPWR_PIN
   digitalWrite(CW_TXPWR_PIN, !CW_TXPWR_PIN_ON);
   pinMode(CW_TXPWR_PIN, OUTPUT);
#endif  
#ifdef CW_KEY_PIN
   digitalWrite(CW_KEY_PIN, !CW_KEY_PIN_ON);
   pinMode(CW_KEY_PIN, OUTPUT);
#endif  
   CWTxPowerOFF();
}
//------------------------------------------------------------------------------------------------------------------


