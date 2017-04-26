

#include "headers.h"


void SPI_Initialize(SPI_TypeDef* SPIx)
{
			SPI_InitTypeDef  SPIx_InitStructure;
			GPIO_InitTypeDef  GPIO_InitStructure;

/**  ADS1256 **/	
/**** SPI3 ****/
			if(SPIx == SPI2) {
				
					RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE );	
					RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE );

					/*!< Config SPI2 SCK pin:PB13		*/
					GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
					GPIO_Init(GPIOB, &GPIO_InitStructure);

					/*!< Config SPI2 MOSI pin:PB15		*/
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
					GPIO_Init(GPIOB, &GPIO_InitStructure);
					
					/*!< Config SPI2 MISO pin:PB14,shoud I set the MISO pin as GPIO_Mode_IN_FLOATING */
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
					GPIO_Init(GPIOB, &GPIO_InitStructure);

					
					/*!< Config SPI2 CS pin:PB12 (there is no need to config SPI NSS pin)
					**in ADS1256 situation,SPI NSS pin is connected with GND in default
					*/
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
					GPIO_Init(GPIOB, &GPIO_InitStructure);

					//Config SPI work in mode 
					SPIx_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  
					SPIx_InitStructure.SPI_Mode = SPI_Mode_Master;		
					SPIx_InitStructure.SPI_DataSize = SPI_DataSize_8b;		
					SPIx_InitStructure.SPI_CPOL = SPI_CPOL_Low;		
					SPIx_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; /*notice the spi mode*/
					SPIx_InitStructure.SPI_NSS = SPI_NSS_Soft;    
					SPIx_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	
					SPIx_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	
					SPIx_InitStructure.SPI_CRCPolynomial = 7;

					SPI_Init(SPI2, &SPIx_InitStructure);
					
					SPI_Cmd(SPI2, ENABLE);
					
			}
/**  ADS1256 **/	


/*** SD ***/	
/** SPI3 **/			
			if(SPIx == SPI3) {
				
					RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE );	
					RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE );

					/*!< Config SPI3 SCK pin:PB13		*/
					GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
					GPIO_Init(GPIOB, &GPIO_InitStructure);

					/*!< Config SPI3 MOSI pin:PB15		*/
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
					GPIO_Init(GPIOB, &GPIO_InitStructure);
					
					/*!< Config SPI3 MISO pin:PB14		*/
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
					GPIO_Init(GPIOB, &GPIO_InitStructure);

					
					/*!< Config SPI3 CS pin:PA15		*/
					GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
					GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
					GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
					GPIO_Init(GPIOA, &GPIO_InitStructure);


					SPIx_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  
					SPIx_InitStructure.SPI_Mode = SPI_Mode_Master;		
					SPIx_InitStructure.SPI_DataSize = SPI_DataSize_8b;		
					SPIx_InitStructure.SPI_CPOL = SPI_CPOL_High;		
					SPIx_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; /*NOTICE THE SPI MODE*/
					SPIx_InitStructure.SPI_NSS = SPI_NSS_Soft;    
					SPIx_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	
					SPIx_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	
					SPIx_InitStructure.SPI_CRCPolynomial = 7;

					SPI_Init(SPI3, &SPIx_InitStructure);
					SPI_Cmd(SPI3, ENABLE);	
					
					SPIx_WriteReadByte(SPI3,0xFF);				
			}
/**  SD **/	
			
}


unsigned char SPIx_WriteReadByte(SPI_TypeDef* SPIx, unsigned char TxData)
{
	unsigned  int  retry=0;
	unsigned char  res;

	/*SPI_I2S_FLAG_TXE=1 means send buffer is empty*/
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET) 
	{
		retry++;
		if(retry>=200)
		{
			return 0;
		}
	}
	SPI_I2S_SendData(SPIx,TxData);

	retry = 0;
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET) 
	{
			retry++;
			if(retry>=200)
			{
				return 0;
			}
	}
	res = SPI_I2S_ReceiveData(SPIx);
	return (res);	
}


void SPIx_SetSpeed(SPI_TypeDef* SPIx,unsigned char SpeedType)
{
	if( SPI3 == SPIx)
		{
				SPI3->CR1 &= 0xFFC7; 
		
				if(SPI_LOW_SPEED == SpeedType)
				{
						SPI3->CR1 |= 7<<3; //clk(APB2)/256
				}
				else if(SPI_HIGH_SPEED == SpeedType)
				{
						/*There is some difference between SPI1 and SPI2*/
						SPI3->CR1 |= 0<<3;	//here is a question:what is the maxium speed of SPI1 ?	
				}	
				else
				{
						return ;
				}
				SPI_Cmd(SPI3, ENABLE);
	}
	
}

