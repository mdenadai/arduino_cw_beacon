#include <arduino.h>
#include <stdarg.h>
#include "util.h"
#include "config.h"


//--------------------------------------------------------------------------------------------------------------------
void inicializa(void)
{
#ifdef DEBUG  
  Serial.begin(9600);  while (!Serial);
#endif
#ifdef STATUS_LED_PIN
  pinMode(STATUS_LED_PIN, OUTPUT);
#endif
}


static volatile unsigned char ativo=0; 

//--------------------------------------------------------------------------------------------------------------------
void beacon_ativo(unsigned char on_off)
{
  ativo=on_off;  
#ifdef STATUS_LED_PIN
  digitalWrite(STATUS_LED_PIN, 0);
#endif    
}

//--------------------------------------------------------------------------------------------------------------------
void aguarda(unsigned long ms)
{
  unsigned long tempo;
#ifdef STATUS_LED_PIN
  static unsigned char ledState;
  static unsigned long tempo_led;
  static unsigned led_aguarda=0;
#endif
  tempo = millis();
  while ( (millis() - tempo) < ms ) {
#ifdef STATUS_LED_PIN
    if ( ativo ) {
        tempo_led = millis();
        ledState=0;
        led_aguarda=1500;
    }
    if ( (millis() - tempo_led) > led_aguarda ) {
      tempo_led=millis();
      if (ledState) {
        ledState=0;
        led_aguarda=1500;
      }
      else {
        ledState=1;
        led_aguarda=50;
      }
      digitalWrite(STATUS_LED_PIN, ledState);
    }
#endif //STATUS_LED_PIN
  }
}


#ifdef DEBUG
//--------------------------------------------------------------------------------------------------------------------
void DBG(const char *fmt, ...)
{
  char buffer[32];
  va_list arg;
  if( fmt!=NULL ) {
    va_start(arg, fmt);
    vsprintf(buffer, fmt, arg);
    va_end(arg);
    Serial.print(buffer);
  }
}
#endif
