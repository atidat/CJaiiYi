
#include "headers.h"

unsigned char SDCardType ;


/*-----------------------------------------------
Function		:Wait card for ready status.
Description	:wait success,return 0;failure return 1
*------------------------------------------------*/
unsigned char SDWaitForReady(void)
{
	unsigned int retry=0;
	u8 res;

	do		//E: TEST SPI CHANNEL IS CLEAR
	{
		res = SPIx_WriteReadByte(SPI3,0xFF);
		if(0xFF == res)
			{
				return 0;
			}
		retry++;	
	}while(retry<0xFFFFF);

	return 1;
}

/*-----------------------------------------------
Function		:Wait card for given response.
Description	:wait success,return 0;failure return 1
*------------------------------------------------*/
unsigned char SDGetResponse(unsigned char GivenRes)		//E:WHY SPI MUST RECEIVE GivenRes??????
{
	unsigned int Cnt=0xFFF;
	unsigned char res;

	do
	{
		res =  SPIx_WriteReadByte(SPI3,0xFF);
		if(res == GivenRes)
		{
			return 0;
		}
		Cnt--;
	}while(Cnt>=1);

	return 1;
}

/*------------------------------------------------------------
Function		:Send cmd to sd;if free is 1,release sd bus,else bind sd data bus
Description	:Init success,return 0;failure return 1
*-------------------------------------------------------------*/
unsigned char SDSendCommand(unsigned char cmd,			//E :WHTA DOES MEAN RELEASE / BIND SD DATA BUS
								unsigned int arg,
								unsigned char crc,
								unsigned char free)
{
	unsigned char res;
	unsigned int retry = 0;

	UnselectCard();
	SPIx_WriteReadByte(SPI3,0xFF);
	SPIx_WriteReadByte(SPI3,0xFF);		//E: WHY NEED THESE TWO CODES?????

/*---------------------------------------------------------------------------------------------
Command Format
	Bit position	47 						46  					[45:40]						[39:8] 		[7:1] 		0 
	Width (bits)	1 						1  							6								32					7 			1 
	Value 	      0     				1								x								x						x		  	1
Description:(start bit)(transmission bit) (command index) 	(argument)	( CRC7) (end bit)
---------------------------------------------------------------------------------------------*/

	SelectCard();
	SPIx_WriteReadByte(SPI3,cmd|0x40);		//E:ONE BYTE ONE TIME
	SPIx_WriteReadByte(SPI3,arg>>24);
	SPIx_WriteReadByte(SPI3,arg>>16);
	SPIx_WriteReadByte(SPI3,arg>>8);
	SPIx_WriteReadByte(SPI3,arg);
	SPIx_WriteReadByte(SPI3,crc);
	
	/*the stop command has an execution delay due to the serial command transmission 
	**Skip a stuff byte when stop reading
	**/
	if(CMD12 == cmd) 		//CMD12: STOP DATA TRANSMISSION
			SPIx_WriteReadByte(SPI3,0xFF);

	/*get the cmd response:zm update in 2015-7-2*/
	retry = 0xFF ;
	do
	{
			res = SPIx_WriteReadByte(SPI3,0xFF);
	}while((res&0x80)&&(retry--));
	
// 	while((res=SPIx_WriteReadByte(SPI1,0xFF)) == 0xFF)
// 		{
// 			retry++;
// 			if(retry>600)
// 				{break;}
// 		}

	if(free == RELEASE)
		{
			UnselectCard();
			SPIx_WriteReadByte(SPI3,0xFF);
		}
	
	return res;
	
}


/*-------------------------------------------------------------
Function		:Init SD card and get the card type.
Description	:
*-------------------------------------------------------------*/
unsigned char sdInitStatus;
unsigned char SDCardInitialize(void)
{
	unsigned  int i,retry;
	unsigned char res;
	unsigned char buffer[4];

	SPI_Initialize(SPI3);

	SDCardType = SDCARD_TYPE_ERR ;

	SPIx_SetSpeed(SPI3,SPI_LOW_SPEED);
	
	SelectCard();
	
	for(i=0;i<20;i++) /*--Wait for SD inner finish initilized--*/
	{
			SPIx_WriteReadByte(SPI3,0xFF);
	}

	retry = 0;
	do
	{
		/* In case of SPI host, CMD0 shall be the first command to send the card to SPI mode.*/
			res = SDSendCommand(CMD0,0,0x95,RELEASE);			//E:RESET SD CARD
			retry++;
			if(retry>=200)
			{
					return  SDOtherErr;
			}
	}while(res!=0x01);	/*CMD0 command's response is R1*/				//E:HOW DID HE KNOW THE RESPONSE IS R1??????

	/*--There is some questions at here--*/
	//SelectCard(); /*--(CMD0+CS) signal means that drive SD card convert to spi mode--*/
	
	/*CMD8 is called Send Interface Condition Command;
		**1AA = 0(20bit:reserved bits) 0001(4 bits:VHS) 10101010(8 bits:check pattern) */
	res = SDSendCommand(CMD8,0x1AA,0x87,NORELEASE);
	/*The response length is 5 bytes. The structure of the first (MSB) byte is identical to response type R1.*/
	/*In the argument, ‘voltage supplied’ is set to the host supply voltage and
	check pattern’ is set to any 8-bit pattern,here is 10101010(recommended)*/

	/*If the SD is V2.0 card,including standard or high capacity type*/
	if(res == 0x01) //first (MSB) byte is type R1
		{
			/*The response for CMD8 is R7 format*/
			buffer[0] = SPIx_WriteReadByte(SPI3,0xFF);//arg[24:31]:reserved bits,should be 00
			buffer[1] = SPIx_WriteReadByte(SPI3,0xFF);//arg[16:23]:reserved bits,should be 00
			buffer[2] = SPIx_WriteReadByte(SPI3,0xFF);//arg[8:15]:voltage accepted:should be 0x01,2.7~3.6
			buffer[3] = SPIx_WriteReadByte(SPI3,0xFF);//arg[0:7]:check pattern :should be 0xAA,
			/*The card that accepted the supplied voltage returns R7 response. 
			**In the response, the card echoes back both the voltage range and check pattern set in the argument.*/
			UnselectCard();
			SPIx_WriteReadByte(SPI3,0xFF);
			
			/*If the card supports voltage 2.7~ 3.3(V),then start sending initialize cmd*/
			if((buffer[2]==0x01)&&(buffer[3]==0xAA))
				{
					retry = 0;
					do
					{
						res = SDSendCommand(CMD55,0,0x01,RELEASE);
						if(res!=0x01)
							{
								return  SDOtherErr;
							}
						/*start initialization and to check if the card has completed initialization*/
						res = SDSendCommand(ACMD41,0x40000000,0x01,RELEASE);
						/*the response ACMD41 is in R1 format*/ 

						retry++;
						if(retry>=200)
							{
								return SDOtherErr;
							}
					}while(res!=0);//Setting bit:“in idle state” in the R1 to “0” indicates completion of initialization

					/*send CMD58 to read OCR register*/
					res = SDSendCommand(CMD58,0,0x01,NORELEASE);
					/*caution:the response CMD58 is in R3 format*/  
					if(res!=0x00)//“in idle state” in the R1 to “0” indicates completion of initialization
					{
							return SDOtherErr;
					}
					/*R3 format including the content of OCR register*/
					buffer[0] = SPIx_WriteReadByte(SPI3,0xFF);//VER2HC:0xC0 VER2:0x80 OCR[24:31]
					buffer[1] = SPIx_WriteReadByte(SPI3,0xFF);//VER2HC:0xFF VER2:0xFF	OCR[16:23]
					buffer[2] = SPIx_WriteReadByte(SPI3,0xFF);//VER2HC:0x80 VER2:0x80	OCR[8:15]
					buffer[3] = SPIx_WriteReadByte(SPI3,0xFF);//VER2HC:0x00 VER2:0x00 OCR[0:7]
					UnselectCard();
					SPIx_WriteReadByte(SPI3,0xFF);
					
					/*Jundge the sd card has finished the power up routine*/
					if(buffer[0]&0x80 != 0x80)
						return SDOtherErr;

					if(buffer[0]&0x40)  SDCardType = SDCARD_TYPE_VER2HC;    //检查CCS	 
					else SDCardType = SDCARD_TYPE_VER2;	    
			/*-----------鉴别SD2.0卡版本结束-----------*/ 
		
					SPIx_SetSpeed(SPI1,SPI_HIGH_SPEED);  
					return  0;  //success
				}
			else return SDOtherErr;//The card don't support voltage and check pattern.

		}

	/*If the SD is V1.x card or not a sd card at all */
	else if(res == 0x05)
		{
			UnselectCard();
			SPIx_WriteReadByte(SPI3,0xFF);

			SDCardType = SDCARD_TYPE_VER1;
				
			retry=0;
			do 
			{
				res = SDSendCommand(CMD55,0,0x01,RELEASE);
				if(res!=0x01)
				{
					return  SDOtherErr;
				}
				res = SDSendCommand(ACMD41,0,0x01,RELEASE);

				retry++;
			}while(res!=0x00&&retry<400);

			if(retry>=400)
				{
					SDCardType = SDCARD_TYPE_MMC;
					retry=0;
					/*Initialize MMC card*/
					do
					{
						res = SDSendCommand(CMD1,0x00,0x01,RELEASE);
						retry++;
					}while(res!=0x00&&retry<0xFFFE);

					if(retry>=0xFFFE||SDSendCommand(CMD16,512,0x01,RELEASE)!=0)
						{
							SDCardType = SDCARD_TYPE_MMC;
						}

				}

			
			}
	UnselectCard();
	SPIx_SetSpeed(SPI3,SPI_HIGH_SPEED);

	if(SDCardType)return  0;
	else if(res) return res;
	else return SDOtherErr;
	
}

/*----------------------------------------------------------------------------
Function		:Read length data from card,save it to pdata.Release the data bus by judging free value
Description	:Success,return 0;else return 1
-----------------------------------------------------------------------------*/
unsigned char SDRecvDataFromCard(unsigned char *pdata,unsigned int len,unsigned char free)
{
	unsigned int retry;
	unsigned char res;

	if(len==0)	return 1;
	
	SelectCard();
	
	/*----Wait for start token from card----*/
	retry=0;
	do
	{
		res = SPIx_WriteReadByte(SPI3,0xFF);
		retry++;
		if(retry>=2000)
			{
				UnselectCard();
				return 1;
			}
	}while(res!=0xFE);

	while(len--)
		{
			*pdata = SPIx_WriteReadByte(SPI3,0xFF);
			pdata++;
		}

	SPIx_WriteReadByte(SPI3,0xFF);
	SPIx_WriteReadByte(SPI3,0xFF);//Two dummy CRC

	if(free == RELEASE)
		{
			UnselectCard();
			SPIx_WriteReadByte(SPI3,0xFF);
		}
	
	return 0; //success
	
}

/*----------------------------------------------------------------------------
Function		:Read num block from card,save it to pdata.
Description	:Success,return 0;else return 1
-----------------------------------------------------------------------------*/
unsigned char SDReadNumBlock(unsigned int sector,
								unsigned char *pdata, 
								unsigned char BlockNum)
{
	unsigned char res;
	unsigned char result;

	if(BlockNum <= 0) return 1;

	if(SDCardType != SDCARD_TYPE_VER2HC)
		{
				sector = sector<<9;
		}

	if(1==BlockNum)
		{
			res = SDSendCommand(CMD17,sector,0x01,RELEASE);
			/*response of the CMD17 is in R1 format*/
			if(res!=0x00)
				{return  res;}
			res = SDRecvDataFromCard(pdata,512, RELEASE);
			if(res!=0x00)
				{return  res;}
			else return 0; //success
		}
	else
		{
			res = SDSendCommand(CMD18,sector,0x01,RELEASE);
			/*response of the CMD18 is in R1 format*/
			if(res!=0x00)
				{return  res;}
				
			do
			{
				res = SDRecvDataFromCard(pdata,512,NORELEASE);
				pdata += 512;
			}while((--BlockNum)&&(res==0));/*here is not BlockNum--*/

			/*-!<Send end trasmit CMD(12)---*/
			 result = SDSendCommand(CMD12,0,0x01,RELEASE);
			/*There is some questions about here:what is the right response about CMD12!!!*/
			/*response of the CMD12 is in format R1b,datasheet says:
			When use 2G and 512MB sd card(kingston),the response is 0x00,
			while the response is 0x7F using 4G SD card(SanDisk) and 8G SD card (TOSHIBA)
			*/
			
		}
	UnselectCard();
	SPIx_WriteReadByte(SPI3,0xFF);

	if(BlockNum>0)
		{
			return BlockNum;
		}
	return res;
	
}

/*----------------------------------------------------------
Function		:Read num block from card,save it to pdata.
Description	:Success,return 0;else return 1
-----------------------------------------------------------*/
unsigned char SDWriteNumBlock( unsigned int sector,
						 		 const unsigned char *pdata, 
						  		 unsigned char BlockNum)
{
	unsigned int i;
	unsigned int retry;
	unsigned char res;

	if(BlockNum <= 0) return 1;

	if(SDCardType != SDCARD_TYPE_VER2HC)
		{
			sector = sector<<9;
		}

	SelectCard();
	
	if(1 == BlockNum)
		{
			
			res = SDSendCommand(CMD24,sector,0x01,NORELEASE);
			if(res==0)
				{
			
					SPIx_WriteReadByte(SPI3, 0xFF);				
					SPIx_WriteReadByte(SPI3, 0xFF);
					SPIx_WriteReadByte(SPI3, 0xFF);	

					/*!<----Start token----*/
					SPIx_WriteReadByte(SPI3, 0xFE);
					for(i=0;i<512;i++)
						{
							SPIx_WriteReadByte(SPI3,*pdata);//send data
							pdata++;
						}
					/*!<----Dummy CRC----*/
					SPIx_WriteReadByte(SPI3, 0xFF);
					SPIx_WriteReadByte(SPI3, 0xFF);

					/*Wait response from card*/
					res = SPIx_WriteReadByte(SPI3, 0xFF);
					
					/*recieve data response token from sdcard:x x x 0 status(3 bits) 1*/
					if((res&0x1F)!=0x05)//res should be 0xE5
					{
						UnselectCard();
						return res;
					}
					
					
					res = SDWaitForReady();
					return res;					
				}
			/*if not recieve a right response of CMD24,return error*/
			else return 1; //error
		}
	
	else  //Block number>1
		{
				if(SDCardType != SDCARD_TYPE_MMC)
					{
						/*----Send erase block command---*/
						/*-CMD 55 res in format R1,be 0x00,when debug,shoule checkk the res value*/	
						res = SDSendCommand(CMD55,0,0x01,RELEASE);	
						/*-CMD AMD23 res in format R1,be 0x00,when debug,shoule checkk the res value*/							
						res = SDSendCommand(ACMD23,BlockNum,0x01,RELEASE);						
					}
				
				res = SDSendCommand(CMD25,sector,0x01,NORELEASE);
				if(res!=0x00)
					{
						UnselectCard();
						return res;
					}
											
				do
					{
									/*---!<Dummy data stream---*/
								SPIx_WriteReadByte(SPI3, 0xFF);
								SPIx_WriteReadByte(SPI3, 0xFF);
								SPIx_WriteReadByte(SPI3, 0xFF);

								/*---!<Multi block write start transmission token---*/
								SPIx_WriteReadByte(SPI1, 0xFC);
								
								for(i=0;i<512;i++)
									{
										SPIx_WriteReadByte(SPI3, *pdata);
										pdata++;
									}
								
								SPIx_WriteReadByte(SPI3, 0xFF);//Dummy CRC
								SPIx_WriteReadByte(SPI3, 0xFF);//Dummy CRC
								
								res = SPIx_WriteReadByte(SPI1, 0xFF);
						/*---Response should be xxx0 0101b,means data write successfully---
					**during Write Multiple Block operation, the host shall stop the
			data transmission using CMD12. In case of a Write Error (response ’110’), 
			the host may send CMD13(SEND_STATUS) in order to get the cause of the write problem. 
			ACMD22 can be used to find the number of well written write blocks.
			*/
								if((res&0x1F)!=0x05)
									{
										UnselectCard();
										return res;
									}

							/*---Wait for writing data to sdcard finished---*/
								if((SDWaitForReady()) != 0)
									{
											UnselectCard();
											SPIx_WriteReadByte(SPI3, 0xFF);
											return 1;
									}
													
							}while(--BlockNum);
			
			/*---Send write over command to card---*/ 
			res = SPIx_WriteReadByte(SPI3, 0xFD); 
			if(0x00 == res) /*!<---Should be 0xFF---*/
				{
					 	return 1;
				}
			
			if(SDWaitForReady()) 
   			 {
   			 		UnselectCard();
						SPIx_WriteReadByte(SPI3, 0xFF); 	
      			return 1;  
    			}
  
			UnselectCard();
			SPIx_WriteReadByte(SPI3, 0xFF); 
			return  BlockNum;
		}

}

/*----------------------------------------------------------
Function		:Read card register CSD.
Description	:Success 0;else return other.
warning	:the program is fault,we can not use it
-----------------------------------------------------------*/
unsigned char pDa[128];
u8 SDGetRegisterCSD(unsigned char  *pdata)
	{
		unsigned char  res;	 
		
		unsigned char i;
				
		res=SDSendCommand(CMD9,0,0x01,NORELEASE);//发CMD9命令，读CSD
 		if(res==0)
 			{
					res = SDRecvDataFromCard(pdata, 16, RELEASE);//接收16个字节的数据 
			}
		UnselectCard();
		if(res)
			{
				return res;
			}
		else
			{
				return 0;
			}
	}  


/*----------------------------------------------------------
Function		:Read media volum capacity.
Description	:Success,return capacity,else return 0
warning	:the program is fault,we can not use it
-----------------------------------------------------------*/
unsigned int  SDGetMediaCapacity(void)
	{
	unsigned char  csd[16];
	unsigned int Capacity;
	unsigned char r1;
	unsigned short i;
	unsigned short temp;  	
	
	//取CSD信息，如果期间出错，返回0
	if(SDGetRegisterCSD(csd)!=0) return 0;	    
	//如果为SDHC卡，按照下面方式计算
	if((csd[0]&0xC0)==0x40)
		{									  
		Capacity=((u32)csd[8])<<8;
		Capacity+=(u32)csd[9]+1;	 
		Capacity = (Capacity)*1024;//得到扇区数
		Capacity *= 512;//得到字节数			   
		}
	else
		{		    
		i = csd[6]&0x03;
		i<<=8;
		i += csd[7];
		i<<=2;
		i += ((csd[8]&0xc0)>>6);
		//C_SIZE_MULT
		r1 = csd[9]&0x03;
		r1<<=1;
		r1 += ((csd[10]&0x80)>>7);	 
		r1+=2;//BLOCKNR
		temp = 1;
		while(r1)
			{
			temp*=2;
			r1--;
			}
		Capacity = ((u32)(i+1))*((u32)temp);	 
		// READ_BL_LEN
		i = csd[5]&0x0f;
		//BLOCK_LEN
		temp = 1;
		while(i)
			{
			temp*=2;
			i--;
			}
		//The final result
		Capacity *= (u32)temp;//字节为单位 	  
		}
	return (u32)Capacity;
	}	  
	  																			    




