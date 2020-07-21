
#include "BSWCAN.h"
#include "DIO.h"

///-------------------------------------------------------------------------///
/// --------------------------- DO SECTION -------------------------------- ///
int DO_SetSettings(char rack, char slot, struct DO_Settings newSettings)
{
	//unsigned char cData[9] = { 0 };
	struct can_frame rcvCANFrame = { 0 };
	unsigned char canData[10] = { 0 };

	int canID = ((SET_SETTING << 1) | (slot << 5) | (rack << 7)) | 0x01;

	canData[0] = 15;  // Settings size
	canData[1] = newSettings.OHLimit;
	canData[2] = newSettings.OHHystersis;
	canData[3] = newSettings.RedundantBPAdd;

	canData[4] = newSettings.activeChannels[0];
	canData[5] = newSettings.activeChannels[1];
	canData[6] = newSettings.activeChannels[2];
	canData[7] = newSettings.activeChannels[3];

	sendFrame(IOCAN_Socket, canID, 8, canData);	// set Settings (first section)
	receiveFrame(IOCAN_Socket, &rcvCANFrame);		// wait for the DO card ACK

	canData[0] = newSettings.safeValueActive[0];
	canData[1] = newSettings.safeValueActive[1];
	canData[2] = newSettings.safeValueActive[2];
	canData[3] = newSettings.safeValueActive[3];

	canData[4] = newSettings.safeValues[0];
	canData[5] = newSettings.safeValues[1];
	canData[6] = newSettings.safeValues[2];
	canData[7] = newSettings.safeValues[3];

	//delay(1);
	sendFrame(IOCAN_Socket, canID, 8, canData);	// set Settings (second section)
	receiveFrame(IOCAN_Socket, &rcvCANFrame);		// wait for the DO card ACK

	return SUCCESS;
}

// Set the same settings (the default one) to the all connected DO cards
int DO_SetAllCardsSettings(char PluggedIOcards[][N_RACK_SLOTS], struct DO_Settings Settings)
{
	char r, c;
	for (r = 0; r < N_MAX_RACKS; r++)
	{
		for (c = 0; c < N_RACK_SLOTS; c++)
			if (PluggedIOcards[r][c] == DO571 || PluggedIOcards[r][c] == DO580)  // or all types of DO cards.
			{
				// DO settings
				DO_SetSettings(r, c, Settings);
				DO_SetState(r, c, MAIN);  // set the card state to MAIN
			}
	}
	return SUCCESS;
}

void DO_SetDefaultSettings(struct DO_Settings *defaultSettings)
{
	defaultSettings->OHLimit = 65;
	defaultSettings->OHHystersis = 5;
	defaultSettings->RedundantBPAdd = 0;

	defaultSettings->activeChannels[0] = 0xFF;
	defaultSettings->activeChannels[1] = 0xFF;
	defaultSettings->activeChannels[2] = 0x00;
	defaultSettings->activeChannels[3] = 0x00;

	defaultSettings->safeValueActive[0] = 0x00;
	defaultSettings->safeValueActive[1] = 0x00;
	defaultSettings->safeValueActive[2] = 0x00;
	defaultSettings->safeValueActive[3] = 0x00;

	defaultSettings->safeValues[0] = 0x00;
	defaultSettings->safeValues[1] = 0x00;
	defaultSettings->safeValues[2] = 0x00;
	defaultSettings->safeValues[3] = 0x00;
}

int DO_SetState(char rack, char slot, int newState)
{
	unsigned char canData[4] = { 0 };
	int canID = ((SET_GET_STATE << 1) | (slot << 5) | (rack << 7)) | 0x01;

	//delay(1);
	canData[0] = newState; canData[1] = 0x00;
	sendFrame(IOCAN_Socket, canID, 2, canData);		// set output card state

	return SUCCESS;
}

int DO_SetValues(char rack, char slot, unsigned char Values[])
{
	int canID;
	canID = ((SET_GET_DATA << 1) | (rack << 7) | (slot << 5) | 0x01) ; // set data for output card in a specified location (rack and slot)
	sendFrame(IOCAN_Socket, canID, 4, Values);  // 32 channels (bits) -> 4 bytes 
	return SUCCESS;
}

///-------------------------------------------------------------------------------------------///
/// ------------------------------------ DI SECTION ----------------------------------------- ///
int DI_SetSettings(char rack, char slot, struct DI_Settings newSettings) // digital Input cards settings
{
	unsigned char canData[10] = { 0 };
	struct can_frame rcvCANFrame = { 0 };
	int canID;
	canID = ((SET_SETTING << 1) | (rack << 7) | (slot << 5));

	canData[0] = 7;  // Settings size
	canData[1] = newSettings.OHLimit;
	canData[2] = newSettings.OHHystersis;
	canData[3] = newSettings.RedundantBPAdd;

	canData[4] = newSettings.FilteringWin[0];
	canData[5] = newSettings.FilteringWin[1];
	canData[6] = newSettings.FilteringWin[2];
	canData[7] = newSettings.FilteringWin[3];

	sendFrame(IOCAN_Socket, canID, 8, canData);	// set Settings (first section)

	receiveFrame(IOCAN_Socket, &rcvCANFrame);	// wait for the DI card ACK
	//printRcvFrame(rcvCANFrame);

	receiveFrame(IOCAN_Socket, &rcvCANFrame);	// wait for the DI card ACK
	//printRcvFrame(rcvCANFrame);

	return SUCCESS;
}

// set the same settings (the default one) to the all connected DI cards
int DI_SetAllCardsSettings(char PluggedIOcards[][N_RACK_SLOTS], struct DI_Settings Settings)
{
	char r, c;
	for (r = 0; r < N_MAX_RACKS; r++)
	{
		for (c = 0; c < N_RACK_SLOTS; c++)
			if (PluggedIOcards[r][c] == DI530)  // or all types of DI cards. At the moment there is only one type of DI card is available
			{
				// DI settings
				DI_SetSettings(r, c, Settings);
				DI_SetState(r, c, MAIN);  // set the card state to MAIN
			}
	}
	return SUCCESS;
}

void DI_SetDefaultSettings(struct DI_Settings *defaultSettings)
{
	defaultSettings->OHLimit = 65;
	defaultSettings->OHHystersis = 5;
	defaultSettings->RedundantBPAdd = 0;
	defaultSettings->FilteringWin[0] = 10;
	defaultSettings->FilteringWin[1] = 10;
	defaultSettings->FilteringWin[2] = 10;
	defaultSettings->FilteringWin[3] = 10;
}

int DI_SetState(char rack, char slot, int newState)
{
	unsigned char canData[4] = { 0 };
	int canID = ((SET_GET_STATE << 1) | (rack << 7) | (slot << 5));
	
	//delay(1);
	canData[0] = 0x02; canData[1] = 0x00;
	sendFrame(IOCAN_Socket, canID, 2, canData);		// set input card state
	return SUCCESS;
}

int DI_GetValues(char rack, char slot, unsigned char Values[])
{
	struct can_frame rcvCANFrame = { 0 };
	char getData[2], i;
	int canID;

	rack &= 15;
	slot &= 3;

	canID = ((SET_GET_DATA << 1) | (rack << 7) | (slot << 5)); // get data from digital input @ specified locations

	sendFrame(IOCAN_Socket, canID, 0, getData);  // Send the read data request to the input data

	receiveFrame(IOCAN_Socket, &rcvCANFrame);
	for (i = 0; i < rcvCANFrame.can_dlc; i++)
		Values[i] = rcvCANFrame.data[i];

	//printRcvFrame(rcvCANFrame);

	return SUCCESS;
}
