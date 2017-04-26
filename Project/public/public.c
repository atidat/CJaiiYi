#include "public.h"


void DelayNumUs(unsigned int nus)
{
		int i,j;
		for(i=0;i<nus;i++)
			for(j=0;j<12;j++)
					;	
}


void DelayNumMs(unsigned int nms)
{
		int i,j;
		for(i=0;i<nms;i++)
			for(j=0;j<2100;j++)
					;	
}


/*Quick sort*/
void QuickSort(int *data,int left,int right,int index)
{
	int i=left,j=right+1;
	int temp;
	int division=data[left];
	if(i<j)
	{
			do
		{
					do
					{
							i++;
					}while(division>=data[i]&&i<=right);
					do
					{
							j--;
					}while(division<data[j]&&j>left);
					if(i<j)
					{
							temp=data[i];
							data[i]=data[j];
							data[j]=temp;
					}
		}while(i<j);
		
		temp=data[left];
		data[left]=data[j];
		data[j]=temp;

		QuickSort(data,left,j-1,index);
		QuickSort(data,j+1,right,index);
	}
	
}


int getArrayAverage(int *pArray,int arrayLen,unsigned char discardDataCount)
{
		int idx,average,sum = 0;		
			
		for(idx = discardDataCount;idx<arrayLen-discardDataCount;idx++)
		{
				sum += pArray[idx];
		}		
		average = sum / (arrayLen-discardDataCount*2);
		
		return (average);
		
}