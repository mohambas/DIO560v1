/***********************************************************************************
*  File     : DI530.c
*----------------------------------------------------------------------------------
*  Object   : it defines all DI-related variables and functions
*
*  Contents : IOCard_Init, IOCard_Check, ApplyNewSetting, ProcessMessage, ...
*
* ---------------------------------------------------------------------------------
*  Date     |	Description
* ----------+----------------------------------------------------------------------
*  93.07.09 | 	Hardware Phase 3, final revision 
* ----------+----------------------------------------------------------------------
*  93.06.29 | 	comments update 
* ----------+----------------------------------------------------------------------
*  93.06.04 | 	Review comments incorporated 
* ----------+----------------------------------------------------------------------
***********************************************************************************/

/***********************************************************************************
*  General Header files
***********************************************************************************/
#include "Config.h"
#include "CAN.h"
#include "Common.h"
#include "DIO550.h"
#include "WDManager.h"
#include "GeneralFunctions.h"
#include <sys/time.h>
#include <signal.h>

#if ( CARD_TYPE == IOTYPE_DIO550 )

/***********************************************************************************
*  Specific Header file
***********************************************************************************/


/***********************************************************************************
*  Local variables
***********************************************************************************/
struct IOCardStruct    IOCard;
union  SettingUnion    Setting;
ULong LastDataValue;
UChar Samplecount=1;
extern int deviceHandle;
extern int watchdogEnable;    	// WDT is Disabled by default
//	All the strings should be kept in flash memory:
Char msgFPOW1Detect[] 	            = "\r INFO:  Field-Power-1 OK.\n";
Char msgFPOW2Detect[] 	            = "\r INFO:  Field-Power-2 OK.\n";
Char msgFPOW3Detect[] 	            = "\r INFO:  Field-Power-3 OK.\n";
Char msgFPOW4Detect[] 	            = "\r INFO:  Field-Power-4 OK.\n";
Char errFPOW1NotDetect[]             = "\r ERROR: NO Field-Power-1.\n";
Char errFPOW2NotDetect[]             = "\r ERROR: NO Field-Power-2.\n";
Char errFPOW3NotDetect[]             = "\r ERROR: NO Field-Power-3.\n";
Char errFPOW4NotDetect[]             = "\r ERROR: NO Field-Power-4.\n";

Char msgAutoTestBypassOK[] 	        = "\r INFO:  DI Auto Test Routine ByPassed!! \n";
Char msgAutoTestPassOK[] 	        = "\r INFO:  DI Auto Test Routine Passed Successfully \n";
Char errAutoTestNAK[] 	            = "\r ERROR: DI Auto Test Routine Failed \n";

///*************************************************************************************
//*  Function   : ReadRegister
//*------------------------------------------------------------------------------------
//*  Object     : This function reads from registers of the DI530 card.
//*
//*  Return     : Read data form the register
//*
//*  Note(s)    : 6 registers are located in for consequent places, from address 00 to 05H.
//*			    : deprecated
//*------------------------------------------------------------------------------------
//*  Input Arguments |  Description
//*------------------+-----------------------------------------------------------------
//*  Reg			   |  The address(name) of register
//*------------------+-----------------------------------------------------------------
//*************************************************************************************/
//UChar ReadRegister( UChar Reg )
//{
//	// Offset value guarantees A15 (DI CPLD CS signal after inversion) is high
//	UChar *DI530_Pointer = (UChar *) (IO_OFFSET);         // Put the offset address to pointer
//	UChar data;
//	
//	data = *(DI530_Pointer + Reg);                        // Add the register address
//	return (data);                                        // Return the value
//}
//
///*************************************************************************************
//*  Function   : WriteRegister
//*------------------------------------------------------------------------------------
//*  Object     : This function writes to registers of the DI101 card.
//*
//*  Return     : None.
//*
//*  Note(s)    : deprecated
//*
//*------------------------------------------------------------------------------------
//*  Input Arguments |  Description
//*------------------+-----------------------------------------------------------------
//*  Reg			   |  The address(name) of register
//*------------------+-----------------------------------------------------------------
//*  Data			   |  The data must be wrote to the register.
//*------------------+-----------------------------------------------------------------
//*************************************************************************************/
//void WriteRegister( UChar Reg, UChar Data )
//{
//	// Offset value guarantees A15 (DI CPLD CS signal after inversion) is high
//	UChar *DI530_Pointer = (UChar *) (IO_OFFSET);     // Put the offset address to pointer
//	
//	*(DI530_Pointer + Reg) = Data;                    // Write the value to the address (offset + register)
//}
//
///*************************************************************************************
//*  Function   : CheckCPLD
//*------------------------------------------------------------------------------------
//*  Object     : to Write a byte to CPLD and reads it to check functionality of CPLD.
//*
//*  Return     : RET_SUCCESS		Test is done successfully
//*               ERR_TESTFAILED	Error detected.
//*
//* Note(s)     : We check four test values: 0xff, 0x00, 0xaa & 0x55 to find problems in 
//*               CPLD or data bus.
//*				  deprecated
//*------------------------------------------------------------------------------------
//*  Input Arguments |  Description
//*------------------+-----------------------------------------------------------------
//*  None			   |  ---
//*------------------+-----------------------------------------------------------------
//*************************************************************************************/
//UChar CheckCPLD(void)
//{
//	UChar RetVal;						        // Keeps the return value
//	
//	WriteRegister(REG_TEST, 0xff);			// Write a data byte to Test register
//	if(ReadRegister(REG_TEST) != 0xff)		// If the read value doesn't match the written value
//		RetVal = ERR_TESTFAILED;		        // Error detected
//	WriteRegister(REG_TEST, 0x00);			// Write a data byte to Test register
//	if(ReadRegister(REG_TEST) != 0x00)		// If the read value doesn't match the written value
//		RetVal = ERR_TESTFAILED;		        // Error detected
//	WriteRegister(REG_TEST, 0x55);			// Write a data byte to Test register
//	if(ReadRegister(REG_TEST) != 0x55)		// If the read value doesn't match the written value
//		RetVal = ERR_TESTFAILED;		        // Error detected
//	WriteRegister(REG_TEST, 0xaa);			// Write a data byte to Test register
//	if(ReadRegister(REG_TEST) != 0xaa)		// If the read value doesn't match the written value
//		RetVal = ERR_TESTFAILED;		        // Error detected
//	
//	return RetVal;
//}

/*************************************************************************************
*  Function   : IOCard_Init
*------------------------------------------------------------------------------------
*  Object     : This function initializes the IOCard modules and defines appropriate IOs
*               on the Microcontroller to manage IO card's modules
*
*  Return     : RET_SUCCESS		Card initialized successfully
*       		ERR_TESTFAILED	Card initialization faced error
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar IOCard_Init(void)
{
	UChar RetVal;
    
	RetVal = RET_SUCCESS;	// Default return value

//	Pin Definition
    GROUP1_SELECT_IN_DEF;
    GROUP2_SELECT_IN_DEF;
//    GROUP3_SELECT_IN_DEF;
//    GROUP4_SELECT_IN_DEF;
//    
//    SIDE_SWITCH_CS_DEF;
//    SIDE_SWITCH_OE_DEF;    
  
    FIELD_POWER1_DEF;
    FIELD_POWER2_DEF;
    FIELD_POWER3_DEF;
    FIELD_POWER4_DEF;
 

	
	// Initialize global variables and flags:
    IOCard.Inputs.D32       = 0 ;
    IOCard.LastInputs.D32   = 0 ;

	//  NOTE-------------NOTE--------------NOTE: deprecated	
/*	// Test the CPLD:
	tmpVar = CheckCPLD();						// Check the CPLD
	if( tmpVar == ERR_TESTFAILED )
    {
		RetVal = ERR_TESTFAILED;
		IOCard.IOError.Bit.CPLDError = 1;	// Set the CPLD error indicator		
	}
	else
    {
		IOCard.IOError.Bit.CPLDError = 0;	// Set the CPLD error indicator
	}	
*/	
    if(watchdogEnable == WDT_ENABLE)
        feedWD(deviceHandle);
//	
    RetVal = IOCard_Check() ;
    if( RetVal  == RET_SUCCESS )
    {
	    if (watchdogEnable == WDT_ENABLE)
		    feedWD(deviceHandle);
    }
//	
#ifdef AUTO_TEST_ROUTINE_ENABLED
    if( AutoTestRoutine() != RET_SUCCESS )
    {
	    if (watchdogEnable == WDT_ENABLE)
		    feedWD(deviceHandle);    // TODO : Report And Blink the Fault LED for 5 Seconds
    }
#else
    printf( "%s",msgAutoTestBypassOK );
#endif
    
    IOCard.IOError.Bit.FieldPower1 = 0;
    IOCard.IOError.Bit.FieldPower2 = 0;
    IOCard.IOError.Bit.FieldPower3 = 0;
    IOCard.IOError.Bit.FieldPower4 = 0;
    
    //RetVal = RET_SUCCESS;					        // Default return value
    if(RetVal!=RET_SUCCESS)
	return RetVal;////
	// Define OE1 and OE2 IO pins:
    OUTPUT_ENABLE_DEF;
//    OUTPUT_DATA_BYTE_DEF;
    FIELD_POWER1_DEF;
    FIELD_POWER2_DEF;
    FIELD_POWER3_DEF;
    FIELD_POWER4_DEF;
    STATUS1_DEF;
//    STATUS2_DEF;
    IOCard.DataOut.D32 = 0;
    IOCard.Output.D32 = 0;
    
    DisableOutputs();
	
    RetVal = IOCard_Check() ;
    if( RetVal  == RET_SUCCESS )
    {
	    if (watchdogEnable == WDT_ENABLE)
		    feedWD(deviceHandle);
    }  
    
	return RetVal;
}
/*************************************************************************************
*  Function   : CheckIODiagnostics
*------------------------------------------------------------------------------------
*  Object     : This function checks the diagnostics for the DO580 card and fills the
*				specified sructure members.
*
*  Return     : None.
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void CheckIODiagnostics(void)
{
	UChar i, isSet;												
	UInt temp;												
	
	//Check OverCurrent Registers
	temp = 0;		                    // Reset the temp variable to 0
    temp = ReadDOStatus();              // Status of the higher 8 outputs
    
	for( i = 0; i < 2; i++ )
    {
		isSet = (temp >> (2 * i)) & 0x01;   // NOTE : If OverCurrent detected [2i] and if OverTemp detected [ 2i+1 ]
        
        switch( i )
        {
		  case 0:
			IOCard.IOWarning.Bit.Overload1 = isSet ;
            break ;
		  case 1:
			IOCard.IOWarning.Bit.Overload2 = isSet ;
            break ;

        }
     }
}
/*************************************************************************************
*  Function   : ReadDOStatus
*------------------------------------------------------------------------------------
*  Object     : This function Reads the Over Current diagnostics 
*
*  Return     : read status
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar ReadDOStatus( void )
{
    UChar Temp = 0;
	
    // OC: Over Current
    // Status Byte : [--| OC3 | --| OC2 | -- | OC1 |-- | OC0 ]
    
    if( READ_STATUS1 != 0 )
        Temp |= 0x01;
//    if( READ_STATUS2 != 0 )
//        Temp |= 0x04;

	
    return Temp;
}
/*************************************************************************************
*  Function   : IOCard_Check
*------------------------------------------------------------------------------------
*  Object     : To check IOCard and return a status
*
*  Return     : RET_SUCCESS		Card checked successfully
*       		ERR_TESTFAILED	Card check faced error
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar IOCard_Check( void )
{
    UChar RetVal;
    RetVal = RET_SUCCESS;

	//  NOTE-------------NOTE--------------NOTE: deprecated	
    // // Check the FPOWs (Field powers):
	//    tmpVar = ReadRegister( REG_FIELD_POWER );			        // Read the field power
	    
    IOCard.IOError.Bit.FieldPower1 = 0;
    IOCard.IOError.Bit.FieldPower2 = 0;
    IOCard.IOError.Bit.FieldPower3 = 0;
    IOCard.IOError.Bit.FieldPower4 = 0;
	
    if(( FIELD_POWER1_READ ) == 0)
        IOCard.IOError.Bit.FieldPower1 = 1;
    if(( FIELD_POWER2_READ ) == 0)
        IOCard.IOError.Bit.FieldPower2 = 1;
    if(( FIELD_POWER3_READ ) == 0)
        IOCard.IOError.Bit.FieldPower3 = 1;
    if(( FIELD_POWER4_READ ) == 0)
        IOCard.IOError.Bit.FieldPower4 = 1;
	    
    if( Card.HasDiag == 0 )
    {
        if(IOCard.IOError.D8 != IOCard.PreviousIOError.D8)
        {
            IOCard.PreviousIOError.D8 = IOCard.IOError.D8; 		// Make a backup of the last errors
            Card.HasDiag = 1; 				// Set Global Error
        }
		
        if(IOCard.IOWarning.D8 != IOCard.PreviousIOWarning.D8)
        {
            IOCard.PreviousIOWarning.D8 = IOCard.IOWarning.D8; 	// Make a backup of the last warning
            Card.HasDiag = 1; 				// Set Global Error
        }
    }
	
    // Check card diagnostics:
    CheckIODiagnostics();
	
    if( Card.HasDiag == 0 )
    {
        if(IOCard.IOError.D8 != IOCard.PreviousIOError.D8)
        {
            IOCard.PreviousIOError.D8 = IOCard.IOError.D8; 		// Make a backup of the last errors
            
			// FOR DEBUG ONLY: check out IOError, if there is any
			// printf("\r Error = 0x%x \n", IOCard.PreviousIOError.D8);
            Card.HasDiag = 1; 				// Set Global Error
        }
        if(IOCard.IOWarning.D8 != IOCard.PreviousIOWarning.D8)
        {
            IOCard.PreviousIOWarning.D8 = IOCard.IOWarning.D8; 	// Make a backup of the last warning
            Card.HasDiag = 1; 				// Set Global Error
        }
    }
	
	return RetVal;  
}
/*************************************************************************************
*  Function   : ReadFieldPowerStatus
*------------------------------------------------------------------------------------
*  Object     : This function Reads field power diagnostics 
*
*  Return     : read status
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar ReadFieldPowerStatus(void)
{
	UChar Temp = 0;
	
	if (FIELD_POWER1_READ != 0)
		Temp |= 0x01;
	if (FIELD_POWER2_READ != 0)
		Temp |= 0x02;
	if (FIELD_POWER3_READ != 0)
		Temp |= 0x04;
	if (FIELD_POWER4_READ != 0)
		Temp |= 0x08;
	
	return Temp;
}

/***********************************************************************************
*  Function   : EnableOutputs ()
*----------------------------------------------------------------------------------
*  Object     : Call this function to enable DO outputs.
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
***********************************************************************************/
void EnableOutputs()
{
//  GROUP1_SELECT_IN_HIGH;
//  GROUP2_SELECT_IN_HIGH;
//  GROUP3_SELECT_IN_HIGH;
//  GROUP4_SELECT_IN_HIGH;
//  
//  OUTPUT_DATA_BYTE_DEF;
//  
  OUTPUT_ENABLE;
}

/***********************************************************************************
*  Function   : DisableOutputs ()
*----------------------------------------------------------------------------------
*  Object     : Call this function to disable DO outputs.
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
***********************************************************************************/
void DisableOutputs()
{
    OUTPUT_DISABLE;
}

/*************************************************************************************
*  Function   : SetOutputs
*------------------------------------------------------------------------------------
*  Object     : This function sets the outputs according to outputs flags. If the latch 
*				flag is active, puts latched data on the outputs and if not, it sets 
* 				normal outputs.
*
*  Return     : None.
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void SetOutputs()
{
    if( ( Card.State == MAIN) || ( Card.State == BACKUP) ) // NOTE : Both BACKUP and MAIN Cards should enter this routine
        IOCard.Output.D32 = Setting.Bit.IsActive.D16 & IOCard.DataOut.D32 ;	// Set the output value due to the output mask
    else if( ( Card.State == FAIL_SAFE_MAIN ) || ( Card.State == FAIL_SAFE_BACKUP ) )
        IOCard.Output.D32 = ( Setting.Bit.IsActive.D16 & Setting.Bit.IsSafe.D16 ) & Setting.Bit.SafeValue.D16 ;
    else 
        IOCard.Output.D32 = 0 ;

    //printf("out=%d , dataO=%d  , active=%d",IOCard.Output.D32,Setting.IsActive.D16,IOCard.DataOut.D32);
    GROUP1_SELECT_IN_HIGH;
    GROUP2_SELECT_IN_HIGH;
//    GROUP3_SELECT_IN_HIGH;
//    GROUP4_SELECT_IN_HIGH;
    BOARD_ADRRESS_1_HIGH;
    BOARD_ADRRESS_2_HIGH;
    
	OUTPUT_DATA_BYTE_DEF;
    EnableOutputs();
    DataBusSelect( GROUP1_OUT_SELECT);
    delayMicroseconds (1);
	SetOUTData(~(IOCard.Output.D8[0] & 0x1F));
    delayMicroseconds (1);
	if (watchdogEnable == WDT_ENABLE)
		feedWD(deviceHandle);  
    DataBusSelect( DESELECT_ALL );
    
    DataBusSelect( GROUP2_OUT_SELECT);         
    delayMicroseconds (1);
	SetOUTData(~((((IOCard.Output.D8[1]) & 0x03) << 3) | ((IOCard.Output.D8[0] & 0xE0) >> 5))) ;
    delayMicroseconds (1);
	if (watchdogEnable == WDT_ENABLE)
		feedWD(deviceHandle);
    DataBusSelect( DESELECT_ALL );

}

/*************************************************************************************
*  Function   : CaptureInputs
*------------------------------------------------------------------------------------
*  Object     : Call this function to capture any changes of the filtered input data
*               in the Input Queue.
*
*  Return     : None.
*
*  Note(s)    : Every time this function is called all the input channels are scanned. if
*               the input value has been detected for the "FilterSize" times, One or Zero,
*               so the filter result will be One or Zero. If the filter result value is 
*               different from the previous captured value in the queue, so the new value 
*               will be placed in a new cell of the queue.
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void CaptureInputs(void)
{

  ULong i,DataBuffer[5];
  ULong AndValue=0xFFFFFFFF,OrValue=0,XorValue=0,NewDataValue=0; 
  INPUT_DATA_BYTE_DEF; 
  for(i=0;i<Samplecount;i++)
  {
//    GROUP4_SELECT_IN_HIGH;
//    GROUP3_SELECT_IN_HIGH;
    GROUP2_SELECT_IN_HIGH;       
    GROUP1_SELECT_IN_LOW;
    delayMicroseconds (1);
    IOCard.Inputs.D8[0] = READ_INPUT_DATA_BYTE;
	
//    GROUP4_SELECT_IN_HIGH;
//    GROUP3_SELECT_IN_HIGH;
    GROUP1_SELECT_IN_HIGH;   
    GROUP2_SELECT_IN_LOW;
    delayMicroseconds (1);
    IOCard.Inputs.D8[1] = READ_INPUT_DATA_BYTE;
	
//    GROUP4_SELECT_IN_HIGH;
//    GROUP2_SELECT_IN_HIGH;
//    GROUP1_SELECT_IN_HIGH;    
////    GROUP3_SELECT_IN_LOW;
//    delayMicroseconds (1);
//    IOCard.Inputs.D8[2] = READ_INPUT_DATA_BYTE;
	  
//for test
	  IOCard.Inputs.D8[2] = 0;
	  
//    GROUP3_SELECT_IN_HIGH;
//    GROUP2_SELECT_IN_HIGH;
//    GROUP1_SELECT_IN_HIGH;    
////    GROUP4_SELECT_IN_LOW;
//    delayMicroseconds (1);
//    IOCard.Inputs.D8[3] = READ_INPUT_DATA_BYTE;    
	  
//for test
    IOCard.Inputs.D8[3] = 0;
	  
    DataBuffer[i]=IOCard.Inputs.D32;
  }
  //AndValue=0xFFFFFFFF;
#ifdef Debounce
  for(i=0;i<Samplecount;i++)
  {
   AndValue=AndValue&DataBuffer[i];
  }
  for(i=0;i<Samplecount;i++)
  {
   OrValue=OrValue|DataBuffer[i];
  }
  XorValue=AndValue ^ OrValue;
  NewDataValue=(LastDataValue&XorValue)|( DataBuffer[Samplecount-1]&(~XorValue));
  LastDataValue=NewDataValue;
  IOCard.DataIn.D32=((ULong)(IOCard.Inputs.D8[3]&0x1F)<<15)|((ULong)(IOCard.Inputs.D8[2]&0x1F)<<10)|((ULong)(IOCard.Inputs.D8[1]&0x1F)<<5)|(ULong)(IOCard.Inputs.D8[0]&0x1F);
#else
  IOCard.DataIn.D32=((ULong)(IOCard.Inputs.D8[3]&0x1F)<<15)|((ULong)(IOCard.Inputs.D8[2]&0x1F)<<10)|((ULong)(IOCard.Inputs.D8[1]&0x1F)<<5)|(ULong)(IOCard.Inputs.D8[0]&0x1F);
#endif
    //IOCard.Data.D32=IOCard.Inputs.D32;      
}

/*************************************************************************************
*  Function   : ApplyNewSetting
*------------------------------------------------------------------------------------
*  Object     : This function sends commands to the IOcard elements to set the recently
*				received settings.
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void ApplyNewSetting( void )
{
//	// FOR DEBUG ONLY: to check out setting.filter stretching time-out set by user
//    /*for( int i = 0; i < MAX_GROUP; i++ )
//        printf( "\r Filter[%d] = 0x%X\n", i, Setting.Filter[i] );
//    
//    for( int i = 0; i < MAX_CHANNEL; i++ )
//        IOCard.PulseStretchingTimer[i] = Setting.Filter[ i/8 ];*/
//
    for( int i = 0; i < 4; i++ )
    {
//		FOR DEBUG ONLY: to check out issafe, isactive, ... parameters of setting
		printf( "\r IsActive[%d] = 0x%X\n", i, Setting.Bit.IsActive.D8[i] ) ;
        printf( "\r IsSafe[%d] = 0x%X\n", i, Setting.Bit.IsSafe.D8[i] ) ;
        printf( "\r SafeValue[%d] = 0x%X\n", i, Setting.Bit.SafeValue.D8[i] ) ;
    }
//    //SetTimer3();// Init stretching timer
}

/*************************************************************************************
*  Function   : SetTimer3()
*------------------------------------------------------------------------------------
*  Object     : This function sets the timer3 registers for 125us pulse. It at first
*               disable it, and refresh its register's values.
*
*  Return     : None. 
*
*  Note(s)    : Has been called during DI530 Initialization
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void SetTimer3( void )
{
//	//UChar i;
//	
//	DISABLE_TIMER3_INTERRUPT;
//
//	//=======  Timer 3 Setting for 10 uSec overflow interrupt	
//	SET_TIMER3A_MODE;
//	SET_TIMER3B_MODE;  // Set the clock to CLKio/8 (8 MHz/ 8 = 1 MHz (1 uSec))
//
//	SET_TIMER3H_INIT;
//	SET_TIMER3L_INIT;  // 10 usec (10 * 1usec = 10 usec); Up counting
//
//	//=================================================
//	/*for (i = 0; i < MAX_CHANNEL; i++)
//    {
//        IOCard.PulseStretchingTimer[i] = Setting.Filter[ i/8 ];
//	}	*/
//	
//	ENABLE_TIMER3_INTERRUPT;
}


/*************************************************************************************
*  Interrupt  : Timer3_OVHandler
*----------------------------------------------------------------------------------
*  Object     : This function is the ISR of the timer3 interrupt. This ISR is called 
*               every 125 us if the Timer3 is enabled.
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
//#pragma vector = TIMER3_OVF_vect
//__interrupt void Timer3_OVHandler(void)
//{
//	//UChar i;
//			        
//	SET_TIMER3H_VALUE;
//	SET_TIMER3L_VALUE;		// 125*1uS Refresh
//	//RunCycle();
//	/*for( i = 0; i < MAX_CHANNEL; i++ )
//    {
//		if( IOCard.PulseStretchingTimer[i] < Setting.Filter[ i/8 ]  )
//			IOCard.PulseStretchingTimer[i]++;
//	}	*/
//}
/*************************************************************************************
*  Function   : SetTimer4()
*------------------------------------------------------------------------------------
*  Object     : This function sets the timer4 registers for 500us pulse. It at first
*               disable it, and refresh its register's values.
*
*  Return     : None. 
*
*  Note(s)    : Has been called during DI530 Initialization
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void SetTimer4( void )
{
	//UChar i;
	
//	DISABLE_TIMER4_INTERRUPT;
//
//	//=======  Timer 4 Setting for 10 uSec overflow interrupt	
//	SET_TIMER4A_MODE;
//	SET_TIMER4B_MODE;  // Set the clock to CLKio/8 (8 MHz/ 8 = 1 MHz (1 uSec))
//
//	//SET_TIMER4H_INIT;
//	//SET_TIMER4L_INIT;  // 10 usec (10 * 1usec = 10 usec); Up counting
//
//	
//	ENABLE_TIMER4_INTERRUPT;
}


/*************************************************************************************
*  Interrupt  : Timer4_OVHandler
*----------------------------------------------------------------------------------
*  Object     : This function is the ISR of the timer4 interrupt. This ISR is called 
*               every 250 us if the Timer3 is enabled.
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/

void Timer4_OVHandler(void)
{
              CalcTimer4Value(IOCard.Timer4SampleCount);
              CaptureInputs();  
        /*if( Card.RxReady == 1 )
        {	
            SET_CAN_LED_BLINK;
			
            DoOnRxReady( CAN_RxMsg.Command, CAN_RxMsg.DataLen ) ;
            Card.RxReady = 0;
        } */       
	
}
/*************************************************************************************
*  Function   : IsValidSetting
*------------------------------------------------------------------------------------
*  Object     : It Checks if settings are valid or not.
*
*  Return     : 1: if it is valid
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar IsValidSetting()
{
    return 1 ;
}

/***********************************************************************************
*  Function   : RunCycle()
*----------------------------------------------------------------------------------
*  Object     : Call this function to activate/disactivate outputs based on state
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
***********************************************************************************/
void RunCycle()
{
    switch( Card.State )
    {
		//=======================================================================================================================================            
		//      Idle            Idle            Idle            Idle            Idle            Idle            Idle            Idle            
		//=======================================================================================================================================            
	  case IDLE:
		//=======================================================================================================================================            
		//      Out-Of-Service          Out-Of-Service          Out-Of-Service          Out-Of-Service          Out-Of-Service
		//=======================================================================================================================================            
	  case OUT_OF_SERVICE:
		if(( Card.PreviousState != OUT_OF_SERVICE ) || ( Card.PreviousState != IDLE ))
			DisableOutputs() ;
        break;
		//=======================================================================================================================================            
		//      Main            Main            Main            Main            Main            
		//=======================================================================================================================================            
	  case MAIN:
		if( Card.PreviousState != MAIN )
			EnableOutputs() ;
		
        break ;
		
		//=======================================================================================================================================            
		//      Backup            Backup            Backup            Backup            Backup            
		//=======================================================================================================================================            
	  case BACKUP:
		if( Card.PreviousState != BACKUP )
		{
			Card.HotRedundancyRxTimer   = 0;
			//Card.HotRedundancyActive    = 0;
			EnableOutputs() ;         // NOTE : Both MAIN and BACKUP DO cards should apply voltage on output 
		}
        break ;
        
		//=======================================================================================================================================            
		//          FailSafe                FailSafe            FailSafe            FailSafe            FailSafe            FailSafe            
		//=======================================================================================================================================            
	  case FAIL_SAFE_MAIN:
		if( Card.PreviousState != FAIL_SAFE_MAIN )
		{
			EnableOutputs() ;
			SetOutputs() ;
		}
        break ;
		
	  case FAIL_SAFE_BACKUP:
		if( Card.PreviousState != FAIL_SAFE_BACKUP )
		{
			EnableOutputs() ;               // NOTE : Both MAIN and BACKUP DO cards should apply voltage on output
			SetOutputs() ;                  // NOTE : Both MAIN and BACKUP DO cards should apply voltage on output
		}
        break ;
    }
}

/*************************************************************************************
*  Function   : ProcessMessage
*------------------------------------------------------------------------------------
*  Object     : To preprocess received message (common for specific cards) & extract
 *              what to do for FCP_CMD_SET_GET_DATA command.
*
*  Return     : 0/1
*
*  Note(s)    : This function is called when then its flag is set in the CAN interrupt 
*				service routine and processes the  message that is received.
*-----------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  rxCommand	   |  received command
*------------------+-----------------------------------------------------------------
*  dataLen		   |  data length
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar ProcessMessage( UChar rxCommand, UChar dataLen )
{
	// Decode the command byte (Please refer to Farineh Internal CAN Protocol document to find more about the commands and responses)
    switch( rxCommand )	            
    {
	  case FCP_CMD_SET_GET_DATA:	// Send data	
                if(CAN_RxMsg.Group==0)//in
                {
		 if( Card.State == MAIN )
		 {
			CAN_TxMsg.Data[0] = IOCard.DataIn.D8[0];  // Put data       
			CAN_TxMsg.Data[1] = IOCard.DataIn.D8[1];
			CAN_TxMsg.Data[2] = IOCard.DataIn.D8[2];       
			CAN_TxMsg.Data[3] = IOCard.DataIn.D8[3];
			CAN_TxMsg.DataLen = 4;                       // Set the number of bytes of packet
			CAN_TxMsg.Command = FCP_CMD_SET_GET_DATA;    // Set the command
			
                        
			CAN_Send();
			return 1 ;
		 }
                }
                else if(CAN_RxMsg.Group==1)////out
                {
		 if( dataLen == 4 )
		 {
			if(( Card.State == MAIN ) || ( Card.State == BACKUP ))
			{
				IOCard.DataOut.D8[0] = CAN_RxMsg.Data[0];
				IOCard.DataOut.D8[1] = CAN_RxMsg.Data[1];
				IOCard.DataOut.D8[2] = CAN_RxMsg.Data[2];
				IOCard.DataOut.D8[3] = CAN_RxMsg.Data[3];
				
				return 1 ;
			}
		 }  
                }
        break;
    }
    
    return 0 ;
}

/*************************************************************************************
*  Function   : DoOnRxReady ()
*------------------------------------------------------------------------------------
*  Object     : what to do if it receives a command
*
*  Return     : None.
*
*  Note(s)    : This function is called when then its flag is set in the CAN interrupt 
*				service routine and performs the required action regarding the RX message. 
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  rxCommand	   |  received command
*------------------+-----------------------------------------------------------------
*  dataLen		   |  data length
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void DoOnRxReady( UChar rxCommand, UChar dataLen )
{
	// FOR DEBUG ONLY: to check out some of IOCard.Data values
	//    if( rxCommand == FCP_CMD_SET_GET_DATA )
	//       printf("\r Data = [%x %x %x %x]\n", IOCard.Data.D8[0], IOCard.Data.D8[1], IOCard.Data.D8[2], IOCard.Data.D8[3]);
      if( rxCommand == FCP_CMD_SET_GET_DATA )
    {
      if(CAN_RxMsg.Group==1)//output command
        SetOutputs() ;
    }
}

/*************************************************************************************
*  Function   : AutoTestRoutine
*------------------------------------------------------------------------------------
*  Object     : This function processes messages those are recieved in CAN interface.
*
*  Return     : RET_SUCCESS		
*       		ERR_TESTFAILED	
*  Note       :Hardware of Autotest doesn't assamble so if you want to discomment it you should change code of it
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
/*UChar AutoTestRoutine(void)
{
    UChar Data_Group1=0;
    UChar Data_Group2=0;
    UChar Data_Group3=0;
    UChar Data_Group4=0;
    UChar TestValue;
    UChar Result = 1;

    if(( FIELD_POWER1_READ ) == 0)
        printf(errFPOW1NotDetect);
    else
        printf( msgFPOW1Detect );
    if(( FIELD_POWER2_READ ) == 0)
        printf(errFPOW2NotDetect);
    else
        printf( msgFPOW2Detect );
    if(( FIELD_POWER3_READ ) == 0)
        printf(errFPOW3NotDetect);
    else
        printf( msgFPOW3Detect );
    if(( FIELD_POWER4_READ ) == 0)
        printf(errFPOW4NotDetect);
    else
        printf( msgFPOW4Detect );
    
    if( ( FIELD_POWER1_READ  == 0 ) || ( FIELD_POWER2_READ  == 0 ) || ( FIELD_POWER3_READ  == 0 ) || ( FIELD_POWER4_READ  == 0 ) )
        return RET_SUCCESS; // TODO : Run the Auto Test Routine for available groups
	
    BOARD_ADRRESS_1_HIGH;
    BOARD_ADRRESS_2_HIGH;    
    for ( UChar i = 0 ; i < 2 ; i++ )
    {
        if ( i == 0)
        {
            LOW_SIDE_SWITCH1_LOW;
            LOW_SIDE_SWITCH2_LOW;
            LOW_SIDE_SWITCH3_LOW;
            LOW_SIDE_SWITCH4_LOW;
            HIGH_SIDE_SWITCH1_HIGH;
            HIGH_SIDE_SWITCH2_HIGH;
            HIGH_SIDE_SWITCH3_HIGH;
            HIGH_SIDE_SWITCH4_HIGH;
            TestValue = 0xFF;
        }
        else
        {
            HIGH_SIDE_SWITCH1_LOW;
            HIGH_SIDE_SWITCH2_LOW;
            HIGH_SIDE_SWITCH3_LOW;
            HIGH_SIDE_SWITCH4_LOW;
            LOW_SIDE_SWITCH1_HIGH;
            LOW_SIDE_SWITCH2_HIGH;
            LOW_SIDE_SWITCH3_HIGH;
            LOW_SIDE_SWITCH4_HIGH;
            TestValue = 0x00;
        }
        
        GROUP2_SELECT_HIGH;
        GROUP3_SELECT_HIGH;
        GROUP4_SELECT_HIGH;
        GROUP1_SELECT_LOW;
        __delay_cycles(800000);
        __watchdog_reset();
        Data_Group1 = READ_INPUT_DATA_BYTE;
		
        GROUP1_SELECT_HIGH;
        GROUP3_SELECT_HIGH;
        GROUP4_SELECT_HIGH;
        GROUP2_SELECT_LOW;
        __delay_cycles(800000);
        __watchdog_reset();
        Data_Group2 = READ_INPUT_DATA_BYTE;
		
        GROUP1_SELECT_HIGH;
        GROUP2_SELECT_HIGH;
        GROUP4_SELECT_HIGH;
        GROUP3_SELECT_LOW;
        __delay_cycles(800000);
        __watchdog_reset();
        Data_Group3 = READ_INPUT_DATA_BYTE;
		
        GROUP1_SELECT_HIGH;
        GROUP2_SELECT_HIGH;
        GROUP3_SELECT_HIGH;
        GROUP4_SELECT_LOW;
        __delay_cycles(800000);
        __watchdog_reset();
        Data_Group4 = READ_INPUT_DATA_BYTE;
        
        if( (Data_Group1 != TestValue) || (Data_Group2 != TestValue) || (Data_Group3 != TestValue) || (Data_Group4 != TestValue) )
        {
			printf( errAutoTestNAK );

			// FOR DEBUG ONLY: to check out the result of auto test routine fail
            // printf("\r DI Auto Test Routine Failed for {%x} Group0=%x Group1=%x Group2=%x Group3=%x \n", TestValue, Data_Group1, Data_Group2, Data_Group3, Data_Group4);
            Result = 0;
            Card.IOFault = 1;
        }
    }
	
    if( Card.IOFault != 1 )
        printf(msgAutoTestPassOK);
	
    GROUP1_SELECT_HIGH;
    GROUP2_SELECT_HIGH;
    GROUP3_SELECT_HIGH;
    GROUP4_SELECT_HIGH;
    
    LOW_SIDE_SWITCH1_LOW;
    LOW_SIDE_SWITCH2_LOW;
    LOW_SIDE_SWITCH3_LOW;
    LOW_SIDE_SWITCH4_LOW;
    HIGH_SIDE_SWITCH1_LOW;
    HIGH_SIDE_SWITCH2_LOW;
    HIGH_SIDE_SWITCH3_LOW;
    HIGH_SIDE_SWITCH4_LOW;
	
    if(Result == 1)
        return RET_SUCCESS;
    else
        return ERR_TESTFAILED;
}*/
/*************************************************************************************
*  Function   : DataBusSelect
*------------------------------------------------------------------------------------
*  Object     : It Determines the Select pins on several latches to provide 
*				the apporpriate DATA (PORTL)
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void  DataBusSelect( UChar Select)
{
  GROUP1_SELECT_IN_HIGH;
  GROUP2_SELECT_IN_HIGH;
//  GROUP3_SELECT_IN_HIGH;
//  GROUP4_SELECT_IN_HIGH;  
    switch ( Select )
    {
	  case DESELECT_ALL:
		
		BOARD_ADRRESS_1_HIGH;
		BOARD_ADRRESS_2_HIGH;
		GROUP1_SELECT_OUT_LOW;
//		GROUP2_SELECT_OUT_LOW;                 
                
        break;
		
	  case READ_BOARD_ADDRESS1:
                
		BOARD_ADRRESS_2_HIGH;
		BOARD_ADRRESS_1_LOW;
		GROUP1_SELECT_OUT_LOW;
//		GROUP2_SELECT_OUT_LOW;
                
                
        
        break;
//        
	  case READ_BOARD_ADDRESS2:
                
		BOARD_ADRRESS_1_HIGH;
		BOARD_ADRRESS_2_LOW;
		GROUP1_SELECT_OUT_LOW;
//		GROUP2_SELECT_OUT_LOW;
                
                
        
        break;
		
	  case GROUP1_OUT_SELECT:
                
		BOARD_ADRRESS_1_HIGH;
		BOARD_ADRRESS_2_HIGH;
		GROUP1_SELECT_OUT_HIGH;
//		GROUP2_SELECT_OUT_LOW;
                
                 
        
        break;
		
//	  case GROUP2_OUT_SELECT:		
//                
//		BOARD_ADRRESS_1_HIGH;
//		BOARD_ADRRESS_2_HIGH;
//		GROUP1_SELECT_OUT_LOW;
//		GROUP2_SELECT_OUT_HIGH;
//                
//              
//        
//        break;
//		
	  case READ_OUTPUT_STATUS:              
	
		BOARD_ADRRESS_1_HIGH;
		BOARD_ADRRESS_2_HIGH;
                
        
        
        break;
        
    }
	
}

/*************************************************************************************
*  Function   : DefineSelectPins
*------------------------------------------------------------------------------------
*  Object     : it Defines the Select pins to be output at the initialization
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void  DefineSelectPins( void )
{
    BOARD_ADRRESS_1_DEF;        // ADE1 Signal as output
    BOARD_ADRRESS_2_DEF;        // ADE1 Signal as output
    OUTPUT_ENABLE_DEF;
    GROUP1_SELECT_IN_DEF;
    GROUP2_SELECT_IN_DEF;
//    GROUP3_SELECT_IN_DEF;
//    GROUP4_SELECT_IN_DEF;
    GROUP1_SELECT_OUT_DEF;
//    GROUP2_SELECT_OUT_DEF;    
}
/***********************************************************************************
*  Interrupt  : USART0_receive  
*----------------------------------------------------------------------------------
*  Object     : Rx interrupt USART0 subroutine
*
*  Return     : None
*
***********************************************************************************/
//#pragma vector = USART0_RX_vect
void signalreceive(void)
{ 
	UChar tmp;
	
//	tmp = USART0_DATA;
	switch( tmp )
	{	
        case CMD_CHANGE_MAPCS_VER:
                Card.MAPCS_VER=!Card.MAPCS_VER;
                LCPC_Write_Byte( 8*4, Card.MAPCS_VER );
                Check_Mapcs_Ver();            
                break;	
        case CMD_CHANGE_HOTREDUNDANCY:
                //if ( (CARD_TYPE == IOTYPE_AO672) || (CARD_TYPE == IOTYPE_AO673) )
                //{
                Card.HotRedundancyActive=!Card.HotRedundancyActive;
                LCPC_Write_Byte( (8*4)+1, Card.HotRedundancyActive );
                Check_HotRedundancyActive();
                //}
                break;
	    case CMD_CHANGE_MAPCS_VER_L:
		        Card.MAPCS_VER = !Card.MAPCS_VER;
		        LCPC_Write_Byte(8 * 4, Card.MAPCS_VER);
		        Check_Mapcs_Ver();            
		        break;	
	    case CMD_CHANGE_HOTREDUNDANCY_L:
		    //if ( (CARD_TYPE == IOTYPE_AO672) || (CARD_TYPE == IOTYPE_AO673) )
		        //{
		        Card.HotRedundancyActive = !Card.HotRedundancyActive;
		        LCPC_Write_Byte((8 * 4) + 1, Card.HotRedundancyActive);
		        Check_HotRedundancyActive();
		    //}
		        break;		
	      default:
		        printf("\n\r\nPlease press the right key: r,v,a,b,c,x,h,0-7\n");
		        break;           
	}
	
}

#endif
//***********************************************************************************
// No More
//***********************************************************************************