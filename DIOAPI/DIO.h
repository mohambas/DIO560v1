#pragma once

#include "Definitions.h"

struct DO_Settings   // DO cards setting structure
{
	unsigned char OHLimit;// = 65;
	unsigned char OHHystersis;// = 5;
	unsigned char RedundantBPAdd;// = 0;
	unsigned char activeChannels[4];// {0xFF, 0xFF, 0xFF, 0xFF}
	unsigned char safeValueActive[4];// {0, 0 ,0 ,0}
	unsigned char safeValues[4];// {0, 0, 0, 0}
};

struct DI_Settings   // DI cards setting structure
{
	unsigned char OHLimit;// = 65;
	unsigned char OHHystersis;// = 5;
	unsigned char RedundantBPAdd;// = 0;
	unsigned char FilteringWin[4];// {10, 10, 10, 10}
};

// ------------------------ Digital Output --------------------------- //
int DO_SetSettings(char rack, char slot, struct DO_Settings newSettings); // digital Output cards settings
int DO_SetAllCardsSettings(char PluggedIOcards[][N_RACK_SLOTS], struct DO_Settings Settings);

void DO_SetDefaultSettings(struct DO_Settings *);
int DO_SetState(char rack, char slot, int newState);
int DO_SetValues(char rack, char slot, unsigned char Values[]);

// ------------------------ Digital Input ------------------------------ //
int DI_SetSettings(char rack, char slot, struct DI_Settings newSettings); // digital Input cards settings
int DI_SetAllCardsSettings(char PluggedIOcards[][N_RACK_SLOTS], struct DI_Settings Settings);

void DI_SetDefaultSettings(struct DI_Settings *);
int DI_SetState(char rack, char slot, int newState);
int DI_GetValues(char rack, char slot, unsigned char Values[]);
