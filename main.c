#include "stm8l_define.h"
#include "lcd.h"
#include "gpio.h"
#include "ram.h"
#include "uart.h"

signed int counta = 0, countb = 0;
volatile bool press = true, sent = false;
volatile unsigned char recvData = '0';
volatile unsigned int done = 0;

void CLK_Init(void);
void GPIO_Init(void);
void ITC_Init(void);
void delay_ms(unsigned int ms);


int main( void )
{
  GPIO_Init();
  CLK_Init();
  UART_Init();

  LCD_Init((1<<2)|(1<<3)| (1<<4)|(1<<5)|(1<<6)) ;
  asm("RIM\n");
  while(!(UART->SR & 0x80));
    UART->DR = 'r';  //goi tin hieu reset
  while (1){

    
  }
 
}

void GPIO_Init(void)
{
  GPIOC->DDR |= (1<<7);
  GPIOC->CR1 |= (1<<7);
  GPIOE->DDR |= (1<<7);
  GPIOE->CR1 |= (1<<7);
  GPIOC->DDR &= ~(1<<2);
  GPIOC->CR1 |= (1<<2);
  
  GPIOC->DDR &= ~(1<<1);
  GPIOC->CR1 |= (1<<1);
  GPIOC->CR2 |= (1<<1);
  ITC_Init();
}
void CLK_Init(void)
{
   CLK->DIVR = 0x00;
   CLK->ICKR |= (1<<0);
   while (CLK->SCSR != 0x01);
   CLK->PCKENR2 |= (1<<3);
   CLK->PCKENR1 |= (1<<5);
   CLK->CRTCR |= (1<<4); //RTC-LSE
   while(!(CLK->ECKR &(1<<3)));
   GPIOE->ODR |= (1<<7);
}

void ITC_Init(void)
{
  EXTI->CR1 |=(1<<3);
  ITC->ISPR3 |= (3<<2);
  ITC->ISPR8 |= (3<<0);
}


void delay_ms(unsigned int ms)
{
  register unsigned int i = 0; 
  while(1< ms*100)
    i++;
}


#pragma vector = 18
__interrupt void Display(void)
{
     if(recvData == 'r')
  {
    counta = 0;
    countb = 0;
    UART->CR2 |= (1<<2);
  }
//  if(recvData == 'b')
 // {
  // countb++;
  //  recvData = '0';
 // }
   if(recvData == 'a')
 {
    counta++;
    recvData = '0';
  }
  //check a &b
  if(counta>=11)
  {
    if((counta - countb) >=2 )
    {
      LCD->CR3 |=(1<<3); // clear interrupt flag
      //Display_string("WIN");
      Display_string("LOSE");
      done = 1;
      UART->CR2 &= ~(1<<2);
    }
  }
  if(countb >= 11)
  {
    if((countb - counta) >= 2 )
    {
      LCD->CR3 |= (1<<3);
      //Display_string("LOSE");
      Display_string("WIN");
      done = 1;
      UART->CR2 &= ~(1<<2);
    }
  }
  if((counta > 99) && (done == 0 ))
  {
    Display_string("WIN");
    LCD->CR3 |= (1<<3);
  }
  if((countb > 99) && (done == 0))
  {
    Display_string("WIN");
    LCD->CR3 |= (1<<3);
  }
  if((counta<=99)&&(done==0))   {
   
   LCD_SEG2(GetHexCodeNum(counta /10));
   LCD_SEG3(GetHexCodeNum(counta %  10));
   LCD_SEG5(GetHexCodeNum(countb /10));
   LCD_SEG6(GetHexCodeNum(countb %10));
  
  }


    

  LCD->CR3 |=(1<<3); // clear interrupt flag
   

}

#pragma vector = 11
__interrupt void Press (void)
{
   
   press = true;
   //counta++;
  // UART_Transmit('a');

    countb++;
     UART_Transmit('b');
    

   EXTI->SR1 |= (1<<1);
}

#pragma vector = 30
__interrupt void UART_Recived(void)
{
  
  GPIOC->ODR ^= (1<<7);
  sent = true;
  recvData = UART->DR;
  // //if(recvData == 'b')
  //   //countb++;
  //  if(recvData == 'a')
  //    counta++;
 

}
//asdasdasdasdsa