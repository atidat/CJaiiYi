


#ifndef   __DRIVER_SDCARD_H__
#define  __DRIVER_SDCARD_H__

/*definition of sdcard type*/
#define  SDCARD_TYPE_ERR   0
#define  SDCARD_TYPE_MMC  1
#define  SDCARD_TYPE_VER1  2
#define  SDCARD_TYPE_VER2   4
#define  SDCARD_TYPE_VER2HC  8

/* SD�������ݽ������Ƿ��ͷ����ߺ궨�� */
#define   NORELEASE 0
#define   RELEASE 1

#define SDOtherErr 0xaa
#define STA_NODISK 0x02

#define  BLOCKSIZE  512

/*����SD������ָ���*/
#define CMD0   0x00     //����λ
#define CMD1	1		//MMC��ʼ������
#define CMD8    8		//�ӿڼ������
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD16   16      //����16������SectorSize Ӧ����0x00
#define CMD17   17      //����17����single sector
#define CMD18   18      //����18����Multi sector
#define ACMD23  23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��дsector
#define CMD25   25      //����25��дMulti sector
#define ACMD41  41      //����41��Ӧ����0x00
#define CMD55   55      //����55��Ӧ����0x01
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00



/*����SD��Ƭѡ����*/
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






