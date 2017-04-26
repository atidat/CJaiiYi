

#ifndef  __APP_ADS_H__
#define  __APP_ADS_H__


extern unsigned char CPTSensorString[60];

extern void GetQuintupleDigitValue(int *SF,int *PP,int *AX, int* Ay, int *CR);
extern void PackCPTSensorData(void);
extern void GetQuintupleBias(void);
extern void DebugGetAmpFactor(void);

#endif