
#include "headers.h"

/*************************************************
file function		:definition of file operation
*************************************************/

/*notice:the program used to definite this variable as global variable
**
**warning: global variable is stored in static memory zone 
*/

FRESULT res;      
UINT    bw,br;        

FATFS fs,*pFs ;       // Work area (file system object) for logical drive

FIL   fsrc;            // file structure

unsigned int fileSize=0;  
unsigned int usedCapacity ;

unsigned char csd_buffer[16];


/********************************************************************
function		:write data into the given file
parameter
	name							pBuffer 							byteCount					path					
	Description: (data buffer pointer)	(count to write)		(file path)
return			:success return 0,else return 1
warn				:the byteCount should be valid length = strlen(pBuff)+1
*********************************************************************/
unsigned char fileHeader[70] = "##Ye-Mo-Da Ho:Mi:Se,xMagValue,yMagValue,zMagValue,aMagValue\r\n";
unsigned char sdWriteDataInFile(unsigned char *pBuffer,
												unsigned int byteCount,
												const char *path)
{
			
			if(NULL==pBuffer || byteCount==0)
					return 1;
			
			/*Mount a Logical Drive,notice unmount */
			if((f_mount(0,&fs))!= FR_OK)
				return 1; //error
			
			/*if not exist,create a new file,in write\read way*/
			res = f_open(&fsrc,path,FA_WRITE | FA_OPEN_ALWAYS | FA_READ);
			
			if(FR_OK == res)
					{
							if(fsrc.fsize == 0)
							{
									res = f_write(&fsrc,fileHeader,strlen(fileHeader),&bw);	
							}
							/*if lseek success,and write data,or close file*/
							if (f_lseek(&fsrc,fsrc.fsize) == FR_OK )
									res = f_write(&fsrc,pBuffer,byteCount,&bw);	
							
							/*if f_lseek is failed,should close the file*/
							f_close(&fsrc);						
					}
					
			/*unmount logical drive*/		
			f_mount(0,NULL);
			if(FR_OK==res && byteCount==bw)
						return 0; //success
			
			return 1;//false	
			
}


/*******************************************************
function		:read data from the given file
parameter
	name							pCount						path					
	Description: (count to read)		(file path)
return			:success return 0,else return 1
warn				:the byteCount should be valid length = strlen(pBuff)+1
********************************************************/
unsigned char uploadBuffer[512];
unsigned char sdReadDataFromFile(const char *path,unsigned int *pCount)
{
		unsigned short idx ;
		unsigned char volumStr[20];
		
		*pCount = 0 ; br = 0;
		if( (f_mount(0,&fs)) != FR_OK)
				return 1; //error
		
		memset(volumStr,'\0',sizeof(volumStr));
		if(f_open(&fsrc,path,FA_READ) == FR_OK)
		{
				sprintf(volumStr,"%9d",fsrc.fsize);
				UartSendString(COM2,volumStr) ;		
		}			
		else
		{
				return 1;//error
		}
		
		while(1)
		{
				if((f_read(&fsrc,uploadBuffer,sizeof(uploadBuffer)-1,&br)) == FR_OK)
				{
						for(idx=0;idx<br;idx++)
						{
								/*usart send data and wait finished send*/
								USART_SendData(USART2, uploadBuffer[idx]);
								while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
						}
						*pCount += br ;
						if(br < sizeof(uploadBuffer)-1)//finished send
							break ;
				}
				else break ;
		}
		
		f_close(&fsrc);
		f_mount(0,NULL);
		
}

/********************************************************************************
function		:get the used capacity of sd card
parameter
	name							totalSize									usedSize					
	Description: (pointer to initial capacity)		(pointer to used capacity)
output			:the initial and used capacity in KB unit
return			:sucess return 0,error return 1
author			:retryming@163.com 		#2015-7-4 
********************************************************************************/
unsigned int initialCardCapacity,usedCardCapacity;
unsigned char getCardCapacity(unsigned int *totalSize,unsigned int *usedSize)
{
		DWORD  free_cluster; //number of free clusters		
		DWORD used_sector,free_sector,total_sector ;
		
		/*Mount a Logical Drive,notice unmount */
		if( f_mount(0,&fs) != FR_OK )
			return 1;
		
		/*root directory,select Logical drive number:0 
		@**gets number of the free clusters on the volume
		**/
		if( f_getfree("0:",&free_cluster,&pFs) == FR_OK ) 
		{
				/*pFs->csize = sector number in a cluster*/
				total_sector = (pFs->n_fatent - 2)*(pFs->csize) ;				
				free_sector = free_cluster*(pFs->csize);
				used_sector = total_sector - free_sector ;
				/*default sector contain 512 bytes */
						
				*totalSize = ( (total_sector>>1)/(1000) );//in MB unit
				*usedSize = (used_sector>>1) ;//in KB unit
				
				f_mount(0,NULL);
				return 0 ;				
		}
				
		f_mount(0,NULL);
		return 1;			
		
}


/***********************************************************
function		:get file name list in given path
input				:directory path
output			:file name string array
return			:success return 0,fail return 1
author			:retryming@163.com 		#2015-7-1
notice			:the file name must be in format:09051503.txt
***********************************************************/
FILE_NAME_STRUCT fileNameArray[256] ;//file list array
int sdReadFileList(const char *path,unsigned char *fileNum)
{
		FILINFO finfo;	 /* File status structure (FILINFO) */
		DIR   DirObject;       //directory structure
	
		unsigned char idx = 0 ;
		unsigned char fileLen = 13; //short file length
	
		/*empty the global variable,string end flag '\0' ascii is 0*/
		memset(fileNameArray,'\0',sizeof(fileNameArray));
	
		/*Mount a Logical Drive,notice unmount */
		if( f_mount(0,&fs) != FR_OK )
			return 1;
		
		if( f_opendir(&DirObject,path) == FR_OK)
		{
					while(f_readdir(&DirObject,&finfo)== FR_OK)
					{
							if(!finfo.fname[0])
							{
										*fileNum = idx;
										break;
							}
							idx += 1;
							/*void *memcpy(void *dest, const void *src, size_t n)*/
							/*example:strlen(09051518.txt)=12*/
							memcpy(fileNameArray[idx].fileName,finfo.fname,(sizeof(finfo.fname)-1));//except '\0'
					}
					f_mount(0,NULL);
					return 0; //success return 0
		}
		f_mount(0,NULL);
		return 1;		
		
}


/***************************************************
function		:create a new directory in sd card
input				:directory path
return			:
author			:retryming@163.com 		#2015-7-3
****************************************************/
unsigned char sdMakeDirectory(const char *path)
{
		/*Mount a Logical Drive,notice unmount */
		if( f_mount(0,&fs) != FR_OK )
			return 1;	
		
		res = f_mkdir(path);
		/*make directory seccess or the dir is aready exist*/
		if((res==FR_OK) || (res == FR_EXIST))
			return 0 ;
		else return 1;//error
		
}


/****************************************************************
function		:delete the given file 
input				:file path
return			:success return 0,error return 1
author			:retryming@163.com 		#2015-9-5
****************************************************************/
unsigned char sdDeleteFile(const char *path)
{
		unsigned char res ;
			/*Mount a Logical Drive,notice unmount */
		if( f_mount(0,&fs) != FR_OK )
			return 1;	
		
		res = f_unlink(path);
		
		f_mount(0,NULL);
		
		if (res == FR_OK) //success
				return 0;
		
		else return 1; //error
		
}











	

