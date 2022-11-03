#include <util/delay.h>
#include "Adc.h"
#include "Uart.h"
#include <stdlib.h>

// Measure Temperature using NTC Thermistor:
 

 
 
// 1- Temperature Measurement
 
float voltageDividerR1 = 10000;
float BValue = 3470;
float R1 = 10000;
float T1 = 298.15;
float R2 ;
float T2 ;
 
float a ;
float b ;
float c ;
float d ;
float e = 2.718281828 ;
 
float tempSampleRead = 0;
bool  measuredTemp_bool = false;

int main(void) {
  
 //Defining local variables used
 unsigned short adc_reading;
 unsigned short saved_desired_temp;
 unsigned short measured_temp;

//Calling initialization functions for ADC and Uart.
//Uart default Baudrate is 9600.
 Adc_Init(); 
 Uart_Init();
 unsigned char buffer[4];
 
 // Set the PD2 pin as an input with an internal pull-up
 DDRD =  DDRD  & B11111011;
 PORTD = PORTD | B00000100;

 // Set PD3, PD5 as O/P for LEDS and drive them as low
 DDRD =   DDRD | B00101000;
 PORTD = PORTD & B11010111;
 
 while (1) {
  //delay to 100ms will give us 10 readings per second for temperature.
   _delay_ms(100);
   PORTD = PORTD & B11010111;
   // check if the key is pressed
   if((PIND & B00000100) == B00000000){
      saved_desired_temp = Adc_ReadChannel(0);
      PORTD = PORTD | B00001000; // turning on green LED to idicate temperature adjustment
   }
   measuredTemp_bool = not measuredTemp_bool;
   if( measuredTemp_bool) {
      adc_reading = Adc_ReadChannel(1);
      measured_temp = adc_reading;
   } else {
      adc_reading = saved_desired_temp;
   }
  // itoa(adc_reading, buffer, 10);
  // Uart_SendString(buffer);
  // Uart_SendString("\n");
   // Calc Temp
   tempSampleRead = float(adc_reading);
   R2 = (voltageDividerR1*tempSampleRead)/(1023-tempSampleRead); //Below is the equation for the ntc temperature sensor from datasheet
   a = 1/T1;
   b = log10(R1/R2);
   c = b / log10(e);
   d = c / BValue ;
   T2 = 1 / (a- d);
   T2 = T2 - 273.15;
   itoa(T2, buffer, 10); //convert temperature to an integer value
   if( measuredTemp_bool) {
      Uart_SendString("Measured Temp = ");
      Uart_SendString(buffer);
      Uart_SendString(" Deg. C        ");
   } else {
     Uart_SendString("Desired Temp = ");
      Uart_SendString(buffer);
      Uart_SendString(" Deg. C        ");
      Uart_SendString("\n");
   }

   if( measured_temp > saved_desired_temp){
      PORTD = PORTD | B00100000; 
   }
   
 } /* End event loop */
 return (0);
}
