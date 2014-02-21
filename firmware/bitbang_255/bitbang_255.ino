/*------------------------------------------------------------------------
  Acrobotic - 01/10/2013
  Author: x1sc0 
  Platforms: Arduino Uno R3
  File: bitbang_255.ino

  Description: 
  This code sample accompanies the "How To Control 'Smart' RGB LEDs: 
  WS2811, WS2812, and WS2812B (Bitbanging Tutorial)" Instructable 
  (http://www.instructables.com/id/Bitbanging-step-by-step-Arduino-control-of-WS2811-/) 
  
  The code illustrates how to continuously send a single 255 value to the 
  WS2811 (driver)/WS2812/WS2812B RGB LEDs over a self-clocked 800KHz, NZR 
  signal.  The implementation was done using assembly so that the timing 
  of the signal was extremely accurate.

  Usage:
  Connect power (5V), ground (GND), and the Arduino Uno pin defined 
  by DIGITAL_PIN to the WS281X ports VCC, GND, and DIN ports, respectively.

  Upload the program to an Arduino Uno, and the Green LED of the first 
  WS281X will light up to full brightness. 

  ------------------------------------------------------------------------
  For a full-blown Arduino library, check out:
  https://github.com/acrobotic/Ai_Library_WS281X/
  ------------------------------------------------------------------------
  Please consider buying products from Acrobotic to help fund future
  Open-Source projects like this! We’ll always put our best effort in every
  project, and release all our design files and code for you to use. 
  http://acrobotic.com/
  ------------------------------------------------------------------------

  License:
  Beerware License; if you find the code useful, and we happen to cross 
  paths, you're encouraged to buy us a beer. The code is distributed hoping
  that you in fact find it useful, but  without warranty of any kind.
------------------------------------------------------------------------*/
#define DIGITAL_PIN       8       // Pin connected to the WS281X
#define PORT              PORTB   // Port
#define PORT_PIN          PORTB0

uint32_t t_f;

void setup() 
{
  // Setting DIGITAL_PIN to output, and initializing it to a LOW value
  pinMode(DIGITAL_PIN,OUTPUT);
  digitalWrite(DIGITAL_PIN,0);
}

void loop() 
{
  // Send the bits, and wait 10ms between transmissions.
  bitbang_255();
  delay(10); // This delay is aditional to the 50us needed by the WS281X
             // to latch the data to its output register.  The 50us delay
             // resides inside the function bitbang_255.
}


void bitbang_255(void) 
{
  while((micros() - t_f) < 50L);  // wait for 50us
  cli(); // Disable interrupts so that timing is as precise as possible

  asm volatile(
  // The volatile attribute is used to tell the compiler not to optimize 
  // this section.  We want every instruction to be left as is.
  //
  // Generating an 800KHz signal (1.25us period) implies that we have
  // exactly 20 instructions clocked at 16MHz (0.0625us duration) to 
  // generate either a 1 or a 0---we need to do it within a single 
  // period. 
  // 
  // By choosing 1 clock cycle as our time unit we can keep track of 
  // the signal's phase (T) after each instruction is executed.
  //
  // To generate a value of 1, we need to hold the signal HIGH (maximum)
  // for 0.8us, and then LOW (minimum) for 0.45us.  Since our timing has a
  // resolution of 0.0625us we can only approximate these values. Luckily, 
  // the WS281X chips were designed to accept a +/- 300ns variance in the 
  // duration of the signal.  Thus, if we hold the signal HIGH for 13 
  // cycles (0.8125us), and LOW for 7 cycles (0.4375us), then the variance 
  // is well within the tolerated range.
  //
  // For a full description of each assembly instruction consult the AVR
  // manual here: http://www.atmel.com/images/doc0856.pdf
    // Instruction        Clock   Description   Phase     Bit Transmitted
    "sbi  %0, %1\n\t"     // 2    PIN HIGH      (T =  2)  
    "rjmp .+0\n\t"        // 2    nop nop       (T =  4)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  6)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  8)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 10)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 12)
    "nop\n\t"             // 1    nop           (T = 13)
    "cbi   %0, %1\n\t"    // 2    PIN LOW       (T = 15)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 17)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 19)
    "nop\n\t"             // 1    nop           (T = 20)  1
    // Uncomment the code below in order to send 8 bits with a value of 1.
    // The WS281X will receive a value of 255, and set the Green LED to 
    // maximum brightness.

    "sbi  %0, %1\n\t"     // 2    PIN HIGH      (T =  2)  
    "rjmp .+0\n\t"        // 2    nop nop       (T =  4)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  6)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  8)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 10)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 12)
    "nop\n\t"             // 1    nop           (T = 13)
    "cbi   %0, %1\n\t"    // 2    PIN LOW       (T = 15)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 17)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 19)
    "nop\n\t"             // 1    nop           (T = 20)  1
    "sbi  %0, %1\n\t"     // 2    PIN HIGH      (T =  2)  
    "rjmp .+0\n\t"        // 2    nop nop       (T =  4)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  6)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  8)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 10)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 12)
    "nop\n\t"             // 1    nop           (T = 13)
    "cbi   %0, %1\n\t"    // 2    PIN LOW       (T = 15)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 17)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 19)
    "nop\n\t"             // 1    nop           (T = 20)  1
    "sbi  %0, %1\n\t"     // 2    PIN HIGH      (T =  2)  
    "rjmp .+0\n\t"        // 2    nop nop       (T =  4)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  6)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  8)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 10)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 12)
    "nop\n\t"             // 1    nop           (T = 13)
    "cbi   %0, %1\n\t"    // 2    PIN LOW       (T = 15)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 17)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 19)
    "nop\n\t"             // 1    nop           (T = 20)  1
    "sbi  %0, %1\n\t"     // 2    PIN HIGH      (T =  2)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  4)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  6)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  8)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 10)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 12)
    "nop\n\t"             // 1    nop           (T = 13)
    "cbi   %0, %1\n\t"    // 2    PIN LOW       (T = 15)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 17)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 19)
    "nop\n\t"             // 1    nop           (T = 20)  1
    "sbi  %0, %1\n\t"     // 2    PIN HIGH      (T =  2)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  4)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  6)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  8)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 10)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 12)
    "nop\n\t"             // 1    nop           (T = 13)
    "cbi   %0, %1\n\t"    // 2    PIN LOW       (T = 15)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 17)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 19)
    "nop\n\t"             // 1    nop           (T = 20)  1
    "sbi  %0, %1\n\t"     // 2    PIN HIGH      (T =  2)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  4)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  6)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  8)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 10)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 12)
    "nop\n\t"             // 1    nop           (T = 13)
    "cbi   %0, %1\n\t"    // 2    PIN LOW       (T = 15)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 17)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 19)
    "nop\n\t"             // 1    nop           (T = 20)  1
    "sbi  %0, %1\n\t"     // 2    PIN HIGH      (T =  2)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  4)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  6)
    "rjmp .+0\n\t"        // 2    nop nop       (T =  8)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 10)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 12)
    "nop\n\t"             // 1    nop           (T = 13)
    "cbi   %0, %1\n\t"    // 2    PIN LOW       (T = 15)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 17)
    "rjmp .+0\n\t"        // 2    nop nop       (T = 19)
    "nop\n\t"             // 1    nop           (T = 20)  1
    
    ::
    // Input operands
    "I" (_SFR_IO_ADDR(PORT)), //%0
    "I" (PORT_PIN)            //%1
  );
  sei();                          // Enable interrupts
  t_f = micros();                 // t_f will be used to measure the 50us 
                                  // latching period in the next call of the 
                                  // function.
}
