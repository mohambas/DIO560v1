#pragma once

struct AO_Settings   // DO cards settings structure
{
	unsigned char OHLimit;// = 65;
	unsigned char OHHystersis;// = 5;
	unsigned char RedundantBPAdd;// = 0;

	unsigned char activeChannel;	// {0xFF}
	unsigned char safeValueActive;	// {0x00}
	unsigned int safeValues[8];// = {0, 0, 0, 0, 0, 0, 0, 0}; // for 8 channels
};

struct AI_Settings   // DI cards settings structure
{
	unsigned char OHLimit;// = 65;
	unsigned char OHHystersis;// = 5;
	unsigned char RedundantBPAdd;// = 0;
	unsigned char filterType;// = 3		// 0: No filter  
										// 1: Averaging
										// 2: Recursive (50%)
										// 3: Recursive (70%)
										// 4: Recursive (80%)

	unsigned char activeChannel;// = 0xFF
};

struct TI_Settings   // TI card AI621 settings structure
{
	unsigned char OHLimit;// = 65;
	unsigned char OHHystersis;// = 5;
	unsigned char RedundantBPAdd;// = 0;
	unsigned char filterType;// = 3		// 0: No filter  
										// 1: Averaging
										// 2: Recursive (50%)
										// 3: Recursive (70%)
										// 4: Recursive (80%)

	unsigned char ambientType;// = 0  	// 0: internal ambient Source
										// 1: External Ambient Source
										// 2: Fixed Zero Ambient Source
										// 3: Fixed 25 Ambient Source

	unsigned char sensorType[8];	// = {2, 2, 2, 2, 2, 2, 2, 2};
									// Open:			0
									// Thermocouple:	1
									// RTD 2 Wire:		2
									// RTD 3 Wire:		3
									// RTD 4 Wire:		4

	unsigned char sensorName[8];	// = {1, 1, 1, 1, 1, 1, 1, 1};

	/* if (sensorType = Thermocouple)	|  if (sensorType = RTD (2/3/4) wires)	|				
	|			TC_TYPE_B				|			 RTD PT100					|
	|			TC_TYPE_E				|			 RTD PT500					|
	|			TC_TYPE_J				|			 RTD PT1000					|
	|			TC_TYPE_K				|										|
	|			TC_TYPE_N				|										|
	|			TC_TYPE_R				|										|
	|			TC_TYPE_S				|										|
	|			TC_TYPE_T				|										|*/

	unsigned char lineResistor[8]; // = {0, 0, 0, 0, 0, 0, 0, 0};
};


// ------------------------ Analoge Output --------------------------- //
int AO_SetSettings(char rack, char slot, struct AO_Settings newSettings); // Analoge Output cards settings
int AO_SetAllCardsSettings(char PluggedIOcards[][N_RACK_SLOTS], struct AO_Settings Settings);

void AO_SetDefaultSettings(struct AO_Settings *);
int AO_SetState(char rack, char slot, int newState);
int AO_SetValues(char rack, char slot, unsigned int Values[]);

// ------------------------ Analoge Input ------------------------------ //
int AI_SetSettings(char rack, char slot, struct AI_Settings newSettings); // Analoge Input cards settings
int AI_SetAllCardsSettings(char PluggedIOcards[][N_RACK_SLOTS], struct AI_Settings AI_Settings, struct TI_Settings TI_Settings);

void AI_SetDefaultSettings(struct AI_Settings *);
int AI_SetState(char rack, char slot, char newState);
int AI_GetValues(char rack, char slot, unsigned int Values[]);

//---------------------------------------------------------------------- //
// ------------ Temprature Input (Special kind of AI) ------------------ //
void TI_SetDefaultSettings(struct TI_Settings *);
int TI_SetSettings(char rack, char slot, struct TI_Settings newSettings); // Analog Input cards settings
int TI_SetState(char rack, char slot, char newState);