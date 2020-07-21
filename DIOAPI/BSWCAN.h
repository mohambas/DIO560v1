#ifndef __BSWCAN_H_
#define __BSWCAN_H_				  

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <net/if.h>
#include <sys/ioctl.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/error.h>
#include "lib.h"

int IOCAN_Socket;	//can0
int CPUCAN_Socket;	//can1
int errCnt;

int sendFrame(int CANSocket, int canID, unsigned char dLen, unsigned char canData[]);
int receiveFrame(int CANSocket, struct can_frame *cf);
int can0_init();
int can1_init();
void printRcvFrame(struct can_frame rcvFrame);

#endif // __BSWCAN_H_