

#include "headers.h"


DSTATUS disk_initialize (BYTE drv)   /* Physical drive nmuber (0..) */
{
    unsigned char  status;

		switch (drv)
        {
						case 0 :
								status = SDCardInitialize();
								if(0 == status)
								{
										 return RES_OK;
								}
								else
								{ 
										/*fix spi bug*/
										SPIx_SetSpeed(SPI3,SPI_LOW_SPEED);
										SPIx_WriteReadByte(SPI3,0xFF);
										SPIx_SetSpeed(SPI3,SPI_HIGH_SPEED);
										return STA_NOINIT;
								}                
						default:
								return STA_NOINIT;
        } 
			
}



DRESULT disk_read (BYTE drv, BYTE *buff, DWORD sector, BYTE count)
{
		unsigned char status;
		
		if(0 == count)
			{
				return RES_PARERR;
			}
		
		status =  SDReadNumBlock(sector,(unsigned char*)buff, count);
		
		if(0 == status)
			{
				return RES_OK;
			}
		else 
		{
			/*fix spi bug*/
			SPIx_SetSpeed(SPI3,SPI_LOW_SPEED);
			SPIx_WriteReadByte(SPI3,0xFF);
			SPIx_SetSpeed(SPI3,SPI_HIGH_SPEED);
			return RES_ERROR;
		}			
		
}

 
DRESULT disk_write (BYTE drv, const BYTE *buff, DWORD sector, BYTE count)
{
		unsigned char status;
		
		if(0 == count)
			{
				return RES_PARERR;
			}

		status = SDWriteNumBlock(sector,(const unsigned char *)buff,count);
		
		if(0 == status)
			{
				return RES_OK;
			}
		else {return RES_ERROR;}			
		
}


DSTATUS disk_status (BYTE drv )
{
		return  RES_OK;
}

 
DRESULT disk_ioctl (BYTE drv, BYTE ctrl, void* buff)
{
		return RES_OK;
}


DWORD get_fattime(void)
{
		return 0;
}
