

#ifndef __FFOPERATE_H__
#define __FFOPERATE_H__

typedef struct
{
		unsigned char fileName[13];
}FILE_NAME_STRUCT;

extern FILE_NAME_STRUCT fileNameArray[256] ;

extern unsigned int initialCardCapacity,usedCardCapacity;//card capacity variable

extern unsigned char sdWriteDataInFile(unsigned char *pBuffer,unsigned int byteCount,const char *path);
extern unsigned char sdReadDataFromFile(const char *path,unsigned int *pCount);


extern int sdReadFileList(const char *path,unsigned char *fileNum);
extern unsigned char getCardCapacity(unsigned int *totalSize,unsigned int *usedSize);
unsigned char sdDeleteFile(const char *path);

extern unsigned char sdMakeDirectory(const char *path);

#endif 

