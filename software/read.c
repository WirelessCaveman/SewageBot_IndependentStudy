//****************************************************************************
//	Reads the ADC ports and Writes to Flash
//	Shailesh Kadamaje
//	07/25/2007
//******************************************************************************
char  value0;
char  value1;                      // 8-bit value to write to segment A
char *Flash_ptr;                // Flash pointer
long int i;
unsigned int k;
int dark_count=0;


// Function prototypes
void  write_Seg (void);
void read(void);

int main_read(long int flag)		//if flag = 0 reading data, if flag = N reading frequency, if flag > 1500 complete darkness detect
{

  long int x;
  WDTCTL = WDTPW + WDTHOLD;       // Stop watchdog timer
  FCTL2 = FWKEY + FSSEL0 + FN0;   // MCLK/2 for Flash Timing Generator
//  flag = 262143;	//got this in 93 sec
  k = flag;
  
	if (flag == 0)
	{
  		ADC12CTL0 = ADC12ON | SHT0_9 | REF2_5V | REFON | MSC;   // ADC on, int. ref. on (2,5 V),
                                                     // single channel single conversion
  		ADC12CTL1 = ADC12SSEL0 | CSTARTADD_0 | CONSEQ0 | SHP | ADC12DIV4;// ACLK / 4 = 8 KHz
	}
	else
	{
	        ADC12CTL0 = ADC12ON | SHT0_0 | REF2_5V | REFON | MSC;   // ADC on, int. ref. on (2,5 V),
                                                     // single channel single conversion
  		ADC12CTL1 = ADC12SSEL3 | CSTARTADD_0 | CONSEQ0 | SHP;// ADC clock = SMCLK
              	FLL_CTL1 |= SELM_XT2;				//xt2 source for mclk
                FLL_CTL1 &= ~XT2OFF;			//switch xt2 on
//		FLL_CTL1 &= ~SMCLKOFF;			//switch SMCLK on
//		FLL_CTL1 |= SELS;			//select XT2 clock as SMCLK source
  	}	
	
  
  
  ADC12MCTL6 = SREF_1 | INCH_6 & ~EOS;                  // int. ref., channel 7
  if (flag == 0)
  {
  ADC12MCTL7 = SREF_1 | INCH_7 & ~EOS;
  }
//  P6SEL |= 0x03;                                              // P6.1 and P6.0 ADC12 function
  P5DIR |= 0x02;                                             // P5.1 output
    
  FCTL1 = FWKEY + ERASE;          // Set Erase bit
  FCTL3 = FWKEY;                  // Clear Lock bit
  if (flag == 0)
  {
	  Flash_ptr = (char *) 0xF000;
	  x = 0x3EF;
  }
  else
  {
	  Flash_ptr = (char *) 0x1000;    // Initialize Flash pointer- frequency detect
	  x = flag;
  }

  *Flash_ptr = 0;                 // Dummy write to erase Flash segment
  ADC12IE = 0x01;                                             // Enable interrupt
  
  if (flag < 1500)
  {
    for (i = 0; i < x;)                              
    {
      ADC12CTL0 |= ENC;                                   // Conversion enabled
      ADC12CTL0 |= ADC12SC;                 // Sampling open
      _BIS_SR(CPUOFF + GIE);              // LPM0, ADC12_ISR will force exit
      
      //    __no_operation();                         // SET BREAKPOINT HERE
    }
  }
  else
  {
    while (dark_count < x)                              
    {
      ADC12CTL0 |= ENC;                                   // Conversion enabled
      ADC12CTL0 |= ADC12SC;                 // Sampling open
      _BIS_SR(CPUOFF + GIE);              // LPM0, ADC12_ISR will force exit
      
      //    __no_operation();                         // SET BREAKPOINT HERE
    }
  }
  
  
  ADC12IE = 0x00;
    FCTL1 = FWKEY;                  // Clear WRT bit
    FCTL3 = FWKEY + LOCK;           // Reset LOCK bit

  
  ADC12IE = 0x00;                                             // Disable interrupt 
  
  return 0;
}

#pragma vector=ADC_VECTOR
__interrupt void ADC12_ISR (void)
{
    if (ADC12MEM0 < 0x7FF) 
    P5OUT &= ~0x02;                               // Clear P5.1 LED off
    else
    P5OUT |= 0x02;                                 // Set P5.1 LED on
    value0 = ADC12MEM6 & 0x0FFF; 
    if (k == 0)			//reading UV inside reactor
    {
    value1 = ADC12MEM7 & 0x0FFF;
    }
    __no_operation();
    if (k < 1500)		//not darkness mode
    {
    	write_Seg();
    }
    else			//complete darkness mode
    {
      if (value0 == 0xFF)
	dark_count++;
      else dark_count = 0;
    }
	
    _BIC_SR_IRQ(CPUOFF);                 // Clear CPUOFF bit from 0(SR)
}

void write_Seg(void)
{
 
  FCTL1 = FWKEY + WRT;            // Set WRT bit for write operation
  
  *(Flash_ptr++) = value0;         // Write value to flash
//    led_on();
//    led_off();
    if (k == 0)
    {
    *(Flash_ptr++) = value1;
    }
    i++;
}

void read(void)
{
  int i;
  Flash_ptr = (char *) 0x1080;    // Initialize Flash pointer  
  for (i = 0; i < 10; i++)
  {
    value0 = *(Flash_ptr++);
    if (k == 0)
    {
    value1 = *(Flash_ptr++);
    }
  }
}

//int * read1(void)
void read1(void)
{
  int i;
  Flash_ptr = (char *) 0x1080;    // Initialize Flash pointer  
  for (i = 0; i < 10; i++)
  {
    value0 = *(Flash_ptr++); 
    if (k == 0)
    {
    value1 = *(Flash_ptr++);
    }
  }
}

