

#ifndef __SPI_H__
#define __SPI_H__


#define  SPI_LOW_SPEED   0
#define  SPI_HIGH_SPEED  1

extern void SPI_Initialize(SPI_TypeDef* SPIx);
extern unsigned char SPIx_WriteReadByte(SPI_TypeDef* SPIx, unsigned char TxData);
extern void SPIx_SetSpeed(SPI_TypeDef* SPIx,unsigned char SpeedType);

#endif