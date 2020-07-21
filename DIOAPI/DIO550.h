/***********************************************************************************
*  File     : DIO550.h
*----------------------------------------------------------------------------------
*  Object   : it defines all DIO550-related constants
*
*  Contents : DIO550 related constants, function prototypes, ...
*
* ---------------------------------------------------------------------------------
*  Date     |	Description
* ----------+----------------------------------------------------------------------

* ----------+----------------------------------------------------------------------
***********************************************************************************/

#ifndef __DIO550_H_
#define __DIO550_H_

/*************************************************************************************
*       Header Files                    
**************************************************************************************/

#include "CAN.h"


/*************************************************************************************
*           DI530 Card registres & Definitions
**************************************************************************************/
//#define MAX_CHANNEL	    32		// Maximum number of output channels
//#define MAX_GROUP       (MAX_CHANNEL/8)

#define REG_DATA0	        0x00	// Inputs 1 to 8 status
#define REG_DATA1	        0x01	// Inputs 9 to 16 status
#define REG_DATA2	        0x02	// Inputs 17 to 24 status
#define REG_DATA3	        0x03	// Inputs 25 to 32 status
#define REG_LO_SW		    0x06	// Low switch register
#define REG_HI_SW		    0x07	// High switch register
#define REG_FIELD_POWER		0x08	// Field Power Status Register

#define FIELD_POWER_1	    0x01	// First Field Power
#define FIELD_POWER_2	    0x02	// Second Field Power
#define FIELD_POWER_3	    0x04	// Third Field Power
#define FIELD_POWER_4	    0x08	// Fourth Field Power
/*************************************************************************************
*           DI531 Configuration
**************************************************************************************/
#define Debounce
/***************************************************************************************
*       DI530 Structures
**************************************************************************************/
union DataUnion
{
    ULong D32;
    UChar D8[4];
};
union DataUnion2
{
    UInt D16;
    UChar D8[2];
};
union IOErrorUnion
{
    struct ErrorBit
    {
        UChar TemperatureSensor:1;          // Temperature sensor is not accessible (if == 1)
        UChar res0:1;                  		// reserved
        UChar FieldPower1:1;                // Field power1 is invalid (if == 1)
        UChar FieldPower2:1;                // Field power2 is invalid (if == 1)
        UChar FieldPower3:1;                // Field power3 is invalid (if == 1)
        UChar FieldPower4:1;                // Field power4 is invalid (if == 1)
        UChar Res1:1;
        UChar Res2:1;
    } Bit;
    UChar D8;
};
union IOWarningUnion
{
    struct WarningBit
    {
        UChar Overload1:1;
        UChar Overload2:1;
        UChar Overload3:1;
        UChar Overload4:1;
        UChar Res4:1;
        UChar Res5:1;
        UChar Res6:1;
        UChar Res7:1;
    } Bit;
    UChar D8;
};

struct IOCardStruct
{	
    union IOErrorUnion   IOError,   PreviousIOError;
    union IOWarningUnion IOWarning, PreviousIOWarning;

    union DataUnion DataIn ;
    union DataUnion DataOut ;
    union DataUnion Inputs, LastInputs;
    union DataUnion Output;     		// Actual output value
    
    UChar PulseStretchingTimer[4];	// Counts up till reaches pulse stretching value
    UChar DataIndex;
    UChar Timer4SampleCount;
};

union SettingUnion 
{ 
    struct SettingStruct
    {
        struct GenericSettingStruct Generic ;
        //UChar Reserved1[7];      // Reserved
        UChar Filter[4];
        //UChar Reserved2[16];      // Reserved
        union DataUnion2 IsActive;      
        union DataUnion2 IsSafe;        
        union DataUnion2 SafeValue; 
        //UChar Reserved3[14];      // Reserved
	}Bit;
    UChar D8[sizeof( struct SettingStruct )];
};

UChar IOCard_Init(void);
void  CheckIODiagnostics(void);
UChar IOCard_Check(void);
void  ApplyNewSetting(void);
/**************************************************************************************
*           DI530 Functions
**************************************************************************************/
//UChar   ReadRegister( UChar Reg ); // deprecated
//void    WriteRegister( UChar Reg, UChar Data); // deprecated
//UChar   CheckCPLD(void);				// deprecated
void    CaptureInputs(void);
//UChar   AutoTestRoutine(void);
//void 	SetTimer3( void );
void 	SetTimer4( void );
/**************************************************************************************
*       DO580 Functions
***************************************************************************************/
UChar ReadDOStatus( void );
UChar ReadFieldPowerStatus( void );
void  EnableOutputs();
void  DisableOutputs();
void  SetOutputs();
void  CalcTimer4Value(UChar SampleCount);
void signalreceive(void);
#endif

//***********************************************************************************
// No More
//***********************************************************************************