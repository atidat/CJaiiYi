#include "headers.h"

#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f) 

PUTCHAR_PROTOTYPE
{
		USART_SendData(USART2,(u8)ch);                                     

		while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET); 

		return ch;
}

void UARTInit(void)
{
	    GPIO_InitTypeDef   GPIO_InitStructure;  
	    USART_InitTypeDef  USART_InitStruct;  
	
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);  
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出 
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	//设置TX引脚
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);
			 
			
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 //上拉输入
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//设置RX引脚
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
			GPIO_Init(GPIOA, &GPIO_InitStructure);

		
	    USART_InitStruct.USART_BaudRate = 9600;  
	    USART_InitStruct.USART_StopBits = USART_StopBits_1;  
	    USART_InitStruct.USART_Parity = USART_Parity_No;  
	    USART_InitStruct.USART_WordLength = USART_WordLength_8b;  
	    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  
	    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
      
		//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		//USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
		
	    USART_Init(USART2,&USART_InitStruct);
	    USART_Cmd(USART2,ENABLE); 
}