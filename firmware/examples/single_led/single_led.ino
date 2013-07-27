/*------------------------------------------------------------------------
  WS2811 demo
------------------------------------------------------------------------*/
#include "Ai_WS2811.h"

Ai_WS2811 ws2811;

/*------------------------------------------------------------------------
  LED object
------------------------------------------------------------------------*/
class LED {
  byte r_,g_,b_;
public:
  LED& setColor(byte r, byte g, byte b) {
    r_ = r;
    g_ = g;
    b_ = b;
  }
  // nb. Interrupts must be disabled when you call this (use "cli()")
  void send() const {
    ws2811.sendByte(g_);
    ws2811.sendByte(b_);
    ws2811.sendByte(r_);
  }
};
LED led1,led2,led3;

void sendLEDs()
{
  // Interrupts off
  cli();
  // Send 60 LEDs
  for (byte i=0; i<1; ++i) {
    led1.send();
    led2.send();
    led3.send();
  }
  // Interrupts on again
  sei();
}

void setup()
{
  // Initialize ws2811 object
  ws2811.init();

  // Set LED colors
  byte a = 0x00;
  byte b = 0xff;
  led1.setColor(b,a,a);
  led2.setColor(a,b,a);
  led3.setColor(a,a,b);
}

void loop()
{
  // Update LEDs
  sendLEDs();

  // Wait one second
  _delay_ms(1000);

  // Swap the LEDs around
  LED t = led1;
  led1 = led2;
  led2 = led3;
  led3 = t;
}

