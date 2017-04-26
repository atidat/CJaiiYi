#include "headers.h"


//#define DebugBias
//#define DebugFactor
#define NormalWork
//#define DebugUsart1

void DeviceInit(void)
{
	
		//NVIC_Configuration();
	
		//RTCInit();
		//TimerInit();
	
		UARTInit();
		//analybaute();
	
		//LedInit();
		
		ADS1256_Init();
			
		/*sdcard must be configed success*/
		////while(SDCardInitialize());
		sdInitStatus = SDCardInitialize();
			
}


int main()
{
   	DeviceInit();

		ADS1256_readStatus(&adcStatus);

	while(1){
		
#ifdef DebugUsart1
			printf("rock&roll\n");
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


}