
#include <stdio.h>
#include <math.h>
#include <msp430x44x.h>
#include "read.c"
#include "send.c"
#include "led.c"
#include "wait.c"

#define FLOATING	float
#define SAMPLE	unsigned char
#define PI (3.142)

#define SAMPLING_RATE	3846.0	//3.846 KHz
#define TARGET_FREQUENCY_A	833.0	//833 Hz
#define TARGET_FREQUENCY_B	416.0	//416 Hz
#define N (128)	//Block size

FLOATING coeff;
FLOATING Q1;
FLOATING Q2;
FLOATING sine;
FLOATING cosine;
FLOATING magnitudeSquared=0;

SAMPLE testData[N];

/* Call this routine before every "block" (size=N) of samples. */
void ResetGoertzel(void)
{
  Q2 = 0;
  Q1 = 0;
}

/* Call this once, to precompute the constants. */
void InitGoertzel(FLOATING freq)
{
  int	k;
  FLOATING	floatN;
  FLOATING	omega;
  
  floatN = (FLOATING) N;
  k = (int) (0.5 + ((floatN * freq) / SAMPLING_RATE));
  omega = (2.0 * PI * k) / floatN;
  sine = sin(omega);
  cosine = cos(omega);
  coeff = 2.0 * cosine;
  
  
  ResetGoertzel();
}

/* Call this routine for every sample. */
void ProcessSample(SAMPLE sample)
{
  FLOATING Q0;
  Q0 = coeff * Q1 - Q2 + (FLOATING) sample;
  Q2 = Q1;
  Q1 = Q0;
}


FLOATING GetMagnitudeSquared(void)
{
  FLOATING result;
  
  result = Q1 * Q1 + Q2 * Q2 - Q1 * Q2 * coeff;
  return result;
}

// Demo 1 
void DetectFreq(FLOATING freq)
{
  int	index;
  float x;
  
//  FLOATING	magnitudeSquared;
  //  FLOATING	magnitude;
  //  FLOATING	real;
  //  FLOATING	imag;
  
  //  printf("For test frequency %f:\n", frequency);
  //Generate(freq);
  InitGoertzel(freq);
  main_read(N); //keep this
  //    main_read();  //remove 
  
  /*Process the samples */
  Flash_ptr = (char *) 0x1000;    // Initialize Flash pointer- frequency detect
  for (index = 0; index < N; index++)
  {
    //    x = (float)((*(Flash_ptr++))*100/255);
    x = (*(Flash_ptr++));
    x = x * 100/255;
    ProcessSample(x);
    x++;
  } 
  // Do the "optimized Goertzel" processing
  magnitudeSquared = GetMagnitudeSquared();
//  magnitude = sqrt(magnitudeSquared);
  //  index = magnitude;
  __no_operation();                         // SET BREAKPOINT HERE
  
  ResetGoertzel();
}



int main(void)
{
  while (1)
  {
    while (1)
    {
//      wait(3);
      DetectFreq(TARGET_FREQUENCY_B);
      if (magnitudeSquared > 500000) send();
//      wait(3);
      DetectFreq(TARGET_FREQUENCY_A);
      if (magnitudeSquared > 500000) break;
    }
    
    led_on();
    
    main_read(0x2FFF);		//wait for darkness
    
    //  wait(3000);
    wait(3);
    
    main_read(0);
    
    led_off();
    
    magnitudeSquared = 0;
    
    while (magnitudeSquared < 500000)
    {
      wait(1);
      DetectFreq(TARGET_FREQUENCY_B);
    }
    
    send();
    
    __no_operation();
    
    return 0;
  }
}


