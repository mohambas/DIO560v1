#include "DIO.h"
#include "BSWCAN.h"
#include "AIO.h"
#include "GeneralFunctions.h"

int AO_SetSettings(char rack, char slot, struct AO_Settings newSettings)
{
	////////////// AO Card Settings ///////////////
	// 0AD    SET_SETTING  		Output    Rack#: 1  Slot#: 1       -   [8] 16 41 05 00 01 FF FF 00
    // 0AD    SET_SETTING  		Output    Rack#: 1  Slot#: 1       -   [1] 00
    // 0AD    SET_SETTING  		Output    Rack#: 1  Slot#: 1       -   [8] 00 00 00 00 00 00 00 00
    // 0AD    SET_SETTING  		Output    Rack#: 1  Slot#: 1       -   [7] 00 00 00 00 00 00 00
    // 0AD    SET_SETTING  		Output    Rack#: 1  Slot#: 1       -   [1] 00

    // 0A9    SET_GET_STATE  	Output    Rack#: 1  Slot#: 1   	   -   [2] 02 00

	unsigned char canData[10] = { 0 };
	struct can_frame rcvCANFrame = { 0 };
	int canID;

	canID = ((SET_SETTING << 1) | (rack << 7) | (slot << 5) | 0x01);

	canData[0] = 22;  // Settings size
	canData[1] = newSettings.OHLimit;
	canData[2] = newSettings.OHHystersis;
	canData[3] = newSettings.RedundantBPAdd;

	canData[4] = 0xFF;  // reserved
	canData[5] = newSettings.activeChannel;
	canData[6] = newSettings.safeValueActive;
	canData[7] = newSettings.safeValues[0] >> 8;

	sendFrame(IOCAN_Socket, canID, 8, canData);	// set Settings (first section)
	receiveFrame(IOCAN_Socket, &rcvCANFrame);	    // wait for the AO card ACK

	canData[0] = newSettings.safeValues[1] >> 8;
	canData[1] = newSettings.safeValues[2] >> 8;
	canData[2] = newSettings.safeValues[3] >> 8;
	canData[3] = newSettings.safeValues[4] >> 8;

	canData[4] = newSettings.safeValues[5] >> 8;
	canData[5] = newSettings.safeValues[6] >> 8;
	canData[6] = newSettings.safeValues[7] >> 8;
	canData[7] = newSettings.safeValues[0] & 0x00FF;

	//delay(1);
	sendFrame(IOCAN_Socket, canID, 8, canData);	// set Settings (second section)
	receiveFrame(IOCAN_Socket, &rcvCANFrame);		// wait for the DO card ACK

	canData[0] = newSettings.safeValues[1] & 0x00FF;
	canData[1] = newSettings.safeValues[2] & 0x00FF;
	canData[2] = newSettings.safeValues[3] & 0x00FF;
	canData[3] = newSettings.safeValues[4] & 0x00FF;
	
	canData[4] = newSettings.safeValues[5] & 0x00FF;
	canData[5] = newSettings.safeValues[6] & 0x00FF;
	canData[6] = newSettings.safeValues[7] & 0x00FF;

	//delay(1);
	sendFrame(IOCAN_Socket, canID, 7, canData);	// Set Settings (second section)
	receiveFrame(IOCAN_Socket, &rcvCANFrame);		// Wait for the DO card ACK
	return SUCCESS;
}

int AO_SetState(char rack, char slot, int newState)
{
	int canID;
	unsigned char canData[4] = { 0 };

	rack &= 15;
	slot &= 3;
	canID = ((SET_GET_STATE << 1) |(rack << 7) | (slot << 5) | 0x01);

	canData[0] = newState; canData[1] = 0x00;
	sendFrame(IOCAN_Socket, canID, 2, canData);		// set output card state
	return SUCCESS;
}

// set the same settings (the default one) to the all connected DI cards
int AO_SetAllCardsSettings(char PluggedIOcards[][N_RACK_SLOTS], struct AO_Settings Settings)
{
	char r, c;
	for (r = 0; r < N_MAX_RACKS; r++)
	{
		for (c = 0; c < N_RACK_SLOTS; c++)
			if (PluggedIOcards[r][c] == AO672 || PluggedIOcards[r][c] == AO673)  // or all types of DO cards.
			{
				// AO settings
				AO_SetSettings(r, c, Settings);
				AO_SetState(r, c, MAIN);  // set the card state to MAIN
			}
	}
	return SUCCESS;
}

void AO_SetDefaultSettings(struct AO_Settings *defaultSettings)
{
	defaultSettings->OHLimit = 65;
	defaultSettings->OHHystersis = 5;
	defaultSettings->RedundantBPAdd = 0;

	defaultSettings->activeChannel = 0xFF;
	defaultSettings->safeValueActive = 0x00;

	defaultSettings->safeValues[0] = 0x0000;
	defaultSettings->safeValues[1] = 0x0000;
	defaultSettings->safeValues[2] = 0x0000;
	defaultSettings->safeValues[3] = 0x0000;

	defaultSettings->safeValues[4] = 0x0000;
	defaultSettings->safeValues[5] = 0x0000;
	defaultSettings->safeValues[6] = 0x0000;
	defaultSettings->safeValues[7] = 0x0000;
}

int AO_SetValues(char rack, char slot, unsigned int Values[])
{
	int canID;
	unsigned char byteValues[10];

	rack &= 15;
	slot &= 3;

	canID = ((SET_GET_DATA << 1) | (rack << 7) | (slot << 5) | 0x01); // set data for output card in a specified location (rack and slot)

	byteValues[0] = Values[0] & 0x00FF;	byteValues[1] = Values[0] >> 8;
	byteValues[2] = Values[1] & 0x00FF; byteValues[3] = Values[1] >> 8;
	byteValues[4] = Values[2] & 0x00FF; byteValues[5] = Values[2] >> 8;
	byteValues[6] = Values[3] & 0x00FF; byteValues[7] = Values[3] >> 8;

	sendFrame(IOCAN_Socket, canID, 8, byteValues);

	byteValues[0] = Values[4] & 0x00FF; byteValues[1] = Values[4] >> 8;
	byteValues[2] = Values[5] & 0x00FF; byteValues[3] = Values[5] >> 8;
	byteValues[4] = Values[6] & 0x00FF; byteValues[5] = Values[6] >> 8;
	byteValues[6] = Values[7] & 0x00FF; byteValues[7] = Values[7] >> 8;

	sendFrame(IOCAN_Socket, canID, 8, byteValues);

	return SUCCESS;
}

//-----------------------------------------------------------------------//
// ------------------------ Analoge Input ------------------------------ //
int AI_SetSettings(char rack, char slot, struct AI_Settings newSettings)
{
	unsigned char canData[10] = { 0 };
	struct can_frame rcvCANFrame = { 0 };

	int canID;

	rack &= 15;
	slot &= 3;

	canID = ((SET_SETTING << 1) | (rack << 7) | (slot << 5));

	canData[0] = 5;  // Settings size
	canData[1] = newSettings.OHLimit;
	canData[2] = newSettings.OHHystersis;
	canData[3] = newSettings.RedundantBPAdd;

	canData[4] = newSettings.filterType;
	canData[5] = newSettings.activeChannel;

	sendFrame(IOCAN_Socket, canID, 6, canData);	// set Settings (first section)
	receiveFrame(IOCAN_Socket, &rcvCANFrame);	// wait for the DO card ACK
	receiveFrame(IOCAN_Socket, &rcvCANFrame);	// wait for the DO card ACK

	return SUCCESS;
}

int AI_SetState(char rack, char slot, char newState)
{
	int canID;
	unsigned char canData[3] = { 0 };

	rack &= 15;
	slot &= 3;

	canID = ((SET_GET_STATE << 1) | (rack << 7) | (slot << 5));

	//delay(1);
	canData[0] = newState; canData[1] = 0x00;
	sendFrame(IOCAN_Socket, canID, 2, canData);		// set output card state

	return SUCCESS;
}

int AI_SetAllCardsSettings(char PluggedIOcards[][N_RACK_SLOTS], struct AI_Settings AI_Settings, struct TI_Settings TI_Settings)
{
	char r, c;
	for (r = 0; r < N_MAX_RACKS; r++)
	{
		for (c = 0; c < N_RACK_SLOTS; c++)
		{
			if (PluggedIOcards[r][c] == AI622 || PluggedIOcards[r][c] == AI623)  // or all types of DO cards.
			{
				// AI settings
				AI_SetSettings(r, c, AI_Settings);
				AI_SetState(r, c, MAIN);	// set the card state to MAIN
			}
			else if (PluggedIOcards[r][c] == AI621)  // or all types of DO cards.
			{
				// TI settings
				TI_SetSettings(r, c, TI_Settings);
				TI_SetState(r, c, MAIN);	// set the card state to MAIN
			}
		}
	}
	return SUCCESS;
}


void AI_SetDefaultSettings(struct AI_Settings *defaultSettings)
{
	defaultSettings->OHLimit = 65;
	defaultSettings->OHHystersis = 5;
	defaultSettings->RedundantBPAdd = 0;

	defaultSettings->filterType = 3;
	defaultSettings->activeChannel = 0xFF;
}

int AI_GetValues(char rack, char slot, unsigned int Values[])
{
	struct can_frame rcvCANFrame = { 0 };
	unsigned char getData[2];
	int canID = 0;
	rack &= 15;
	slot &= 3;

	canID = ((SET_GET_DATA << 1) | (rack << 7) | (slot << 5));	// get data from input @ specified locations

	sendFrame(IOCAN_Socket, canID, 0, getData);

	receiveFrame(IOCAN_Socket, &rcvCANFrame);
	Values[0] = rcvCANFrame.data[0] + ((int)rcvCANFrame.data[1] << 8);
	Values[1] = rcvCANFrame.data[2] + ((int)rcvCANFrame.data[3] << 8);
	Values[2] = rcvCANFrame.data[4] + ((int)rcvCANFrame.data[5] << 8);
	Values[3] = rcvCANFrame.data[6] + ((int)rcvCANFrame.data[7] << 8);

	//printRcvFrame(rcvCANFrame);

	receiveFrame(IOCAN_Socket, &rcvCANFrame);  // receive 4 remained channels value
	Values[4] = rcvCANFrame.data[0] + ((int)rcvCANFrame.data[1] << 8);
	Values[5] = rcvCANFrame.data[2] + ((int)rcvCANFrame.data[3] << 8);
	Values[6] = rcvCANFrame.data[4] + ((int)rcvCANFrame.data[5] << 8);
	Values[7] = rcvCANFrame.data[6] + ((int)rcvCANFrame.data[7] << 8);

	//printRcvFrame(rcvCANFrame);

	// for AI621 case: 
	// AI621(TI) reports 16 channels value but in real world it has only 8 input channles; therefore 2 useless CAN message should be received (dropped)  
	if (pluggedCards[rack][slot] == AI621)
	{
		receiveFrame(IOCAN_Socket, &rcvCANFrame); // ** unused **
		receiveFrame(IOCAN_Socket, &rcvCANFrame); // ** unused **
	}

	return SUCCESS;
}

/// ---------------------------------------------------------------------------///
/// ---------------------------- TI (AI621) -----------------------------------///
void TI_SetDefaultSettings(struct TI_Settings *defaultSettings)
{
	defaultSettings->OHLimit = 65;
	defaultSettings->OHHystersis = 5;
	defaultSettings->RedundantBPAdd = 0;

	defaultSettings->filterType = 3;
	defaultSettings->ambientType = 0;

	defaultSettings->sensorType[0] = 2;
	defaultSettings->sensorType[1] = 2;
	defaultSettings->sensorType[2] = 2;
	defaultSettings->sensorType[3] = 2;
	defaultSettings->sensorType[4] = 2;
	defaultSettings->sensorType[5] = 2;
	defaultSettings->sensorType[6] = 2;
	defaultSettings->sensorType[7] = 2;

	defaultSettings->sensorName[0] = 1;
	defaultSettings->sensorName[1] = 1;
	defaultSettings->sensorName[2] = 1;
	defaultSettings->sensorName[3] = 1;
	defaultSettings->sensorName[4] = 1;
	defaultSettings->sensorName[5] = 1;
	defaultSettings->sensorName[6] = 1;
	defaultSettings->sensorName[7] = 1;

	defaultSettings->lineResistor[0] = 0;
	defaultSettings->lineResistor[1] = 0;
	defaultSettings->lineResistor[2] = 0;
	defaultSettings->lineResistor[3] = 0;
	defaultSettings->lineResistor[4] = 0;
	defaultSettings->lineResistor[5] = 0;
	defaultSettings->lineResistor[6] = 0;
	defaultSettings->lineResistor[7] = 0;
}

int TI_SetSettings(char rack, char slot, struct TI_Settings newSettings)
{
	/*cData[0] = 0x25; cData[1] = 0x41; cData[2] = 0x05; cData[3] = 0x00;
	cData[4] = 0x01; cData[5] = 0x00; cData[6] = 0x0A; cData[7] = 0x00;
	sendFrame(0xEC, 8, cData);		// set AI Settings
	receiveFrame(&rcvFrame);
	
	cData[0] = 0x0A; cData[1] = 0x00; cData[2] = 0x0A; cData[3] = 0x00;
	cData[4] = 0x0A; cData[5] = 0x00; cData[6] = 0x0A; cData[7] = 0x00;
	sendFrame(0xEC, 8, cData);		// set AI Settings
	
	cData[0] = 0x0A; cData[1] = 0x00; cData[2] = 0x0A; cData[3] = 0x00;
	cData[4] = 0x0A; cData[5] = 0x00; cData[6] = 0x00; cData[7] = 0x00;
	sendFrame(0xEC, 8, cData);		// set AI Settings
	
	cData[0] = 0x00; cData[1] = 0x00; cData[2] = 0x00; cData[3] = 0x00;
	cData[4] = 0x00; cData[5] = 0x00; cData[6] = 0x00; cData[7] = 0x00;
	sendFrame(0xEC, 8, cData);		// set AI Settings
	
	sendFrame(0xEC, 6, cData);		// set AI Settings
	receiveFrame(&rcvFrame);*/
	
	unsigned char canData[10] = { 0 };
	struct can_frame rcvCANFrame = { 0 };

	int canID;

	rack &= 15;
	slot &= 3;

	canID = ((SET_SETTING << 1) | (rack << 7) | (slot << 5));

	canData[0] = 37;  // Settings size -> 0x25
	canData[1] = newSettings.OHLimit;
	canData[2] = newSettings.OHHystersis;
	canData[3] = newSettings.RedundantBPAdd;

	canData[4] = newSettings.filterType;
	canData[5] = newSettings.ambientType;

	// Sensor Setting structure
	/*----|-----|-----|-----|-----||-----|-----|-----\
	| SN4 | SN3 | SN2 | SN1 | SN0 || ST2 | ST1 | ST0 |
	\-----|-----|-----|-----|-----||-----|-----|----*/

	canData[6] = ((newSettings.sensorName[0] << 3) | newSettings.sensorType[0]);
	canData[7] = newSettings.lineResistor[0];

	sendFrame(IOCAN_Socket, canID, 8, canData);	// set Settings (first section)
	receiveFrame(IOCAN_Socket, &rcvCANFrame);		// wait for receive

	canData[0] = (newSettings.sensorName[1] << 3) | (newSettings.sensorType[1]);
	canData[1] = newSettings.lineResistor[1];
	canData[2] = (newSettings.sensorName[2] << 3) | (newSettings.sensorType[2]);
	canData[3] = newSettings.lineResistor[2];
	canData[4] = (newSettings.sensorName[3] << 3) | (newSettings.sensorType[3]);
	canData[5] = newSettings.lineResistor[3];
	canData[6] = (newSettings.sensorName[4] << 3) | (newSettings.sensorType[4]);
	canData[7] = newSettings.lineResistor[4];

	sendFrame(IOCAN_Socket, canID, 8, canData);	// set Settings (first section)
	//receiveFrame(&rcvCANFrame);		// wait for receive

	canData[0] = (newSettings.sensorName[5] << 3) | (newSettings.sensorType[5]);
	canData[1] = newSettings.lineResistor[5];
	canData[2] = (newSettings.sensorName[6] << 3) | (newSettings.sensorType[6]);
	canData[3] = newSettings.lineResistor[6];
	canData[4] = (newSettings.sensorName[7] << 3) | (newSettings.sensorType[7]);
	canData[5] = newSettings.lineResistor[7];
	canData[6] = 0;
	canData[7] = 0;

	sendFrame(IOCAN_Socket, canID, 8, canData);	// set Settings (first section)

	canData[0] = 0;
	canData[1] = 0;
	canData[2] = 0;
	canData[3] = 0;
	canData[4] = 0;
	canData[5] = 0;
	canData[6] = 0;
	canData[7] = 0;
	sendFrame(IOCAN_Socket, canID, 8, canData);	// set Settings (first section)
	
	sendFrame(IOCAN_Socket, canID, 6, canData);	// set Settings (first section)

	receiveFrame(IOCAN_Socket, &rcvCANFrame);		// wait for receive

	return SUCCESS;
}

int TI_SetState(char rack, char slot, char newState)
{
	int canID;
	unsigned char canData[3] = { 0 };

	rack &= 15;
	slot &= 3;

	canID = ((SET_GET_STATE << 1) | (rack << 7) | (slot << 5));

	//delay(1);
	canData[0] = newState; canData[1] = 0x00;
	sendFrame(IOCAN_Socket, canID, 2, canData);		// set output card state

	return SUCCESS;
}