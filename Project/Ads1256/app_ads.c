#include  "headers.h"

/**********************************************
  SF		: sleeve friction
  PP		: pore	 pressure
  AX		: angle	 x
  AY		: angle  y
  CR		: cone   resistance
**********************************************/
#define CarliSampleCount 20
#define DISCARD_SEED_COUNT 2
#define SENSOR_SCALE 10000   // magFiled/Voltage(1V)

int SF_Bias, PP_Bias, AX_Bias, AY_Bias, CR_Bias;

void GetQuintupleBias(void)
{
		int idx = 0; 
		int SF_DigitArray[CarliSampleCount], PP_DigitArray[CarliSampleCount],
			  AX_DigitArray[CarliSampleCount], AY_DigitArray[CarliSampleCount],
		  	CR_DigitArray[CarliSampleCount];

		int SF, PP, AX, AY, CR ;

		unsigned char SF_TmpStr[11], PP_TmpStr[11], AX_TmpStr[11],
					        AY_TmpStr[11], CR_TmpStr[11];	
				
		memset(SF_TmpStr,'\0',sizeof(SF_TmpStr));
		memset(PP_TmpStr,'\0',sizeof(PP_TmpStr));
		memset(AX_TmpStr,'\0',sizeof(AX_TmpStr));
		memset(AY_TmpStr,'\0',sizeof(AY_TmpStr));
		memset(CR_TmpStr,'\0',sizeof(CR_TmpStr));
				
		/*Get CarliSampleCount sample seed*/
		for(idx=0; idx < CarliSampleCount; idx++)
		{
				GetQuintupleDigitValue(&SF_DigitArray[idx], &PP_DigitArray[idx],
				&AX_DigitArray[idx], &AY_DigitArray[idx], &CR_DigitArray[idx]);
		}	
				

		/* sorting */
		/* pick	MaxValue and MinValue */
		QuickSort(SF_DigitArray,0,CarliSampleCount-1,CarliSampleCount);
		QuickSort(PP_DigitArray,0,CarliSampleCount-1,CarliSampleCount);
		QuickSort(AX_DigitArray,0,CarliSampleCount-1,CarliSampleCount);
		QuickSort(AY_DigitArray,0,CarliSampleCount-1,CarliSampleCount);
		QuickSort(CR_DigitArray,0,CarliSampleCount-1,CarliSampleCount);
			
				
		/* calculate average */
		/* delete MaxValue and MinValue */
		SF = getArrayAverage(SF_DigitArray,CarliSampleCount,DISCARD_SEED_COUNT);
		PP = getArrayAverage(PP_DigitArray,CarliSampleCount,DISCARD_SEED_COUNT);
		AX = getArrayAverage(AX_DigitArray,CarliSampleCount,DISCARD_SEED_COUNT);
		AY = getArrayAverage(AY_DigitArray,CarliSampleCount,DISCARD_SEED_COUNT);
		CR = getArrayAverage(CR_DigitArray,CarliSampleCount,DISCARD_SEED_COUNT);
		
				
		sprintf(SF_TmpStr,"%5.3f",(double)SF);
		strcat(SF_TmpStr,"\t");
		printf("%s", SF_TmpStr);

		sprintf(PP_TmpStr,"%5.3f",(double)PP);
		strcat(PP_TmpStr,"\t");
		printf("%s", PP_TmpStr);

		sprintf(AX_TmpStr,"%5.3f",(double)AX);
		strcat(AX_TmpStr,"\t");
		printf("%s", AX_TmpStr);

		sprintf(AY_TmpStr,"%5.3f",(double)AY);
		strcat(AY_TmpStr,"\t");
		printf("%s", AY_TmpStr);

		sprintf(CR_TmpStr,"%5.3f",(double)CR);
		strcat(CR_TmpStr,"\t");
		printf("%s\n", CR_TmpStr);
		
//		if(minFlag == 1)
//		{
//				ADS1256_SetCalibrationCommand();//2015-9-25 updated
//				minFlag = 0;
//		}
			
}


void DebugGetAmpFactor(void)
{
		int SF_DigitNum, PP_DigitNum, AX_DigitNum,
			AY_DigitNum, CR_DigitNum;

		int SF_RemoveBiasResult, PP_RemoveBiasResult, 
		    AX_RemoveBiasResult, AY_RemoveBiasResult, CR_RemoveBiasResult;

		double SF_AnalogNum, PP_AnalogNum, 
			   AX_AnalogNum, AY_AnalogNum, CR_AnalogNum;

		double SF_ActualValue, PP_ActualValue, 
		       AX_ActualValue, AY_ActualValue, CR_ActualValue;

		unsigned char SF_String[30], PP_String[30], 
		  	          AX_String[30], AY_String[30], CR_String[30];
	
		/*Set input voltage as 1V,get the digital number */
		GetQuintupleDigitValue(&SF_DigitNum, &PP_DigitNum,
							&AX_DigitNum, &AY_DigitNum, &CR_DigitNum);
		
		/*remove the bias from digital number*/
		SF_RemoveBiasResult = SF_DigitNum - SF_Bias ;
		PP_RemoveBiasResult = PP_DigitNum - PP_Bias ;
		AX_RemoveBiasResult = AX_DigitNum - AX_Bias ;
		AY_RemoveBiasResult = AY_DigitNum - AY_Bias ;
		CR_RemoveBiasResult = CR_DigitNum - CR_Bias ;
		
			
		SF_AnalogNum = SF_RemoveBiasResult * ADS_LSB_WEIGHT ;
		PP_AnalogNum = PP_RemoveBiasResult * ADS_LSB_WEIGHT ;
		AX_AnalogNum = AX_RemoveBiasResult * ADS_LSB_WEIGHT ;
		AY_AnalogNum = AY_RemoveBiasResult * ADS_LSB_WEIGHT ;
		CR_AnalogNum = CR_RemoveBiasResult * ADS_LSB_WEIGHT ;
		
			
		SF_ActualValue = SF_AnalogNum * SENSOR_SCALE ; ////////
		PP_ActualValue = PP_AnalogNum * SENSOR_SCALE ; ////////
		AX_ActualValue = AX_AnalogNum * SENSOR_SCALE ; ////////
		AY_ActualValue = AY_AnalogNum * SENSOR_SCALE ; ////////
		CR_ActualValue = CR_AnalogNum * SENSOR_SCALE ; ////////
		
				
		memset(SF_String,'\0',sizeof(SF_String));
		memset(PP_String,'\0',sizeof(PP_String));
		memset(AX_String,'\0',sizeof(AX_String));
		memset(AY_String,'\0',sizeof(AY_String));
		memset(CR_String,'\0',sizeof(CR_String));
		
		sprintf(SF_String,"%5.3f",(double)SF_ActualValue);
		strcat(SF_String,"\t");
		printf("%s", SF_String);

		sprintf(PP_String,"%5.3f",(double)PP_ActualValue);
		strcat(PP_String,"\t");
		printf("%s", PP_String);
		
		sprintf(AX_String,"%5.3f",(double)AX_ActualValue);
		strcat(AX_String,"\t");
		printf("%s", AX_String);
		
		sprintf(AY_String,"%5.3f",(double)AY_ActualValue);
		strcat(AY_String,"\t");
		printf("%s", AY_String);
		
		sprintf(CR_String,"%5.3f",(double)CR_ActualValue);
		strcat(CR_String,"\t");
		printf("%s", CR_String);
				
}

void GetQuintupleDigitValue(int *SF,int *PP,int *AX, int* AY, int *CR)
{
		*SF = ADS1256_ReadOneChannelValue(1);
		*PP = ADS1256_ReadOneChannelValue(2);
		*AX = ADS1256_ReadOneChannelValue(3);
		*AY = ADS1256_ReadOneChannelValue(4);
		*CR = ADS1256_ReadOneChannelValue(5);				
}	

unsigned char CPTSensorString[60]; /*CPT SENSOR data interface */

void PackCPTSensorData(void)
{
		int    SF_DigitNum, PP_DigitNum, 
					 AX_DigitNum, AY_DigitNum, 
			     CR_DigitNum;

		int    SF_RemoveBiasResult, PP_RemoveBiasResult,
		       AX_RemoveBiasResult, AY_RemoveBiasResult,
			     CR_RemoveBiasResult;

		double SF_AnalogNum, PP_AnalogNum,
			     AX_AnalogNum, AY_AnalogNum,
			     CR_AnalogNum;

		double SF_ActualValue, PP_ActualValue,
			     AX_ActualValue, AY_ActualValue,
			     CR_ActualValue;

		unsigned char tmpStr[10];
	
		memset(CPTSensorString,'\0',sizeof(CPTSensorString));
		memset(tmpStr,'\0',sizeof(tmpStr));
		
		/*get the digital number */
		GetQuintupleDigitValue(&SF_DigitNum, &PP_DigitNum,
				 &AX_DigitNum, &AY_DigitNum, &CR_DigitNum);
		
		/*remove the bias from digital number*/
		SF_RemoveBiasResult = SF_DigitNum - SF_Bias ;
		PP_RemoveBiasResult = PP_DigitNum - PP_Bias ;
		AX_RemoveBiasResult = AX_DigitNum - AX_Bias ;
 		AY_RemoveBiasResult = AY_DigitNum - AY_Bias ;
		CR_RemoveBiasResult = CR_DigitNum - CR_Bias ;


		SF_AnalogNum = SF_RemoveBiasResult * ADS_LSB_WEIGHT	;
		PP_AnalogNum = PP_RemoveBiasResult * ADS_LSB_WEIGHT	;

		//SF_AnalogNum = SF_RemoveBiasResult * ADS_LSB_WEIGHT ;
		//PP_AnalogNum = PP_RemoveBiasResult * ADS_LSB_WEIGHT ;
		AX_AnalogNum = AX_RemoveBiasResult * ADS_LSB_WEIGHT ;
 		AY_AnalogNum = AY_RemoveBiasResult * ADS_LSB_WEIGHT ;
		CR_AnalogNum = CR_RemoveBiasResult * ADS_LSB_WEIGHT ;
/****** y = kx + b; ******/
/****** y : physical  value ******/
/****** x : eletrical value ******/
		//SF_ActualValue = K1 * SF_AnalogNum + B1 ;
		//PP_ActualValue = K2 * PP_AnalogNum + B2 ;
		//AX_ActualValue = K3 * AX_AnalogNum + B3 ;
		//AY_ActualValue = K4 * AY_AnalogNum + B4 ;
		//CR_ActualValue = K5 * CR_AnalogNum + B5 ;		

		sprintf(tmpStr,"%5.3f",(double)SF_AnalogNum);
		strcat(tmpStr,",");
		memcpy(CPTSensorString,tmpStr,strlen(tmpStr));
		
		memset(tmpStr,'\0',sizeof(tmpStr));
		sprintf(tmpStr,"%5.3f",(double)PP_AnalogNum);
		strcat(tmpStr,",");
		strcat(CPTSensorString,tmpStr);
			
		memset(tmpStr,'\0',sizeof(tmpStr));
		sprintf(tmpStr,"%5.3f",(double)AX_AnalogNum);
		strcat(tmpStr,",");
		strcat(CPTSensorString,tmpStr);

		memset(tmpStr,'\0',sizeof(tmpStr));
		sprintf(tmpStr,"%5.3f",(double)AY_AnalogNum);
		strcat(tmpStr,",");
		strcat(CPTSensorString,tmpStr);

		memset(tmpStr,'\0',sizeof(tmpStr));
		sprintf(tmpStr,"%5.3f",(double)CR_AnalogNum);
		strcat(tmpStr,"\r\n");
		strcat(CPTSensorString,tmpStr);
		
		//UartSendString(COM2,magneticString) ;
		
		
}





									     