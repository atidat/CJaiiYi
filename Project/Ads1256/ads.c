#include "headers.h"


/*--------config the GPIO for ADS1256 data ready pin---------*/
static void ADS1256_Config_GPIO(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
	
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD ,ENABLE );	
/*--------PD-10---------*/			
		GPIO_InitStructure.GPIO_Pin = ADS_DRDY_PIN;
		GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
		GPIO_Init(ADS_DRDY_PORT, &GPIO_InitStructure);
}


unsigned char ADS1256_ReadSingleReg(unsigned char RegAddr)
{
		unsigned char readByteData ;

		/*1st Command Byte: 0001 rrrr where rrrr is the address of the first register to read*/
		SPIx_WriteReadByte(SPI2, CMD_RREG|RegAddr);

		/*2nd Command Byte: 0000 nnnn where nnnn is the number of bytes to read ¨C 1*/
		SPIx_WriteReadByte(SPI2, 0x00);	/*here nnnn is  0 = 1-1 */

		/*notice the timing definition of T6*/
		DelayNumUs(10);

		readByteData = SPIx_WriteReadByte(SPI2, 0xFF);

		return readByteData ;
}


void ADS1256_WriteReg(unsigned char RegAddr, unsigned char RegValue)
{
		/*1st Command Byte: 0101 rrrr where rrrr is the address to the first register to be written.*/
		 SPIx_WriteReadByte(SPI2, CMD_WREG | RegAddr);
	
		 /*2nd Command Byte: 0000 nnnn where nnnn is the number of bytes to be written*/
		 SPIx_WriteReadByte(SPI2, 0x00);	/*here nnnn is  0 = 1-1 */
	
		 /*Data Byte(s): data to be written to the registers.*/
		 SPIx_WriteReadByte(SPI2, RegValue);
	
		 DelayNumUs(6);
	
}

void ADS1256_SetSingleChannel(unsigned char ch)
{
		if(ch>7)	return	;

		/*for single-end analog input mode,ch value set postive analog input
		**while negative input channel(AINN)is set with AINCOM in default(bit3 = 1)			
		*/

		ADS1256_WriteReg(REG_MUX ,((ch<<4)|(1<<3)));

}

void ADS1256_SetDiffChannel(unsigned char ch)
{
		if(ch>4)	return ;
	
		switch(ch)
		{
				case 0:ADS1256_WriteReg(REG_MUX ,(0 << 4) | 1);//AINP:IN0	AINN:IN1
					break;
				case 1:ADS1256_WriteReg(REG_MUX ,(2 << 4) | 3);//AINP:IN2	AINN:IN3
					break;
				case 2:ADS1256_WriteReg(REG_MUX ,(4 << 4) | 5);//AINP:IN4	AINN:IN5
					break;
				case 3:ADS1256_WriteReg(REG_MUX ,(6 << 4) | 7);//AINP:IN6	AINN:IN7
					break;
				default:
					break;				
		}
}


void ADS1256_SetDataRate(unsigned char sps)
{
		while(ReadDrdyLine()); /*MCU can write register only in ready line is low*/
	
		ADS1256_WriteReg(REG_DRATE,sps);
	
		DelayNumUs(10);
	
		/*After changing the PGA, data rate, buffer status, writing to the OFC or FSC registers, 
		and enabling or disabling the	sensor detect circuitry,perform a synchronization operation 
		to force DRDY high. It will stay high until valid data is ready*/
		ADS1256_SetSynCommand();
}
	
void ADS1256_SetCommand(unsigned char cmd)
{
		SPIx_WriteReadByte(SPI2, cmd);
}



void ADS1256_SetSynCommand(void)
{
		SPIx_WriteReadByte(SPI2, CMD_SYNC);
	
		/*Here is some questions about time delay*/
		//ADS1256_DelayNumUs(12);
	
		SPIx_WriteReadByte(SPI2, CMD_WAKEUP);//wakeup command
	
		/*Here is some questions about time delay*/
		//ADS1256_DelayNumUs(12);	
}


void ADS1256_SetCalibrationCommand(void)
{
		/*what's the ready-line status when send Calibration command*/
		//while(ReadDrdyLine());
		ADS1256_SetCommand(CMD_SELFCAL);
	
		/*DRDY goes high at the beginning of the calibration*/
		while(!ReadDrdyLine());
	
		/*DRDY goes low after the calibration completes and settled data is ready*/
		while(ReadDrdyLine());
	
		//printf("ADS1256 has configured!");
	
}


int ADS1256_ReadOneChannelValue(unsigned char ch)
{
		unsigned int digitValue = 0 ; 

		/*select channel*/
		while(ReadDrdyLine());
		ADS1256_SetSingleChannel(ch);
	
		while(ReadDrdyLine());
		ADS1256_SetSynCommand();	
		
	/*After a synchronization operation,DRDY stays high until valid data is ready*/
																					while(!ReadDrdyLine());
	
	/*Issue command:RDATA after DRDY goes low to read a single conversion result*/
		while(ReadDrdyLine());	
		ADS1256_SetCommand(CMD_RDATA);
	
	/*	Timing sequence:T6	*/
		DelayNumUs(10);

		digitValue = (SPIx_WriteReadByte(SPI2, 0xFF)<<16);
		digitValue += (SPIx_WriteReadByte(SPI2, 0xFF)<<8);
		digitValue += SPIx_WriteReadByte(SPI2, 0xFF);
		
		if(digitValue & 0x800000)//here is some question
				digitValue += 0xFF000000;
		
		return digitValue ;			
		
}


int DigitalValue[CHCOUNT];
void ADS1256_ReadMultiChannelValue(int *pDigitValue,unsigned char chNum)
{
		unsigned char idx = 0;
	
		if(0 == chNum)
			return ;
		
		for(idx=0;idx<chNum;idx++)
		{
				pDigitValue[idx] = ADS1256_ReadOneChannelValue(idx);
		}
}


void ADS1256_Init(void)
{
		/*Init data ready line */
		ADS1256_Config_GPIO();
	
		/*Init SPI2 for ADS1256 interface */
		SPI_Initialize(SPI2); 
	
		ADS1256_SetCommand(CMD_RESET);
		DelayNumUs(100000);	//wait for Vref is stable
	
		/*it is strongly recommended to perform an additional self-calibration by issuing 
		the SELFCAL command after the power supplies and voltage reference have had time
		to settle to their final values in pdf.27*/
		ADS1256_SetCalibrationCommand();
	
		while(ReadDrdyLine());
	/*Most Significant Bit First,Auto-Calibration Enabled,Buffer disabled:11110111b */
		ADS1256_WriteReg(REG_STATUS ,0xF7);
	
		while(ReadDrdyLine());
		/*PGA=1 */
		ADS1256_WriteReg(REG_ADCON,0x00);
	
		while(ReadDrdyLine());
	/*select channle AIN0 as initial analog input*/
		ADS1256_SetSingleChannel(0);
	
		while(ReadDrdyLine());
	/*set data rate as sps as initial rate*/
		ADS1256_WriteReg(REG_DRATE,RATE_10SPS);	
		
		ADS1256_SetCalibrationCommand();
			
		/*After changing the PGA, data rate, buffer status, writing to the OFC or FSC registers, 
		and enabling or disabling the	sensor detect circuitry,perform a synchronization operation 
		to force DRDY high. It will stay high until valid data is ready*/
		ADS1256_SetSynCommand();
		
}


ADS1256_STATUS adcStatus ;/*ADS1256 status variable*/
unsigned char  ADS1256_readStatus(ADS1256_STATUS *ptr)
{
		unsigned char read[4] ;
		unsigned char FirstRegAddr = 0x00 ;
		unsigned char idx ;

		while(ReadDrdyLine());	
		/*1st Command Byte: 0001 rrrr where rrrr is the address of the first register to read*/
		SPIx_WriteReadByte(SPI2, CMD_RREG|FirstRegAddr);

		/*2nd Command Byte: 0000 nnnn where nnnn is the number of bytes to read ¨C 1*/
		 SPIx_WriteReadByte(SPI2, 0x03);	/*here nnnn is  3 = 4-1 ,read 4 register*/

		/*notice the timing definition of T6*/
		DelayNumUs(10);
		
		for(idx=0;idx<4;idx++)
			read[idx] = SPIx_WriteReadByte(SPI2, 0xFF);

		/*Analyse the register value*/
		ptr->identID = (read[0]>>4) ;
		ptr->dataOrder = (read[0]&(1<<3));
		ptr->autoCalibration = (read[0]&(1<<2));
		ptr->bufferStatus = (read[0]&(1<<1));
		
		ptr->muxValue = read[1];
		ptr->pgaValue = (read[2]&0x07);
		ptr->dataRate = read[3];
		
		/*Judge if ads1256 was configed correct*/
		if( (ptr->bufferStatus&(1<<1)) && (ptr->autoCalibration&(1<<2)) )
				return 0; //config the ads1256 success
		else return 1;	//false			
			
}