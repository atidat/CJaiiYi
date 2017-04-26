
#ifndef __ADS_H__
#define __ADS_H__


#define CHCOUNT 5

#define ADS_LSB_WEIGHT ((2*2.489)/(0x7FFFFF))

#define ADS_DRDY_PORT GPIOD
#define ADS_DRDY_PIN GPIO_Pin_10

#define ReadDrdyLine()	(GPIO_ReadInputDataBit(ADS_DRDY_PORT,ADS_DRDY_PIN))


/*ads1256 command definition */
enum
{
		 CMD_WAKEUP  = 0x00, // Completes SYNC and Exits Standby Mode 0000  0000 (00h)
		 CMD_RDATA   = 0x01, // Read Data 0000  0001 (01h)
		 CMD_RDATAC  = 0x03, // Read Data Continuously 0000   0011 (03h)
		 CMD_SDATAC  = 0x0F, // Stop Read Data Continuously 0000   1111 (0Fh)
		 CMD_RREG    = 0x10, // Read from REG rrr 0001 rrrr (1xh)
		 CMD_WREG    = 0x50, // Write to REG rrr 0101 rrrr (5xh)
		 CMD_SELFCAL = 0xF0, // Offset and Gain Self-Calibration 1111    0000 (F0h)
		 CMD_SELFOCAL= 0xF1, // Offset Self-Calibration 1111    0001 (F1h)
		 CMD_SELFGCAL= 0xF2, // Gain Self-Calibration 1111    0010 (F2h)
		 CMD_SYSOCAL = 0xF3, // System Offset Calibration 1111   0011 (F3h)
		 CMD_SYSGCAL = 0xF4, // System Gain Calibration 1111    0100 (F4h)
		 CMD_SYNC    = 0xFC, // Synchronize the A/D Conversion 1111   1100 (FCh)
		 CMD_STANDBY = 0xFD, // Begin Standby Mode 1111   1101 (FDh)
		 CMD_RESET   = 0xFE // Reset to Power-Up Values 1111   1110 (FEh)
};


/* register address,follow with reset value */
enum
{
		 REG_STATUS = 0,	// x1H
		 REG_MUX    = 1, // 01H
		 REG_ADCON  = 2, // 20H
		 REG_DRATE  = 3, // F0H
		 REG_IO     = 4, // E0H
		 REG_OFC0   = 5, // xxH
		 REG_OFC1   = 6, // xxH
		 REG_OFC2   = 7, // xxH
		 REG_FSC0   = 8, // xxH
		 REG_FSC1   = 9, // xxH
		 REG_FSC2   = 10 // xxH
};


/* ads1256 sps enum ,range from 2.5 to 30,000 sps */
enum
{
			RATE_30000SPS = 0xF0,	//default value 
			RATE_15000SPS = 0xE0,
			RATE_7500SPS = 0xD0,
			RATE_3750SPS = 0xC0,
			RATE_2000SPS = 0xB0,
			RATE_1000SPS = 0xA1,
			RATE_500SPS = 0x92,
			RATE_100SPS = 0x82,
			RATE_60SPS = 0x72,
			RATE_50SPS = 0x63,
			RATE_30SPS = 0x53,
			RATE_25SPS = 0x43,
			RATE_15SPS = 0x33,
			RATE_10SPS = 0x23,
			RATE_5SPS = 0x13,
			RATE_2O5SPS = 0x03
};


typedef struct
{
	unsigned char identID;
	unsigned char dataOrder ;
	unsigned char autoCalibration;
	unsigned char bufferStatus ;	
	unsigned char muxValue;
	unsigned char pgaValue;
	unsigned char dataRate;
	
}ADS1256_STATUS;



extern ADS1256_STATUS adcStatus ;/*ADS1256 status variable*/

extern void ADS1256_DelayNumUs(unsigned int nus);

extern void ADS1256_SetCommand(unsigned char cmd);
extern void ADS1256_SetSynCommand(void);
extern void ADS1256_SetCalibrationCommand(void);

extern void ADS1256_WriteReg(unsigned char RegAddr, unsigned char RegValue);
extern void ADS1256_SetDataRate(unsigned char sps);

extern void ADS1256_SetSingleChannel(unsigned char ch);

extern void ADS1256_Init(void);

#endif