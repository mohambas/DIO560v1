
#include "BSWCAN.h"
//#include "DIO.h"
//#include "AIO.h"
#include "GeneralFunctions.h"
#include <wiringPi.h>
#include <stdio.h>
#include "WDManager.h"
unsigned char syncVal = 1;

int sysInit()
{
	unsigned char canData[2];

	IOCAN_Socket = -1;
	CPUCAN_Socket = -1;
	errCnt = -1;

	IOCAN_Socket = can0_init(); //can0_init(); //
	if (-1 == IOCAN_Socket) {
		printf("Error while CAN-0 initialization!!\n");
		return ERROR;
	}

	CPUCAN_Socket = can1_init(); // can1_init(); // 
	if (-1 == CPUCAN_Socket) {
		printf("Error while CAN-1 initialization!!\n");
		return ERROR;
	}

	// Set the plugged cards to zero
	char r, s;
	for (r = 0; r < N_MAX_RACKS; r++)
		for (s = 0; s < N_RACK_SLOTS; s++)
			pluggedCards[r][s] = 0;

	canData[0] = 0;
	if (sendFrame(IOCAN_Socket, 0x23, 1, canData))
	{
		printf("Error while system initialization!!\n");
		return ERROR;
	}
		
	delayMicroseconds(8);
	sendFrame(IOCAN_Socket, 0x23, 1, canData);
	delayMicroseconds(8);
	sendFrame(IOCAN_Socket, 0x23, 1, canData);
	delayMicroseconds(8);
	sendFrame(IOCAN_Socket, 0x23, 1, canData);
	delayMicroseconds(8);
	sendFrame(IOCAN_Socket, 0x23, 1, canData);
	
	delayMicroseconds(30);
	
	canData[0] = 1;
	sendFrame(IOCAN_Socket, 0x23, 1, canData);
	delayMicroseconds(8);
	sendFrame(IOCAN_Socket, 0x23, 1, canData);
	delayMicroseconds(8);
	sendFrame(IOCAN_Socket, 0x23, 1, canData);
	delayMicroseconds(8);
	sendFrame(IOCAN_Socket, 0x23, 1, canData);
	delayMicroseconds(8);
	sendFrame(IOCAN_Socket, 0x23, 1, canData);
	
	delayMicroseconds(800);

	return SUCCESS;
}

void HWConfig(char rack, char slot, char pluggedCardType)  // Detemine the card connection configuration
{
	pluggedCards[rack][slot] = pluggedCardType;
}

int Sync()  // should has void arg
{
	unsigned char cData[2];

	cData[0] = syncVal;				// sync value should be toggled in each cycle
	sendFrame(IOCAN_Socket, 0x21, 1, cData);		// sync

	delayMicroseconds (1);
	//canID = ((0 << 1) | (0 << 7) | (1 << 5));   // =0x025
	//sendFrame(CPUCAN_Socket, 0x25, 1, cData);		// Heartbeat to the redundant MPU

	// Sync pulse
	if (syncVal == 0x01)
		syncVal = 0x00;
	else syncVal = 0x01;
	

	return SUCCESS;
}

int getAllCardsStatus(char nInputCards, char nOutputCards, char PluggedIOcards[][N_RACK_SLOTS], struct IOCardStatus cardsInfo[])
{
	int canID;
	struct can_frame rcvStatFrame = { 0 };
	char CT = -1, cmd = -1, slot = -1, rack = -1, i;

	unsigned char cData[2];

	//printf("---------------------------------\n");
	if (nInputCards > 0)
		sendFrame(IOCAN_Socket, 0x08, 0, cData);   	// set-get state from input cards

	for (i = 0; i < nInputCards; i++)
	{
		receiveFrame(IOCAN_Socket, &rcvStatFrame);	// Wait for all the connected card(s) status/ACK
		//printRcvFrame(rcvStatFrame);
		canID = rcvStatFrame.can_id;
		cmd = (canID >> 1) & 0x000F;

		if (cmd == SET_GET_STATE)
		{
			rack = (canID >> 7) & 0x000F;
			slot = (canID >> 5) & 0x0003;
			
			//canDLC = rcvCANFrame.can_dlc;

			CT = rcvStatFrame.data[0] & 0x0F;
			if (CT != PluggedIOcards[rack][slot])
			{
				printf("Rack %d, Slot %d, Input Card(s) Configurations is NOT Valid!!!\n", rack, slot);
				return ERROR;
			}
			
			cardsInfo[i].DS = (rcvStatFrame.data[0] >> 7) & 0x01;
			cardsInfo[i].CS = (rcvStatFrame.data[0] >> 4) & 0x07;
			cardsInfo[i].CT = CT;		//rcvCANFrame.data[0] & 0x0F;
			cardsInfo[i].Rack = rack;	//(canID >> 7) & 0x000F;
			cardsInfo[i].Slot = slot;	//(canID >> 5) & 0x0003;
		}
	}

	if (nOutputCards > 0)
		sendFrame(IOCAN_Socket, 0x09, 0, cData);   	// set-get state from input cards
	for (i = nInputCards; i < (nInputCards + nOutputCards); i++)
	{
		receiveFrame(IOCAN_Socket, &rcvStatFrame);	// Wait for all the connected card(s) status/ACK
		//printRcvFrame(rcvStatFrame);
		canID = rcvStatFrame.can_id;
		cmd = (canID >> 1) & 0x000F;

		if (cmd == SET_GET_STATE)
		{
			rack = (canID >> 7) & 0x000F;
			slot = (canID >> 5) & 0x0003;
			
			//canDLC = rcvCANFrame.can_dlc;

			CT = rcvStatFrame.data[0] & 0x0F;
			if (CT != PluggedIOcards[rack][slot])
			{
				printf("Rack %d, Slot %d, Output Card(s) Configurations is NOT Valid!!!\n", rack, slot);
				return ERROR;
			}
			
			cardsInfo[i].DS = (rcvStatFrame.data[0] >> 7) & 0x01;
			cardsInfo[i].CS = (rcvStatFrame.data[0] >> 4) & 0x07;
			cardsInfo[i].CT = CT;		//rcvCANFrame.data[0] & 0x0F;
			cardsInfo[i].Rack = rack;	//(canID >> 7) & 0x000F;
			cardsInfo[i].Slot = slot;	//(canID >> 5) & 0x0003;

			//printf("CT: %s , CS: %d, DS: %d\n", cardsType[CT], cardsInfo[i].CS, DS);
		}
	}

	return SUCCESS;
}

// Get the status by the card address
int getCardsStatus(char PluggedIOcards[][N_RACK_SLOTS], struct IOCardStatus cardsInfo[])
{
	int canID;
	struct can_frame rcvStatFrame = { 0 };
	char CT, k = 0;
	unsigned char r, s;
	unsigned char cData[2];

	for (r = 0; r < N_MAX_RACKS; r++)
		for (s = 0; s < N_RACK_SLOTS; s++)
		{
			if (PluggedIOcards[r][s] == DI530 || PluggedIOcards[r][s] == AI621 || PluggedIOcards[r][s] == AI623 || PluggedIOcards[r][s] == AI622)
			{
				canID = ((SET_GET_STATE << 1) | (r << 7) | (s << 5)); // get state from input cards @ specified locations
				sendFrame(IOCAN_Socket, canID, 0, cData);   	// set-get state from input cards
				if (receiveFrame(IOCAN_Socket, &rcvStatFrame) == ERROR)
				{
					printf("No Response from card @ Rack: %d, Slot: %d !!! \r\n", r, s);
					//return ERROR;
				}

				//printRcvFrame(rcvStatFrame);
				CT = rcvStatFrame.data[0] & 0x0F;
				
				if (CT != PluggedIOcards[r][s])
				{
					printf("cardType = %d, should be: %d \n", CT, PluggedIOcards[r][s]);

					printf("Rack %d, Slot %d, Input Card Type is Wrong !!!\r\n", r, s);
					return ERROR;
				}
			
				cardsInfo[k].DS = (rcvStatFrame.data[0] >> 7) & 0x01;
				cardsInfo[k].CS = (rcvStatFrame.data[0] >> 4) & 0x07;
				cardsInfo[k].CT = CT;
				cardsInfo[k].Rack = r;		//(canID >> 7) & 0x000F;
				cardsInfo[k].Slot = s;		//(canID >> 5) & 0x0003;
				k++;
			}
			else if (PluggedIOcards[r][s] == DO571 || PluggedIOcards[r][s] == DO580 || PluggedIOcards[r][s] == AO673 || PluggedIOcards[r][s] == AO672)
			{
				canID = ((SET_GET_STATE << 1) | (r << 7) | (s << 5) | 0x01); // get state from input cards @ specified locations
				sendFrame(IOCAN_Socket, canID, 0, cData);   	// set-get state from input cards
				if (receiveFrame(IOCAN_Socket, &rcvStatFrame) == ERROR)
				{
					printf("No Response from the card @ Rack: %d, Slot: %d !!! \r\n", r, s);
					return ERROR;
				}

				CT = rcvStatFrame.data[0] & 0x0F;
				if (CT != PluggedIOcards[r][s])
				{
					//printf("cardType = %d, should be: %d \n", CT, PluggedIOcards[r][s]);

					printf("Rack %d, Slot %d, Output Card Type is Wrong !!!\r\n", r, s);
					return ERROR;
				}
			
				cardsInfo[k].DS = (rcvStatFrame.data[0] >> 7) & 0x01;
				cardsInfo[k].CS = (rcvStatFrame.data[0] >> 4) & 0x07;
				cardsInfo[k].CT = CT;
				cardsInfo[k].Rack = r;		//(canID >> 7) & 0x000F;
				cardsInfo[k].Slot = s;		//(canID >> 5) & 0x0003;
				k++;
			}
		}

	return SUCCESS;
}

//void fixCardsState(char NUM_IO_CARDS, struct IOCardStatus curIOStatus[], struct DI_Settings DI_Config, struct DO_Settings DO_Config, struct AI_Settings AI_Config, struct TI_Settings TI_Config, struct AO_Settings AO_Config) // interpret the cards status and resolve probable issues.
//{
//	unsigned char i = 0;
//	for (i = 0; i < NUM_IO_CARDS; i++)
//	{
//		if (curIOStatus[i].CS != MAIN)
//		{
//			if (curIOStatus[i].CT == DI530)  // DI cards
//			{
//				DI_SetSettings(curIOStatus[i].Rack, curIOStatus[i].Slot, DI_Config);
//				DI_SetState(curIOStatus[i].Rack, curIOStatus[i].Slot, MAIN);
//			}
//			else if (curIOStatus[i].CT == DO580 || curIOStatus[i].CT == DO571) // DO cards
//			{
//				DO_SetSettings(curIOStatus[i].Rack, curIOStatus[i].Slot, DO_Config);
//				DO_SetState(curIOStatus[i].Rack, curIOStatus[i].Slot, MAIN);
//			}
//			else if (curIOStatus[i].CT == AO672 || curIOStatus[i].CT == AO673) // AO cards
//			{
//				AO_SetSettings(curIOStatus[i].Rack, curIOStatus[i].Slot, AO_Config);
//				AO_SetState(curIOStatus[i].Rack, curIOStatus[i].Slot, MAIN);
//			}
//			else if (curIOStatus[i].CT == AI622 || curIOStatus[i].CT == AI623) // AI cards
//			{
//				AI_SetSettings(curIOStatus[i].Rack, curIOStatus[i].Slot, AI_Config);
//				AI_SetState(curIOStatus[i].Rack, curIOStatus[i].Slot, MAIN);
//			}
//			else if (curIOStatus[i].CT == AI621) // TI cards
//			{
//				TI_SetSettings(curIOStatus[i].Rack, curIOStatus[i].Slot, TI_Config);
//				TI_SetState(curIOStatus[i].Rack, curIOStatus[i].Slot, MAIN);
//			}
//		}
//	}
//}

unsigned char getNumOfPluggedCards()
{
	unsigned char nCards = 0;
	char r, s;

	for (r = 0; r < N_MAX_RACKS; r++)
	{
		for (s = 0; s < N_RACK_SLOTS; s++)
		{
			if (pluggedCards[r][s] != 0)
				nCards++;
		}
	}
	return nCards;
}

void getDeviceUID(char* serial)
{
	FILE *file;

//	file = popen("sed -n '/^Hardware/{s/.* //;p}' /proc/cpuinfo", "r");
//	fscanf(file, "%10s", hardware);
//
//	file = popen("sed -n '/^Revision/{s/.* //;p}' /proc/cpuinfo", "r");
//	fscanf(file, "%10s", revision);

	file = popen("sed -n '/^Serial/{s/.* //;p}' /proc/cpuinfo", "r");
	fscanf(file, "%20s", serial);

	pclose(file);
}

int getDeviceModel()
{
	char revision[8];

	FILE *file = popen("sed -n '/^Revision/{s/.* //;p}' /proc/cpuinfo", "r");
	fscanf(file, "%8s", revision);
	pclose(file);
	int rev = (int)strtol(revision, NULL, 16);
	return ((rev >> 4) & 0xFF);
}

int getDeviceProcessor()
{
	char revision[8];

	FILE *file = popen("sed -n '/^Revision/{s/.* //;p}' /proc/cpuinfo", "r");
	fscanf(file, "%8s", revision);
	pclose(file);
	int rev = (int)strtol(revision, NULL, 16);
	return ((rev >> 12) & 0x0F);
}

int getDeviceManufacturer()
{
	char revision[8];

	FILE *file = popen("sed -n '/^Revision/{s/.* //;p}' /proc/cpuinfo", "r");
	fscanf(file, "%8s", revision);
	pclose(file);
	int rev = (int)strtol(revision, NULL, 16);
	return ((rev >> 16) & 0x0F);
}

void DefINData(void)
{
	pinMode(D0, INPUT);
	pinMode(D1, INPUT);
	pinMode(D2, INPUT);
	pinMode(D3, INPUT);
	pinMode(D4, INPUT);
	pinMode(D5, INPUT);
	pinMode(D6, INPUT);
	pinMode(D7, INPUT);
	pullUpDnControl(D0, PUD_DOWN);
	pullUpDnControl(D1, PUD_DOWN);
	pullUpDnControl(D2, PUD_DOWN);
	pullUpDnControl(D3, PUD_DOWN);
	pullUpDnControl(D4, PUD_DOWN);
	pullUpDnControl(D5, PUD_DOWN);
	pullUpDnControl(D6, PUD_DOWN);
	pullUpDnControl(D7, PUD_DOWN);
}

void DefOUTData(void)
{
	pinMode(D0, OUTPUT);
	pinMode(D1, OUTPUT);
	pinMode(D2, OUTPUT);
	pinMode(D3, OUTPUT);
	pinMode(D4, OUTPUT);
	pinMode(D5, OUTPUT);
	pinMode(D6, OUTPUT);
	pinMode(D7, OUTPUT);
}

UChar ReadINData(void)
{
	UChar IOData;
//	IOData = digitalRead(D0)&0x01;
	UChar p0, p1, p2, p3, p4, p5, p6, p7;
	p0	=	digitalRead(D0);
	p1	=	digitalRead(D1);
	p2	=	digitalRead(D2);
	p3	=	digitalRead(D3);
	p4	=	digitalRead(D4);
	p5	=	digitalRead(D5);
	p6	=	digitalRead(D6);
	p7	=	digitalRead(D7);
	IOData	=	(p0) | (p1<<1) | (p2<<2) | (p3<<3) | (p4<<4) | (p5<<5) | (p6<<6) | (p7<<7);
	p6	=	0;
	return IOData;
}

void  SetOUTData(UChar IOData)
{
	IOData = ~IOData;
	UChar p0, p1, p2, p3, p4, p5, p6, p7;
	p0	=	IOData & HIGH;
	p1	=	(IOData>>1) & HIGH;
	p2	=	(IOData>>2) & HIGH;
	p3	=	(IOData>>3) & HIGH;
	p4	=	(IOData>>4) & HIGH;
	p5	=	(IOData>>5) & HIGH;
	p6	=	(IOData>>6) & HIGH;
	p7	=	(IOData>>7) & HIGH;
	
	digitalWrite(D0, p0);
	digitalWrite(D1, p1);
	digitalWrite(D2, p2);
	digitalWrite(D3, p3);
	digitalWrite(D4, p4);
	digitalWrite(D5, p5);
	digitalWrite(D6, p6);
	digitalWrite(D7, p7);
	
}