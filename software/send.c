void send(void)
{
  int i;
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  int temp;
//  char x;
  
  Flash_ptr = (char *) 0xF000;    // Initialize Flash pointer- data read
  
  FLL_CTL0 |= XCAP14PF;                     // Configure load caps
  P4SEL |= 0x03;                            // P4.0,1 = USART1 TXD/RXD
  ME2 |= UTXE1 + URXE1;                     // Enable USART1 TXD/RXD
  UCTL1 |= CHAR;                            // 8-bit character
  UTCTL1 |= SSEL1;                          // UCLK = SMCLK
  UBR01 = 0x69;                             // 1MHz 1200
  UBR11 = 0x03;                             // 1MHz 1200
  UMCTL1 = 0x03;                            // Modulation
  UCTL1 &= ~SWRST;                          // Initalize USART state machine
  IE2 |= URXIE1;                            // Enable USART1 RX interrupt
  for (i = 0; i < 0x7DE; i++)
  {
    temp = (0x0F & ((*(Flash_ptr)) >> 4));
    if (temp >= 0x0A)
      temp = temp + 55;
    else temp =  temp + 48;
    while (!(IFG2 & UTXIFG1));
    TXBUF1 = temp;

    temp = (0x0F & (*(Flash_ptr++)));
    if (temp >= 0x0A)
      temp = temp + 55;
    else temp =  temp + 48;
    while (!(IFG2 & UTXIFG1));
    TXBUF1 = temp;
    
  }
 }

