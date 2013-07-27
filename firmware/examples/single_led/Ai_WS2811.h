#include <Arduino.h>
#include <util/delay.h>

/*------------------------------------------------------------------------
  Pin mappings for different chips
------------------------------------------------------------------------*/

#if defined (__AVR_ATtiny85__)
#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED_PIN PINB
#define LED_BIT _BV(3)

#elif defined (__AVR_ATtiny84__)
#define LED_DDR DDRA
#define LED_PORT PORTA
#define LED_PIN PINA
#define LED_BIT _BV(0)

#else
// Assume Arduino Uno, digital pin 8 (ie. port B, pin 0)
// See http://arduino.cc/en/Hacking/PinMapping168 for pin mappings

#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED_PIN PINB
#define LED_BIT _BV(0)

#endif

#define NOP __asm__("nop\n\t")

/*------------------------------------------------------------------------
  Set the LED pin as output on startup
------------------------------------------------------------------------*/
class Ai_WS2811 {
  public:
    void init();
    void sendByte(byte b);
};
