#pragma once

//#include "DIO.h"
//#include "AIO.h"
#include "Definitions.h"
#include "Config.h"
struct IOCardStatus   // DI cards setting structure
{
	unsigned char DS;
	unsigned char CS;
	unsigned char CT;
	unsigned char Rack;
	unsigned char Slot;
};

char pluggedCards[N_MAX_RACKS][N_RACK_SLOTS];

// ----------------------- General functions ------------------------- //
int sysInit();
int Sync();
int CPU_Heartbeat();
void HWConfig(char rack, char slot, char pluggedCardType);
//int getCardsStatus(char nIOCards, char cards[][N_RACK_SLOTS], struct IOCardStatus cardsInfo[]);
int getAllCardsStatus(char nInputCards, char nOutputCards, char PluggedIOcards[][N_RACK_SLOTS], struct IOCardStatus cardsInfo[]);
int getCardsStatus(char PluggedIOcards[][N_RACK_SLOTS], struct IOCardStatus cardsInfo[]);
//void resolveCardsIssue(char NUM_IO_CARDS, struct IOCardStatus curIOStatus[], struct DI_Settings DI_Config, struct DO_Settings DO_Config);
//void fixCardsState(char nIOCards, struct IOCardStatus curIOStatus[], struct DI_Settings DI_Config, struct DO_Settings DO_Config, struct AI_Settings AI_Config, struct TI_Settings TI_Config, struct AO_Settings AO_Config);
unsigned char getNumOfPluggedCards();
void getDeviceUID(char* serial);
int getDeviceModel();
int getDeviceProcessor();
int getDeviceManufacturer();
void DefINData(void);
void DefOUTData(void);
UChar ReadINData(void);
void  SetOUTData(UChar IOData);