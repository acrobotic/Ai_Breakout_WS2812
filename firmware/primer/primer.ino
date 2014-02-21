/*---------------------------------------------------------------------
  Acrobotic - 01/10/2013
  Author: x1sc0 
  Platforms: Arduino Uno R3
  File: primer.ino
  Description: 
  This code sample is a self-contained demo (no need to install 
  additional libraries) for controlling WorldSemi's WS2811 LED driver IC.  
  Note that this driver IC is integrated into the WS2812, and WS2812B RGB 
  LED modules, thus it can be used to control these as well.
  
  The code was written to showcase the bare minimum needed to control the 
  LED driver IC, hoping to illustrate how the AVR assembly instructions are
  used to produce highly precise timing for a digital signal.
  
  Usage:
  Once the code is uploaded, and some WS281X are connected to the pin
  specified by DATA_PIN, we should observe the LEDs light up simultaneously,
  iterating through the colors Red, Green, and Blue with a 1s hold time.

  For more interesting lighting effects please check out the effects.ino file
  contained in the firmware directory within the respository 
  https://github.com/acrobotic/Ai_Breakout_WS2812/

  For a full-blown Arduino library, check out:
  https://github.com/acrobotic/Ai_Library_WS28XX/
  
  License:
  Beerware License; if you find the libraries useful, and we happen to cross 
  paths, you're encouraged to buy us a beer. The code is distributed hoping
  that you in fact find it useful, but  without warranty of any kind.
------------------------------------------------------------------------*/

#define NUM_CHIPS         64
#define NUM_LEDS          NUM_CHIPS*3
#define DATA_PIN          8
#define PORT              PORTB
#define PINMASK           _BV(0)
#define CYCLE_DELAY       1000 

uint8_t* rgb_arr = NULL;
uint32_t endTime;

void setup() 
{
  pinMode(DATA_PIN,OUTPUT);
  digitalWrite(DATA_PIN,0);
  if((rgb_arr = (uint8_t *)malloc(NUM_LEDS)))             
  {                 
    memset(rgb_arr, 0, NUM_LEDS);                         
  }        
  render();
}

void loop() 
{
  int i;
  for(i=0;i<1;i++)
    setPixelColor(i,10,0,0);
  render();
  delay(1000);
  for(i=0;i<1;i++)
    setPixelColor(i,100,0,0);
  render();
  delay(1000);
  for(i=0;i<1;i++)
    setPixelColor(i,255,0,0);
  render();
  delay(1000);
  for(i=0;i<1;i++)
    setPixelColor(i,0,10,0);
  render();
  delay(1000);
  for(i=0;i<1;i++)
    setPixelColor(i,0,100,0);
  render();
  delay(1000);
  for(i=0;i<1;i++)
    setPixelColor(i,0,255,0);
  render();
  delay(1000);
  for(i=0;i<1;i++)
    setPixelColor(i,0,0,10);
  render();
  delay(1000);
  for(i=0;i<1;i++)
    setPixelColor(i,0,0,100);
  render();
  delay(1000);
  for(i=0;i<1;i++)
    setPixelColor(i,0,0,255);
  render();
  delay(1000);
  for(i=0;i<1;i++)
    setPixelColor(i,10,10,10);
  render();
  delay(1000);
  for(i=0;i<1;i++)
    setPixelColor(i,100,100,100);
  render();
  delay(1000);
  for(i=0;i<1;i++)
    setPixelColor(i,255,255,255);
  render();
  delay(1000);
}

void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) 
{
  if(n < NUM_CHIPS) 
  {
    uint8_t *p = &rgb_arr[n * 3]; 
    *p++ = g;  
    *p++ = r;
    *p = b;
  }
}

void render(void) 
{
  if(!rgb_arr) return;

  while((micros() - endTime) < 50L);                     

  noInterrupts(); // Need 100% focus on instruction timing

  volatile uint16_t 
    i   = NUM_LEDS; // Loop counter                      
  volatile uint8_t  
   *ptr = rgb_arr,   // Pointer to next byte
    b   = *ptr++,   // Current byte value                
    hi,             // PORT w/output bit set high        
    lo;             // PORT w/output bit set low         

  // 20 inst. clocks per bit: HHHHHxxxxxxxxLLLLLLL       
  // ST instructions:         ^   ^        ^       (T=0,5,13)

  volatile uint8_t next, bit;

  hi   = PORT |  PINMASK;                               
  lo   = PORT & ~PINMASK;
  next = lo;
  bit  = 8;

  asm volatile(
   "head20:\n\t"          // Clk  Pseudocode    (T =  0) 
    "st   %a0, %1\n\t"    // 2    PORT = hi     (T =  2) 
    "sbrc %2, 7\n\t"      // 1-2  if(b & 128)            
     "mov  %4, %1\n\t"    // 0-1   next = hi    (T =  4) 
    "dec  %3\n\t"         // 1    bit--         (T =  5) 
    "st   %a0, %4\n\t"    // 2    PORT = next   (T =  7) ST and MOV don't                         
    "mov  %4, %5\n\t"     // 1    next = lo     (T =  8) change Z flag,                           
    "breq nextbyte20\n\t" // 1-2  if(bit == 0)       <-- so this is OK.
    "rol  %2\n\t"         // 1    b <<= 1       (T = 10)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 12)
    "nop\n\t"             // 1    nop           (T = 13)
    "st   %a0, %5\n\t"    // 2    PORT = lo     (T = 15)
    "nop\n\t"             // 1    nop           (T = 16)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 18)
    "rjmp head20\n\t"     // 2    -> head20 (next bit out)
   "nextbyte20:\n\t"      //                    (T = 10)
    "ldi  %3, 8\n\t"      // 1    bit = 8       (T = 11)
    "ld   %2, %a6+\n\t"   // 2    b = *ptr++    (T = 13)
    "st   %a0, %5\n\t"    // 2    PORT = lo     (T = 15)
    "nop\n\t"             // 1    nop           (T = 16)
    "sbiw %7, 1\n\t"      // 2    i--           (T = 18)
    "brne head20\n\t"     // 2    if(i != 0) -> head20 (next byte)
    ::
    "e" (&PORT),          // %a0
    "r" (hi),             // %1
    "r" (b),              // %2
    "r" (bit),            // %3
    "r" (next),           // %4
    "r" (lo),             // %5
    "e" (ptr),            // %a6
    "w" (i)               // %7
  ); // end asm
  interrupts();
  endTime = micros(); // Save EOD time for latch on next call
}
