int led_on(void) 
{
  P1SEL = 0x00;
  P1DIR  = 0xFF;    
  P1OUT  = ~(0x08);
  return 0;
}

int led_off(void)
{
  P1SEL = 0x00;
  P1DIR  = 0xFF;    
  P1OUT  = ~(0x00);
  return 0;
}

