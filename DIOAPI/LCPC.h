/***********************************************************************************
*  File      : LCPC.h
*----------------------------------------------------------------------------------
*  Object    : This file contains required functions for Initializing and 
*              handling LCPC card and the Microcontroller chip.
*
*  Contents  : constants and functions' prototype
*
* ---------------------------------------------------------------------------------
*  Date     |	Description
* ----------+----------------------------------------------------------------------

* ----------+----------------------------------------------------------------------
***********************************************************************************/

#ifndef __LCPC_H_
#define __LCPC_H_				  
#include "GeneralFunctions.h"
#include "Hardware.h"
#include "Common.h"

/************************************************************************************
*       General definitions
***********************************************************************************/
// Card general states:
#define     IDLE				1		// Initialize & Check the IO Card
#define     MAIN				2		// Card is doing its main job
#define     BACKUP				3		// Card stands as a backup for the main card
#define     FAIL_SAFE_MAIN		4		// CAN Connection seems to be disconnected, State was MAIN
#define     FAIL_SAFE_BACKUP  	5		// CAN Connection seems to be disconnected, State was BACKUP
#define     OUT_OF_SERVICE		6       // Card is out of service

// Card group
#define     INPUT_GROUP		                0x00		
#define     OUTPUT_GROUP	                0x01		
#define     INOUT_GROUP	                    0x00
#define     CMD_CHANGE_HOTREDUNDANCY_L        'H'
#define     CMD_CHANGE_HOTREDUNDANCY        'h'
/************************************************************************************
*       MAPCS version
***********************************************************************************/
#define CMD_CHANGE_MAPCS_VER       'x'
#define CMD_CHANGE_MAPCS_VER_L       'X'
#define MAPCS1                      0   //MAPCS1 backplane produvts
#define MAPCS2A                     1   //MAPCS2A products(metal rack mount products)
/**************************************************************************************
*       User configurable definitions
**************************************************************************************/
//  NOTE-------------NOTE--------------NOTE: deprecated	
    #define LCPC_Read                         LCPC_InternalRead
    #define LCPC_Write_Byte                   LCPC_InternalWrite_Byte
/* 
// We provided two parallel routines and can be selected every one; the interna or external E2PROM
#if E2PROMType == 0
    #define LCPC_E2PROMRead                         LCPC_InternalE2PRead
    #define LCPC_E2PROMWrite_Byte                   LCPC_InternalE2PWrite_Byte
    #define LCPC_E2PROMTest                         LCPC_InternalE2PTest
#else
    #define LCPC_E2PROMRead                         LCPC_ExternalE2PRead
    #define LCPC_E2PROMWrite_Byte                   LCPC_ExternalE2PWrite_Byte
    #define LCPC_E2PROMTest                         LCPC_ExternalE2PTest
#endif

//// Note:   Size of EEPROM word address (Set due to data sheet)
//// EEPROM needs double word address (AT24C64 or AT24C512)
//// EEPROM needs single word address (AT24C04)	
//#define WORDADD_SIZE	            2		    // see above
//#define PAGE_SIZE		            32		    // EEPROM page size (32 for AT24C64, 128 for AT24C512 & 16 for AT24C04) (***)
//#define PAGE_NUMBER		            256		    // Number of pages for each E2PROM (256 for AT24C64, 512 for AT24C512 & 32 for AT24C04) (***)
//
//// Note:   In the case of using AT24C01 to AT24C16 (One word addressing)
//// Change the mask according to device word address bits.Pay attention
//// E2PROM_SLAVEADD shuold change due to Datasheet description.	
//#define	E2PROM_ADDMASK		        0x01ff		// See above
//
//#define LoopTimesforWrite2E2PROM    1           // Number of E2PROM cells that will be programmed in every internal cycle if it needs
//
//#define EEPROM_OK		            0x01		// EEPROM works properly
//#define EEPROM_FAILED	            0x02		// EEPROM doesn't work
//#define ExternalRAMisOK              0
//*/
//
//#define ENABLE_OUTPUTS		PORTF &= ~(1 << PORTF3)		// AP_OE1 = 0	(PE5) NOTE : Just For AO Card
//
///*************************************************************************************
//*       Temperature Sensor's definitions
//***********************************************************************************/
////Slave addresses could be found in hardware schematic
//
////#define LCPC_IOEXP_SLA		0x20	// deprecated: SLA address of LCPC IO Expander (MAX7313_U2) (LEDs + BP Address)
////#define	LCPC_E2PROM_SLA		0x52	// deprecated: SLA address of LCPC EEPROM (AT24Cxx_U12)
//#define LCPC_TEMPS_SLA			0x48	// SLA Address of LCPC Temperature sensor
////#define LCPC_TYPEVER_SLA		0x28	// deprecated: SLA Address of IO Card's Type & Version definer IO Expander (MAX7313)
////#define IO_TEMPSENSOR_SLA	    0x4c	// deprecated: SLA address of IO Card's temperature sensor
//
//// Functions return:
#define	RET_SUCCESS		        0		// Function is done successfully
#define ERR_IICBUS		        1		// IIC not working properly (Collision or bus error)
#define ERR_NOTRESPOND	        2		// Device doesn't respond
#define ERR_REPEAT		        3		// Error happened and repeation of function may required
#define ERR_PAGESIZE	        4		// Data size is greater than EEPROM page size
#define ERR_TESTFAILED	        5		// Data size is greater than EEPROM page size
#define ERR_TIMEOUT		        6		// Function execution timed out
#define RET_OVERHEAT	        7		// Over heat detected
#define RET_HYSTHEAT	        8		// Temperature is in hysteresis margin
#define RET_NORMHEAT	        9		// Normal heat detected
//#define ERR_FATAL		        10		// Fatal error occured

///************************************************************************************
//*       Note:	Modify these lines if you want to change the settings of temperature
//*		sensor (TCN75).
//***************************************************************************************/
//#define TS_FAULTQUEUE	        2		// defines how many read should be done before reporting temperature by temp sensor
//#define TS_SETPOINT		        60		// Default setpoint for the temperature sensor in comparator mode (Degrees Centigrade)
//#define TS_HYSTERESIS	        5		// Default hysteresis value for the temperature sensor in comparator mode (Degrees Centigrade)

/************************************************************************************
*                Card Structures
**************************************************************************************/
// IO Card data structures:
union LCPCErrorUnion
{
    struct
    {
        UChar Link1:1;         		// CAN0's connection error
        UChar Link2:1;         		// CAN1's connection error
        UChar Link3:1;         		// CAN2's connection error
        UChar TypeVer:1;       		// error in Type/Version
        UChar TemperatureSensor:1;  // Temp. sensor error in access
        UChar Res1:1;				// reserved 
        UChar Res2:1;				// reserved 
        UChar Res3:1;				// reserved 
    }Bit;
    UChar D8;
};

union LCPCWarningUnion
{
    struct
    {
        UChar Communication:1;	    // TEC & REC
        UChar DataReceive:1;	    // CAN's Receive Error (CANINTF_ERRIF)
        UChar DataTransmit:1;       // CAN's Transmit Error (CANINTF_MERRF)
        UChar ExternalRAM:1;	    // External RAM's error flag (reserved)
        UChar EEPROM:1;             // E2PROM's error flag 
        UChar OverheatLCPC:1;	    // Overheat LCPC warning
        UChar OverheatIO:1;	        // Overheat IO warning flag (reserved)
        UChar ResetViaWatchdog:1;   // System has seen one watchdog reset
    }Bit;
    UChar D8;
};

struct GenericSettingStruct
{
    UChar OverHeatLimit;    // Set point for temperature alarming
    UChar OverHeatBand;     // Hysteresis for temperature alarming
    UChar PairCardAddress;  // Related Redundant Backplane address    
};

struct PCCardStruct
{
    UChar Group;			// The Group of the card
    UChar Type1;			// The Type1 of the card
    UChar Type2;			// The Type1 of the card
    UChar Version;			// The card version
    UChar Revision;			// The card revision
    UChar ModuleType;		// The Hardweare defined module type
    UChar Type;				// Complete Card Type
    UChar FirmwareVersion;	// The firmware version of the card
    UChar HardwareVersion;	// The hardware version of the card
    UChar LCPCType;			// Type of LCPC

    UChar BPAddress;		// The Backplane Address of the card

    UChar State;			// The state of the card ( Use 3-bits )
    UChar NewState;			// The new state of the card forced by MPU
    UChar PreviousState;	// The previous State

    UChar HARTState;		// State of HART communication

    SChar TemperatureLCPC;	// Keeps card recently read temperature

    UChar FieldPowerStatus;	// The status of the field power
    bool  IOFault;			// Fault occured in IO Cards
    UChar IOFaultCounter;	// Fault Counter in IO Cards

    UChar CAN_EFLG;				// CAN Error Flag
    UChar RxReady;           	// If not zero indicates that new data received from CAN

    UChar GeneralLED;		  	// LCPC LEDs condition
    UInt  LedBlinkCounter100ms;
    UInt  LedBlinkCounter200ms;
    UInt  LedBlinkCounter500ms;	// Counter for various blink frequencies
    UInt  LedBlinkCounter1s;	// Counter for various blink frequencies
    UChar HasDiag ;				// a flag to set if Card has any diagnostics

    union LCPCWarningUnion     LCPCWarning;			// Warning Structure for LCPC
    union LCPCWarningUnion     PreviousLCPCWarning; // previous Warning structure for LCPC
    union LCPCErrorUnion       LCPCError;			// Error Structure for LCPC
    union LCPCErrorUnion       PreviousLCPCError;	// previous Error structure for LCPC

    UInt    Timer2_CounterA;	// Link timeout to inter FAILSAFE state
    UInt    Timer2_CounterB;	// CAN Send message timeout (reserved)
    UInt    Timer2_CounterC;	// CAN Performance check (reserved)
    UInt    Timer2_CounterD;	// Latch timeout (AO and DO) (reserved)
    UInt    Timer2_CounterE;	// LED Refresh interval counter (reserved)
    UInt    Timer2_CounterF;	// ADC watchdog (AI, AIH & TI), SPM Timeout in Bootloader
    UInt    Timer2_CounterG;	// Timer for managing Hot Redundancy(CAN2 receiving time out) (reserved)
    UInt    Timer2_CounterH;	// Timer for managing Hot Redundancy(CAN2 sending interval) (reserved)
    UInt    Timer2_CounterI;	// fail safe timer(CAN0&1 sending interval) (reserved)
    UInt    Timer2_Calibration;	// Calibration Timeout (reserved)
    UInt    Timer2_SetFilter;	// ADC SetFilter Timeout
    UInt    SerialPortTimer;	// Serial Port Timeout

    UInt    LEDsUpdateTime;		// LED update timer to be compared with LED_UPDATE_TIME_VAL in AI621
    bool    LED_Update_Flag;	// a flag to update LED based on 
	
    UInt    IO_LED_Timer;		// LED update timer for IO Cards
    bool    IO_LED_Update_Flag;	// LED update flag for IO Cards 

    UInt    OpenLoadTestTime;	// Open Load Test timer for AI621
    bool    OpenLoadTestFlag;	// Open Load Test Flag for AI621

    UInt    ConvTime;			// conversion-time timer for Analog Input Cards

    UChar   HotRedundancyTxTimer;		// Timer for Hot Redundancy Transmit
    UChar   HotRedundancyRxTimer;		// Timer for Hot Redundancy Receive
    UChar   HotRedundancyActive;		// Hot Redundancy Active Flag
    UChar   HotRedundancyFailCounter;	// Counter for Hot Redundancy Fail events
	
    UInt    UART3_RX_Timer;         	// USART3 RX Time Out Counter
    UChar   CAN_TX_Timeout_Timer[3];	// CAN time out timer to check out activity

    UChar   NewSettingReceived ;		// flag to check if new setting is received
    UInt    SizeOfSetting ;	// size of setting received from MPU
    UInt    SettingOffset ; // Current offset in Setting Structure, it's incremented while receiving SetSetting packets
    UChar MAPCS_VER;
	bool	IsCalibration;	// a flag to switch between calibration/normal mode in AI/AO Cards
};

/*************************************************************************************
*				LCPC Function Prototypes
**************************************************************************************/
//// LCPC Card General functions:
UChar LCPC_Init( UChar CardType );
//UChar LCPC_Translate_IICRetVal( UChar RetVal );
//
//// IO Card Type and Version:
UChar LCPC_GetIOTypeVer(UChar* pCardType, UChar* pCardVer);
//UChar LCPC_Get_Type(void );
//
//// Micro Controller:
void Timer2OVHandler(int signo);
void LCPC_EnableInterrupt(void);
//void LCPC_DisableInterrupt(void);
void LCPC_SetTimer2(void);
void timer_seconds(long int seconds);
void timer_useconds(long int useconds);
void LCPC_SHInputThread(void);
//UChar LCPC_USARTTransmit( UChar *Data, UChar DataLen );
//Int putchar(Int c);
//
////void LCPC_IOExpLEDAddInit( void );  // deprecated
void  LCPC_DefBackplaneAddress( void );
UChar LCPC_GetBackplaneAddress( void );
void  LCPC_SetLEDs(UChar LEDVal);
UChar CheckCardTypeAndVersion( UChar CardType );

// DAQ:
//UChar LCPC_ExternalE2PWrite_Byte(UInt WordAdd, UChar DataByte); // deprecated
//UChar LCPC_ExternalE2PWrite_Page(UInt WordAdd, UChar *DataBuf, UChar DataSize); // deprecated
//UChar LCPC_ExternalE2PRead_Current(UChar data); // deprecated
//UChar LCPC_ExternalE2PRead(UInt WordAdd, UChar *DataBuf, UChar DataSize); // deprecated
//UChar LCPC_ExternalE2PTest(void); //deprecated
UChar LCPC_InternalWrite_Byte(UInt WordAdd, UChar DataByte);
UChar LCPC_InternalRead(UInt WordAdd, UChar *DataBuf, UChar DataSize);

// Temperature Sensor:
UChar LCPC_TempSensorInit();
//UChar LCPC_TempSensorTest(UChar IOEX_SLA);
UChar LCPC_TempSensorReadTemp( SChar* Temperature);
UChar LCPC_CheckTemp(SChar* Temperature, SChar SetPoint, UChar TempHyst);
UChar LCPC_Check( void ) ;
//
//// RAM:
////UChar LCPC_RAMCheck(UInt StartAddress); // deprecated
//
//// watchdog
void WDT_off(void);
void WDT_Prescaler_Change(int watchdogtimeout);

#endif // !__LCPC_H_

//***********************************************************************************
// No More
//***********************************************************************************
