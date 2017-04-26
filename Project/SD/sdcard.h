


#ifndef   __DRIVER_SDCARD_H__
#define  __DRIVER_SDCARD_H__

/*definition of sdcard type*/
#define  SDCARD_TYPE_ERR   0
#define  SDCARD_TYPE_MMC  1
#define  SDCARD_TYPE_VER1  2
#define  SDCARD_TYPE_VER2   4
#define  SDCARD_TYPE_VER2HC  8

/* SD传输数据结束后是否释放总线宏定义 */
#define   NORELEASE 0
#define   RELEASE 1

#define SDOtherErr 0xaa
#define STA_NODISK 0x02

#define  BLOCKSIZE  512

/*定义SD卡常用指令表*/
#define CMD0   0x00     //卡复位
#define CMD1	1		//MMC初始化命令
#define CMD8    8		//接口检测命令
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置SectorSize 应返回0x00
#define CMD17   17      //命令17，读single sector
#define CMD18   18      //命令18，读Multi sector
#define ACMD23  23      //命令23，设置多sector写入前预先擦除N个block
#define CMD24   24      //命令24，写sector
#define CMD25   25      //命令25，写Multi sector
#define ACMD41  41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00



/*配置SD卡片选引脚*/
#define  CardSelectPort  	GPIOA
#define  CardSelectPin		GPIO_Pin_15
#define  SelectCard()  	  (GPIO_ResetBits(CardSelectPort,CardSelectPin))
#define  UnselectCard()		(GPIO_SetBits(CardSelectPort,CardSelectPin))


extern unsigned char SDCardType ;

extern unsigned char sdInitStatus;
extern unsigned char SDCardInitialize(void);

extern unsigned char SDReadNumBlock(unsigned int sector,
								unsigned char *pdata, 
								unsigned char BlockNum);

extern unsigned char SDWriteNumBlock( unsigned int sector,
						 		 const unsigned char *pdata, 
						  		 unsigned char BlockNum);

extern unsigned int  SDGetMediaCapacity(void);



#endif 






