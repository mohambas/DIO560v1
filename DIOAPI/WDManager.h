#ifndef	__WDManager_H__
				  
#define __WDManager_H__// !WDManager.h


//int deviceHandle = 0;
#define		WDT_TIMEOUT		1  //10 sec
#define		WDT_ENABLE		1
int initWD(char *WDdev);
int setTimeoutWD(int devHandle, int timeOut_sec);
int feedWD(int devHandle);
//int disableWD(int devHandle);
#endif