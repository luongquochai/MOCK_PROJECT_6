#include "stm8l_define.h"
#include "lcd.h"
#include "gpio.h"
#include "ram.h"

volatile unsigned int counta = 0, countb = 0;
volatile bool press = true, sent = false;
volatile unsigned char recvData;

void CLK_Init(void);
void GPIO_Init(void);
void ITC_Init(void);
void delay_ms(unsigned int ms);

int main( void )
{
  GPIO_Init();
  CLK_Init();
  LCD_Init((1<<1)|(1<<2)|(1<<3)| (1<<4)|(1<<5)|(1<<6)) ;
  asm("RIM\n");
  while (1){
    delay_ms(500);

  }
  
  
  
}

void GPIO_Init(void)
{
  GPIOC->DDR |= (1<<7);
  GPIOC->CR1 |= (1<<7);
  GPIOE->DDR |= (1<<7);
  GPIOE->CR1 |= (1<<7);
  
  GPIOC->DDR &= ~(1<<1);
  GPIOC->CR1 |= (1<<1);
  GPIOC->CR2 |= (1<<1);
 
}
void CLK_Init(void)
{
   CLK->DIVR = 0x00;
   CLK->PCKENR2 |= (1<<3);
   CLK->CRTCR &= ~(7<<5); //RTCDIV=0
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
  LCD_SEG1(GetHexCodeChar('A'));
  COL1 = 1;
  LCD_SEG4(GetHexCodeChar('B'));
  COL2 = 1;
  if(press)
 {
    LCD_SEG2(GetHexCodeNum(counta / 10));
    LCD_SEG3(GetHexCodeNum(counta % 10));
    LCD_SEG5(GetHexCodeNum(countb % 10));
    LCD_SEG6(GetHexCodeNum(countb / 10));
  }
//  Display_string("LOSE");
  LCD->CR3 |=(1<<3); // clear interrupt flag
}
#pragma vector = 11
__interrupt void Press (void)
{
   counta++;
   press = true;
   EXTI->SR1 |= (1<<1);
}

#pragma vector = 30
__interrupt void UART_Recived(void)
{
  GPIOC->ODR ^= (1<<7);
  if(UART->DR = 'b')
  {
    countb++;
    sent = true;
  }
  while (!(UART->SR & (1<<5)));
  recvData  = UART->DR;
  
  UART->SR &= !(1<<5);

}