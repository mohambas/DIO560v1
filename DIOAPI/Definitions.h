#ifndef __DEFINITIONS_H_
				  
#define __DEFINITIONS_H_


#define  DEBUG_MODE
//#define RELEASE

// spi commands
// === Commands ===
#define CAN_RESET       0xC0 
#define CAN_READ        0x03 

// === Registers ===
#define CANSTAT         0x0E
#define CANCTRL         0x0F

int ProductType;

enum results
{
	SUCCESS = 0,
	ERROR   = 1
};

enum cardState
{ 
	IDLE           = 1, 
	MAIN           = 2, 
	BACKUP         = 3, 
	MAIN_FS        = 4, 
	BACKUP_FS      = 5, 
	OUT_OF_SERVICE = 6 
};

enum cardTypes
{
	MPU1  = 0,
	MPU2  = 1,  //redundant
	DI530 = 2,
	DO580 = 4,
	DO571 = 5,
	AI621 = 6,
	AO673 = 8,
	AI623 = 9,
	AI622 = 11,
	AO672 = 13,
	NA    = 16
};

enum commandSet
{
	SYNC              = 0,
	ROLE_IDENTIFY     = 1,
	RESERVED_1        = 2,
	RESERVED_2        = 3,
	SET_GET_STATE     = 4,
	SET_GET_DATA      = 5,
	SET_SETTING       = 6,
	INITIATE_TRANSFER = 8, 
	DOWNLOAD_DATA     = 9,
	UPLOAD_DATA       = 10,
	HOT_REDUNDANCY    = 15
};

#define		N_MAX_RACKS			16
#define		N_RACK_SLOTS		4
#define		MAX_NUM_IO_CARDS	62  // 6 bits address (4 bits for rack and 2 bits for slot) be advised that 2 slots is occuppied with the MPUs

//#define		RTU_450				450
#define		RTU_451				451
//#define		DIO_560				560

//// Definition of RTU-450 LED Pins
//#define 	RUN_LED_450			23
//#define 	STBY_LED_450		24
//#define 	STATUS_LED_450		25
//#define 	FLT_LED_450			28
//
//// Definition of RTU-451 LED Pins
//#define 	RUN_LED_451			2
//#define 	STBY_LED_451		3
//#define 	STATUS_LED_451		5
//#define 	FLT_LED_451			4

#ifdef RTU_451

#define 	RUN_LED_560			2
#define 	STBY_LED_560		3
#define 	STATUS_LED_560		4
#define 	FLT_LED_560			5

#define		PIN_Base_560		0
#endif

////Definition of DIO-560 LED Pins
#ifdef DIO_560
				  
#define 	RUN_LED_560			8
#define 	STBY_LED_560		9
#define 	STATUS_LED_560		10
#define 	FLT_LED_560			11
#define		PIN_Base_560		100
#endif // DIO_560

#define		ADE1				6
#define		ADE2				7

#define		BP_ADDR0			2	
#define		BP_ADDR1			3
#define		BP_ADDR2			4   
#define		BP_ADDR3			5
#define		BP_ADDR4			6
#define		BP_ADDR5			7

#define		INPUT_CS_1			2
#define		INPUT_CS_2			3
#define		OUTPUT_CS			38
#define		OE					39

#define		D0					30
#define		D1					31
#define		D2					32
#define		D3					33
#define		D4					34
#define		D5					35
#define		D6					36
#define		D7					37

#define		FPOW_INPUTS_FLAG_1	14
#define		FPOW_INPUTS_FLAG_2	15
#define		FPOW_OUTPUTS_FLAG_1	0
#define		FPOW_OUTPUTS_FLAG_2	1

#define		OUTPUT_STATUS		4



// General pinout
#define 	RUN_LED			1
#define 	STBY_LED		2
#define 	STATUS_LED		3
#define 	FLT_LED			4


#define		LED_ON			0
#define		LED_OFF			1


#define		MODEL_3BPLUS			13
#define		MODEL_CM3				10
#define		MODEL_3B				8
#define		MODEL_2B				4

#define		MANUFACTURER_SONYUK				0
#define		MANUFACTURER_EGOMAN				1
#define		MANUFACTURER_EMBEST_1			2
#define		MANUFACTURER_EMBEST_2			4
#define		MANUFACTURER_SONYJAPAN			3
#define		MANUFACTURER_STADIUM			5

#define		PROCESSOR_BCM2835			0
#define		PROCESSOR_BCM2836			1
#define		PROCESSOR_BCM2837			2

#endif  //__DEFINITIONS_H_