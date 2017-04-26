#include "headers.h"


//#define DebugBias
//#define DebugFactor
#define NormalWork
//#define DebugUsart1

void DeviceInit(void)
{
		/*If use SW mode ,close JTAG debug tool*/
		//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		//RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
		//GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
		//NVIC_Configuration();
	
		//RTCInit();
		//TimerInit();
	
		UARTInit();
		//analybaute();
	
		//LedInit();
		
		ADS1256_Init();
			
		/*sdcard must be configed succesee*/
		//while(SDCardInitialize());
		//sdInitStatus = SDCardInitialize();
			
}


int main()
{
   	DeviceInit();
		//UARTInit();
		//ADS1256_readStatus(&adcStatus);

	while(1){
		
#ifdef DebugUsart1
															printf("rock&roll\n");
		//USART_SendData(USART2, 0Xf00f);
		DelayNumMs(1000);
#endif		

#ifdef DebugBias
		GetQuintupleBias();
#endif

#ifdef DebugFactor
	  DebugGetAmpFactor();
#endif


#ifdef NormalWork
		
		PackCPTSensorData();
		DelayNumUs(3000);
		printf("%s", CPTSensorString);
		
#endif


	}

	return 0;	
	//SPI_Initialize();

}