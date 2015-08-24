/*********************Uses Basic Timer*********************************/


#include <msp430x44x.h>
//#include "led.c"
//int ticks;
int d;
// This will drift and will require calibration – ok for testing though.
//#define TICKS_PER_SECOND 2500
void wait(int delay)
{
  WDTCTL = WDTPW + WDTHOLD; // Stop WDT
  IE2 |= BTIE; // Enable BT interrupt
  BTCTL = 0x00+BTDIV+BTIP2+BTIP1+BTIP0; // Divide System clock by 256//removing BTSSEL from above to select ACLK
  _EINT(); // Enable interrupts
  for (d = delay; d > 0;)
  {
    IE2 |= BTIE; // Enable BT interrupt
    _BIS_SR(CPUOFF+ GIE);	
    __no_operation(); // Required only for C-spy
  }
}


// Basic Timer interrupt service routine
#pragma vector=BASICTIMER_VECTOR
__interrupt void basic_timer(void)
{
//  ticks++;
  d--;
//  if (ticks % 2 == 0)
//    led_on();
//  else led_off();
  if (d <= 0)
  {
    _DINT();
  }
  
  _BIC_SR_IRQ(CPUOFF);
}
