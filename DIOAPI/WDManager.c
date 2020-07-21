#include <fcntl.h>
#include <linux/watchdog.h>
#include <stdio.h>
#include "WDManager.h"
#include <sys/ioctl.h>


//#define		WDT_TIMEOUT		1  //1 sec
//#define		WDT_ENABLE		1
//extern int deviceHandle;
//extern int watchdogEnable;    	// WDT is Disabled by default
int initWD(char *WDdev)
{
	int devHandle;
	if ((devHandle = open(WDdev, O_RDWR | O_NOCTTY)) < 0) {
		printf("WDT> Error in opening watchdog device!! %s -> %d\n", WDdev, devHandle);
	} 
	return devHandle;
}

int setTimeoutWD(int devHandle, int timeOut_sec)
{
	if (ioctl(devHandle, WDIOC_SETTIMEOUT, &timeOut_sec) != -1)
	{
		//printf("\n WDT> The watchdog timeout has sat to %d seconds.\n\n", timeOut_sec);
		return 0;
	}

	printf("\n WDT> Error in setting watchdog timeout!!!\n\n");
	return -1;

	//int cutTimeout = 0;	
	//ioctl(devHandle, WDIOC_GETTIMEOUT, &cutTimeout);
}

int feedWD(int devHandle)
{
	return ioctl(devHandle, WDIOC_KEEPALIVE, 0);
}

//int disableWD(int devHandle)
//{
//	return write(devHandle, "V", 1);
//}