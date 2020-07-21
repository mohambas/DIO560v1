/***********************************************************************************
*  File      : LCPC.c
*----------------------------------------------------------------------------------
*  Object    : This file contains required functions for Initializing and 
*              handling LCPC card and the Microcontroller chip.
*
*  Contents  : interrupt routine, interrupt handlers, SPI init, and ...
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
#include "Common.h"
#include "IIC.h"
#include "CAN.h"
#include "WDManager.h"
#include <pthread.h>
#include <signal.h>
#include <sys/time.h>
#include <mcp23017.h>
#include "Hardware.h"
/***********************************************************************************
*  Local variables
***********************************************************************************/
struct PCCardStruct Card;
extern int watchdogEnable;
//extern int watchdogtimeout;
extern int deviceHandle;
Char msgStartInit[]          = "\r Initializing MAPCS LCPC ...\n";
Char msgFirmwareVersion[]    = "\r Firmware Version (Date) : 98-05-15 \n";

Char errBPAdd[]              = "\r ERROR: Reading Backplane address caused error.\n";
Char msgBPAdd[]              = "\r INFO:  Rack:%d Slot:%d \n";

Char errTypeMismatch[] 	    = "\r ERROR: Defined type and version doesn't match the hardware.\n";
Char errModuleTypeMismatch[] = "\r ERROR: Defined Module Type doesn't match the hardware for Card Type.\n";
Char msgTypeMatch[] 		   	= "\r INFO:  Defined and hardwired card type and version matched.\n";
Char errTypeVerErr[]  		= "\r ERROR: Reading IO Card Type & Version data caused error.\n";
Char errTypeVerFAIL[]  		= "\r ERROR: FAIL TYPE & Version \n";

Char msgEEPROMTypeInternal[]	= "\r INFO:  Type of E2PROM: Internal.\n";

//  NOTE-------------NOTE--------------NOTE: deprecated	
//Char msgEEPROMTypeExternal[]	= "\r INFO:  Type of E2PROM: External.\n"; 
//Char errE2PFailed[]         	= "\r ERROR: EEPROM test caused error.\n";
//Char msgE2POK[] 	           	= "\r INFO:  The EEPROM check is done successfully.\n";
//
//Char errRAMFailed[]         	= "\r ERROR: RAM check failed.\n";
//Char msgRAMOK[] 	           	= "\r INFO:  RAM check is done successfully.\n";

Char msgCardType[]          	= "\r INFO:  CARD TYPE ={ 0x%X 0x%X }";
Char msgLCPCType[]          	= "\r INFO:  LCPC Type = 0x%x \n";
Char msgtmpBuf1_2[]         	= "\r INFO:  tmpBuf1 = 0x%x  tmpBuf2 = 0x%x \n";
Char msgCardVerRev[]        	= "\r INFO:  VERSION ={ 0x%X } CARD REVISION ={ 0x%X } ";
Char msgModuleType[]        	= "\r INFO:  ModuleType ={ 0x%X }\n ";
Char msgCardTypeAOH[]       	= "\r >>>> CARD TYPE AOH <<< \n";
Char msgCardTypeAIH[]       	= "\r >>>> CARD TYPE AIH <<< \n";
Char msgFirmwareVer[]       	= "\r INFO:  FW Ver:0x%X, HW Ver:0x%X, CardType=0x%X\n\r\n";
Char msgCardTypeVer[]       	= "\r INFO:  CardType = 0x%x CardVersion = 0x%x \n";

Char msgLCTempSensInit[]     = "\r INFO:  LCPC Heat Sensor initialized.\n";
Char errIICBus[] 		    = "\r ERROR: IIC Bus not working!\n";
Char errLCTempSensNotRes[]   = "\r ERROR: LCPC Heat Sensor not responding!\n";
Char errTempSensTimeout[]    = "\r ERROR: LCPC Heat Sensor caused timeout error!\n";
Char errTempSensUnknown[]    = "\r ERROR: LCPC Heat Sensor initializing caused unknown error!\n";
Char errOverHeat[] 		    = "\r INFO:  LCPC OVERHEAT detected!\n";
Char errTempSensReadErr[]    = "\r ERROR: Reading from LCPC Heat Sensor caused error!\n";
Char errLCPCErrorWarning[]   = "\r LCPCError = %X LCPCWarning = %X \n";


/*************************************************************************************
*  Function   : LCPC_Init
*------------------------------------------------------------------------------------
*  Object     : This function Initializes LCPC modules.
*
*  Return     : result of initialization
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  CardType		   |  Type of IO Card, in format of IOTYPE_DI530, ...
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar LCPC_Init( UChar CardType )
{
    UChar RetVal = RET_SUCCESS;		// General variables
	
//---------------------------------------------------------------------------------
    Card.State                  = IDLE;  // First state
    Card.NewState               = IDLE;  // First state
    Card.PreviousState          = 0;
    Card.LCPCWarning.D8			= 0;
    Card.PreviousLCPCWarning.D8 = 0; 
    Card.LCPCError.D8  			= 0;
    Card.PreviousLCPCError.D8   = 0;
    Card.TemperatureLCPC        = 0;		        
    Card.FieldPowerStatus       = 0;	    
    Card.BPAddress              = 0xFF;
#ifdef IS_INPUT
    Card.Group                  = INPUT_GROUP;
#else
    Card.Group                  = OUTPUT_GROUP;			    
#endif    
#ifdef  IS_INOUT
    Card.Group                  = INOUT_GROUP;
#endif    

    Card.Type1                  = 0;			
    Card.Type2                  = 0;			
    Card.Version                = 0;			
    Card.Revision               = 0;			
    Card.ModuleType             = 0;		
    Card.Type                   = 0;				
    Card.FirmwareVersion        = 0;	
    Card.HardwareVersion        = 0;	
    Card.GeneralLED             = 0xFF;			
    Card.CAN_EFLG               = 0; 
    Card.HasDiag                = 0;
    Card.NewSettingReceived     = 0;
    Card.SizeOfSetting          = SIZE_OF_SETTING ;
    Card.SettingOffset          = 0;
    Card.IOFault                = 0;
    Card.IOFaultCounter         = 0;
	
    Card.IO_LED_Update_Flag     = false;
	
//    RESET_CAN_LED_BLINK;
	//----------------------------------------------------------------------------------    
	
    INPUT_DATA_BYTE_DEF;        // PORTL is input to read the board type version
    
    WDT_off();  //Disable Watchdog
    
    delayMicroseconds( 2000 ); 		// Delay 2sec for the reset signal duration and ADC reset process to be finished

	
    WDT_Prescaler_Change(1); //Enable Watchdog for 1sec 
	
			
	mcp23017Setup(PIN_Base_560, 0x20);      // Initialize MCP23017 IN IIC Bus

	
//    LCPC_InputInit(115200);		// Bash Input Data semaphore post
	
    
    printf("\r\n\r\n\r\n");
    printf("%s", msgStartInit );
    printf("\r\n\r\n");
	
    printf("%s",msgFirmwareVersion);

//  NOTE-------------NOTE--------------NOTE: deprecated	
//    // Initialize the LCPC IOExpander (LEDs & BPAddress):
//    LCPC_IOExpLEDAddInit();

    LCPC_DefBackplaneAddress();
    Card.BPAddress = LCPC_GetBackplaneAddress();    // Read the backplane address
    printf( msgBPAdd,((Card.BPAddress & 0x3C) >> 2) + 1, (Card.BPAddress & 0x3) + 1);
    Card.BPAddress = Card.BPAddress & 0x3F ;
       
	// Initialize the temperature sensor: 
	if(LCPC_TempSensorInit() != RET_SUCCESS)
	{
		Card.LCPCError.Bit.TemperatureSensor = 1; 		        // Set LCPC temperature sensor error bit    
		RetVal = ERR_TESTFAILED;
		
		// FOR DEBUG ONLY: another confirmation message for LCPC temperature sensor error
        // printf_P("\r FAIL LCPC TEMP SENSOR \n");
	}
	
	//--------------------------------------------    
#if ( (CARD_TYPE != IOTYPE_ANALYZER)  && (CARD_TYPE != IOTYPE_CALIBRATOR) )    

	//  NOTE-------------NOTE--------------NOTE: deprecated	
	/*
    // Initialize the IO Card's IOExpander (Card Type & Version):
    if( LCPC_IOExpTypeVerInit() != RET_SUCCESS )
    {
        Card.LCPCError.Bit.IOExpander    = 1;	    // Set LCPC IOExp. error bit
        RetVal = ERR_TESTFAILED;
        printf("\r FAIL LCPC IO EXP TYPE VER \n");
    }
	*/
	
    // Check the defined card type with hardwired one
    if( CheckCardTypeAndVersion( CardType ) != RET_SUCCESS )
    {
        RetVal = ERR_TESTFAILED;
        printf("%s",errTypeVerFAIL);
    }
#endif	

	//--------------------------------------------      
	printf("%s",msgEEPROMTypeInternal);    

//  NOTE-------------NOTE--------------NOTE: deprecated	
/*	
#if E2PROMType == 0
	printf(msgEEPROMTypeInternal);    
#else
	printf(msgEEPROMTypeExternal);
#endif
	
    // Test the EEPROM 
    if( LCPC_E2PROMTest() == ERR_TESTFAILED )
    {
        printf(errE2PFailed);
        Card.LCPCWarning.Bit.EEPROM = 1;	// Set the EEPROM warning bit
        RetVal = ERR_TESTFAILED;
    }
    else	
    {
        printf(msgE2POK);
    }
	*/
	
//    INTERRUPT_VECTOR_ENABLE;	// Write the Interrupt Vector Change Enable (IVCE) bit to one.      
//    INTERRUPT_SETTING_NORMAL;	// Within four cycles, write the desired value to IVSEL while writing a zero to IVCE.						

//  NOTE-------------NOTE--------------NOTE: deprecated	
/*  
	//Interrupt vectors are placed at the start of the Flash memory:
#if CARD_CODE == BOOTLOAD_CODE			
    INTERRUPT_VECTOR_ENABLE;		// Write the Interrupt Vector Change Enable (IVCE) bit to one.      
    INTERRUPT_SETTING_BOOTLOAD;     // Within four cycles, write the desired value to IVSEL while writing a zero to IVCE.			
#else
    INTERRUPT_VECTOR_ENABLE;	// Write the Interrupt Vector Change Enable (IVCE) bit to one.      
    INTERRUPT_SETTING_NORMAL;	// Within four cycles, write the desired value to IVSEL while writing a zero to IVCE.						
#endif	
*/    
    // The general interrupt enable bit is set
//    LCPC_EnableInterrupt();  				           

//  NOTE-------------NOTE--------------NOTE: deprecated	    
/* 
    // Enable external memory	
    LCPC_EnExternalMem;	
    
    // External RAM Check:
    if( LCPC_RAMCheck(EXTRAM_STARTADD) == ERR_TESTFAILED)
    {
        Card.LCPCWarning.Bit.ExternalRAM = 1;	// Set the external RAM error bit
        printf(errRAMFailed);
        RetVal = ERR_TESTFAILED;
    }
    else
    {
        Card.LCPCWarning.Bit.ExternalRAM = 0;	// Reset the external RAM error bit
        printf(msgRAMOK);
    }
*/
	
    Card.GeneralLED = 0xFF;			// Initial value for LEDs
    LCPC_SetLEDs( 0xFF );			// turn all the LEDs off
    
    LCPC_SetTimer2(); 				// General timer with 1 mSec interval  
    if( SYSTEM_RESET_VIA_WATCHDOG && !SYSTEM_POWER_ON_RESET ) 
        Card.LCPCWarning.Bit.ResetViaWatchdog = 1;
    
    Card.RxReady = 0 ;
    Card.Timer2_CounterA = 0 ;
    Card.HotRedundancyTxTimer = 0;
    Card.HotRedundancyRxTimer = 0;
    //Card.HotRedundancyActive    = 0;
    Card.HotRedundancyFailCounter   = 0;
    
    Setting.Bit.Generic.OverHeatLimit = 65 ;
    Setting.Bit.Generic.OverHeatBand = 5 ;
   
    return RetVal;
}

/*************************************************************************************
*  Function   : WDT_Prescaler_Change
*------------------------------------------------------------------------------------
*  Object     : To change watchdog timer prescaler
*
*  Return     : None.
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void WDT_Prescaler_Change(int watchdogtimeout)
{
	//    __disable_interrupt();
	//    __watchdog_reset();
	////	Start timed equence 
	//	WD_START;
	////	Set new prescaler(time-out) value = 64K cycles (~0.5 s) 
	//	WD_PRESCALER_SET;
	//	
	//    __enable_interrupt();
	setTimeoutWD(deviceHandle, watchdogtimeout);
}

/*************************************************************************************
*  Function   : WDT_off
*------------------------------------------------------------------------------------
*  Object     : To turn off watchdog timer
*
*  Return     : None.
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void WDT_off(void)
{
	watchdogEnable = 0;
}

/*************************************************************************************
*  Function   : LCPC_Translate_IICRetVal
*------------------------------------------------------------------------------------
*  Object     : a simple function just to make code easily readable
*
*  Return     : translated value of return.
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  RetVal		   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar LCPC_Translate_IICRetVal( UChar RetVal )
{
    // Translate RetVal:
    if( RetVal == IIC_SUCCESS )			
        return RET_SUCCESS;			    // Function is done successfully
    else if( RetVal == IICERR_START )		
        return ERR_IICBUS;		        // IIC bus error
    else if( RetVal == IICERR_SLAACK )	
        return ERR_NOTRESPOND;		    // Device not responding
    else if( RetVal == IICERR_TIMEOUT )	
        return ERR_TIMEOUT;			    // Timeout error
    else	
        return ERR_REPEAT;				// Error during execution of function
}

/*************************************************************************************
*  Function   : LCPC_SetLEDs
*------------------------------------------------------------------------------------
*  Object     : This function manages all the 5 LEDs on LCPC Card.
*
*  Return     : None.
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  LEDVal		   |  the value of five desired LEDs in 5 bits.
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void LCPC_SetLEDs(UChar LEDVal)
{
    //SET_LED1
    if ((LEDVal & 0x01))
        SET_LED1_HIGH;
    else
        SET_LED1_LOW;
    //SET_LED2
    if ((LEDVal & 0x02))
        SET_LED2_HIGH;
    else
        SET_LED2_LOW;
    //SET_LED3
    if ((LEDVal & 0x04))
        SET_LED3_HIGH;
    else
        SET_LED3_LOW;
    //SET_LED4
    if ((LEDVal & 0x08))
        SET_LED4_HIGH;
    else
        SET_LED4_LOW;
    //SET_LED5
//    if ((LEDVal & 0x10))
//        SET_LED5_HIGH;
//    else
//        SET_LED5_LOW;
}

/****************************************************************************************
*           IO CARD TYPE & VERSION 
****************************************************************************************/
/*************************************************************************************
*  Function   : CheckCardTypeAndVersion
*------------------------------------------------------------------------------------
*  Object     : To check Card Type and version
*
*  Return     : the result of check, successful or failed
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  CardType		   |  Type of IO Card, in format of IOTYPE_DI530, ...
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar CheckCardTypeAndVersion( UChar CardType )
{
    UChar tmpBuf1, tmpBuf2;		// General variables
    UChar tmpCardType = 0 ;
    UInt  CardCode = 0 ;
	
//    Card.LCPCType = LCPC_Get_Type();
//    printf(msgLCPCType, Card.LCPCType);
	
    if( LCPC_GetIOTypeVer( &tmpBuf1, &tmpBuf2 ) != RET_SUCCESS )		// Read card's type & version from hardware
    {
        printf("%s",errTypeVerErr);
        Card.LCPCError.Bit.TypeVer = 1;		 
        return ERR_TESTFAILED;	// Error detected during execution of function	
    }
	
    printf(msgtmpBuf1_2, tmpBuf1, tmpBuf2);
	// Data matched successfully
    Card.Type1      = CARD_TYPE1;       // Save card type1
    Card.Type2      = CARD_TYPE2;	    // Save card type2
    Card.Version    = CARD_VERSION;	    // Save card version
    Card.Revision   = CARD_REVISION;	// Save card revision
    Card.ModuleType = CARD_MODULE_TYPE;	// Save card module type
	
    printf(msgCardType, Card.Type1, Card.Type2);
    printf(msgCardVerRev, Card.Version, Card.Revision);
    printf(msgModuleType, Card.ModuleType);

    CardCode = ((Int)( (tmpBuf1 & 0x3F) << 8 ) | (Int)tmpBuf2 ) ;

    
    if ( CARD_CODE ==  AO673_CODE)
    {
        CardCode = AO673_CODE;
        printf("%s",msgCardTypeAOH);
    }
 
    if ( CARD_CODE ==  AI623_CODE)
    {
 
        CardCode = AI623_CODE;
        printf("%s",msgCardTypeAIH);
    }
	// ================================
  

    if( CARD_CODE != CardCode )
    {     // Check if the hardware type & version mismatch the defined software			
        printf("%s", errTypeMismatch );

        return ERR_TESTFAILED;	// Error detected during execution of function	
        
    }
  
    if( MODULETYPE != CARD_MODULE_TYPE )
    {
        printf("%s",errModuleTypeMismatch);
 
        return ERR_TESTFAILED;	// Error detected during execution of function	
    }

   
    if (((Int)(tmpBuf1 & 0x3F) << 8 | (Int)tmpBuf2) == 0x1c24)	//TI Cards
    {
        tmpCardType = IOTYPE_AI621;
    }
    else if( Card.Type1 == 0x00 )         //DO Cards 
    {
        if (Card.Type2 == 0x01)
        {
            if (Card.Version == 0x00)
                if ( Card.Revision == 0x01 || Card.Revision == 0x02 || Card.Revision == 0x03 || Card.Revision == 0x04)  
                    tmpCardType = IOTYPE_DO580;
        }
        else if (Card.Type2 == 0x02)
        {
            if (Card.Revision == 0x01 || Card.Revision == 0x02 || Card.Revision == 0x03)
                tmpCardType = IOTYPE_DO571;
        }
        else if (Card.Type2 == 0x03)
        {
          tmpCardType = IOTYPE_DO591;
        }
        
    }
    else if( Card.Type1 == 0x01 )//DI Cards //
    {         
        if (Card.Type2 == 0x01)
        {
            if (Card.Version == 0x00)
                if (Card.Revision == 0x01 || Card.Revision == 0x02 || Card.Revision == 0x03 || Card.Revision == 0x04 ) 
                {
                  if(CardTypeSelection==1)
                    tmpCardType = IOTYPE_DI530;
                  else if (CardTypeSelection==10)
                    tmpCardType = IOTYPE_DI531;
                }
        }
        else if (Card.Type2 == 0x07)
        {
            if (Card.Version == 0x00)
                if (Card.Revision == 0x00||Card.Revision == 0x01 || Card.Revision == 0x02 || Card.Revision == 0x03 || Card.Revision == 0x04 )  
                {
                  tmpCardType = IOTYPE_DIO550;
                }
        }          

    }
    else if( Card.Type1 == 0x02 )    //AO Cards
    {
		if (Card.Type2 == 0x02)
        {
            if (Card.Version == 0x00)
            {
                if ((Card.Revision == 0x01) || (Card.Revision == 0x02) || (Card.Revision == 0x03) || (Card.Revision == 0x04) || (Card.Revision == 0x05) || (Card.Revision == 0x06))
                {
                    if (Card.ModuleType == 0x01)
                        tmpCardType = IOTYPE_AO672;
                    else if (Card.ModuleType == 0x00)
                        tmpCardType = IOTYPE_AO673;
                }
            }
        }
    }
    else if( Card.Type1 == 0x3 ) //AI Cards
    {
	if (Card.Type2 == 0x03)
        {

	        if (Card.Version == 0x00)
	        {
		        if (Card.Revision == 0x01 || Card.Revision == 0x02 || Card.Revision == 0x03 || Card.Revision == 0x04 || Card.Revision == 0x05)
		        {
			        if (Card.ModuleType == 0x01)
				        tmpCardType = IOTYPE_AI622;
			        else if (Card.ModuleType == 0x00)
				        tmpCardType = IOTYPE_AI623;
		        }
	        }
        }
        else if (Card.Type2 == 0x04)
        {

            if (Card.Version == 0x00)
                if (Card.Revision == 0x01 || Card.Revision == 0x02 || Card.Revision == 0x03 || Card.Revision == 0x04)
                    tmpCardType = IOTYPE_AI621;
        }
	else if (Card.Type2 == 0x05)
        {
            if (Card.Version == 0x00)
                  if ( Card.Revision == 0x00 ||Card.Revision == 0x01 || Card.Revision == 0x02 || Card.Revision == 0x03 || Card.Revision == 0x04 || Card.Revision == 0x05)
                        tmpCardType = IOTYPE_AI624;                                             

           
          }        
      
    }
    // Modify firmware version and hardware version
    Card.FirmwareVersion = FIRMWARE_VERSION;
    Card.HardwareVersion = HARDWARE_VERSION;
    printf(msgFirmwareVer, FIRMWARE_VERSION, HARDWARE_VERSION, tmpCardType );
	
    //printf("%x %x",CardType,tmpCardType);
    if( CardType == tmpCardType )
    {
        Card.Type = tmpCardType;
        return RET_SUCCESS ;
    }
    printf("\r \n");
    return ERR_TESTFAILED;
}

/*************************************************************************************
*  Function   : LCPC_GetIOTypeVer
*------------------------------------------------------------------------------------
*  Object     : This function is called to read IO card's Type and version.
*
*  Return     : RET_SUCCESS			Function is done successfully
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  pCardType	   |  Will be filled with Card Type value
*------------------+-----------------------------------------------------------------
*  pCardType	   |  Will be filled with Card Version value
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar LCPC_GetIOTypeVer(UChar* pCardType, UChar* pCardVer)
{
    UChar RetVal = RET_SUCCESS;
    UChar TempValue1;
    UChar TempValue2;
    
   
    INPUT_DATA_BYTE_DEF;        // PORTL is input to read the board type version 
	
    DefineSelectPins();
    DataBusSelect(DESELECT_ALL);
    delayMicroseconds(8);
    
    DataBusSelect(READ_BOARD_ADDRESS1); // note note, board address1 is equal to tmpBuf2
    delayMicroseconds(8);
    TempValue1 = READ_INPUT_DATA_BYTE;
    *pCardVer = TempValue1;
	
    DataBusSelect(READ_BOARD_ADDRESS2); // note note, board address2 is equal to tmpBuf1
    delayMicroseconds(8);
    TempValue2 = READ_INPUT_DATA_BYTE;
    *pCardType = TempValue2;
	
    printf(msgCardTypeVer, TempValue2, TempValue1 );
    DataBusSelect(DESELECT_ALL);
	
    return RetVal;
}

/***************************************************************************************
*           MICRO CONTROLLER FUNCTIONS
****************************************************************************************/
/*************************************************************************************
*  Function   : LCPC_EnableInterrupt
*------------------------------------------------------------------------------------
*  Object     : This function sets the enable interrupt flag.
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void LCPC_EnableInterrupt(void)
{
//    LCPC_INTERRUPT_ENABLE;
}

/*************************************************************************************
*  Function   : LCPC_DisableInterrupt
*------------------------------------------------------------------------------------
*  Object     : This function disables all interrupts.
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void LCPC_DisableInterrupt(void)
{
//    LCPC_INTERRUPT_DISABLE;
}

void timer_seconds(long int seconds)
{
	struct itimerval timer1;
 
	timer1.it_interval.tv_usec = 0;
	timer1.it_interval.tv_sec = seconds;
	timer1.it_value.tv_usec = 0;
	timer1.it_value.tv_sec = seconds;
 
	setitimer(ITIMER_REAL, &timer1, NULL);
}
 
void timer_useconds(long int useconds)
{
	struct itimerval timer2;
 
	timer2.it_interval.tv_usec = useconds;
	timer2.it_interval.tv_sec = 0;
	timer2.it_value.tv_usec = useconds;
	timer2.it_value.tv_sec = 0;
 
	setitimer(ITIMER_REAL, &timer2, NULL);
}

/*************************************************************************************
*  Function   : LCPC_SetTimer2
*------------------------------------------------------------------------------------
*  Object     : Disable it, and refresh its register's values.
*               Has been called before initializing IO cards 
*
*  Return     : RET_SUCCESS
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void LCPC_SetTimer2(void)
{
	DISABLE_TIMER2_INTERRUPT; // Disable timer 2
//	SET_TIMER2B_MODE;    	 // Sets the timer2 in normal mode.Set the clock to CLKio/64 (8 MHz/ 64 = 125KHz (8 uSec))
    
	SET_TIMER2_VALUE;

    Card.Timer2_CounterA = 0;			// CAN link timeout indicator
    Card.Timer2_CounterB = 0;           // reserved 
    Card.Timer2_CounterC = 0;           // reserved 
    Card.Timer2_CounterD = 0;           // reserved     
    Card.Timer2_CounterE = 0;           // reserved 
    Card.Timer2_CounterF = 0;           // ADC timeout indicator
    Card.Timer2_CounterG = 0;		    // reserved 
    Card.Timer2_CounterH = 0;		    // reserved 
    Card.Timer2_CounterI = 0;	      	// reserved  
    Card.Timer2_Calibration = 0 ;
    Card.Timer2_SetFilter   = 0 ;
    Card.LedBlinkCounter200ms = 0;
    Card.LedBlinkCounter500ms = 0;
    Card.LedBlinkCounter1s  = 0;
    Card.HotRedundancyTxTimer = 0;
    Card.HotRedundancyRxTimer = 0;
    Card.HotRedundancyFailCounter   = 0;
	
    Card.ConvTime = 0;
	
    Card.IO_LED_Timer = 0;
	
    Card.LEDsUpdateTime = 0;
    Card.LED_Update_Flag = false;
	
    Card.OpenLoadTestTime = 0;
    Card.OpenLoadTestFlag = 0;
    Card.SerialPortTimer = 0;
	
    ENABLE_TIMER2_INTERRUPT;	// Enable timer 2
}

/***********************************************************************************
*  Interrupt  : Timer2OVHandler
*----------------------------------------------------------------------------------
*  Object     : This function is the ISR of the timer2 interrupt. This ISR is called 
*               every 1 ms if the Timer2 is enabled 
*
*  Return     : None
*
***********************************************************************************/
//#pragma vector = TIMER2_OVF_vect
void Timer2OVHandler(int signo)
{
    UChar tmpData;
	
//	SET_TIMER2_VALUE;	
	
	//RunCycle();
//	Increase the Timer2 related counters      
      
    Card.Timer2_CounterA++;      	  // CAN link timeout indicator
    Card.Timer2_CounterB++;           // reserved  
    Card.Timer2_CounterC++;           // reserved 
    Card.Timer2_CounterD++;           // reserved 
    Card.Timer2_CounterE++;           // reserved 
    Card.Timer2_CounterF++;           // ADC timeout indicator
    Card.Timer2_CounterG++;           // reserved 
    Card.Timer2_CounterH++;           // reserved 
    Card.Timer2_CounterI++;		      // reserved 
    Card.Timer2_Calibration++;		  // reserved 
    Card.Timer2_SetFilter++;          // ADC SetFilter timeout
    Card.LedBlinkCounter200ms++;	
    Card.LedBlinkCounter100ms++;
    Card.LedBlinkCounter500ms++;
    Card.LedBlinkCounter1s++;
	
    Card.ConvTime++;
    Card.IO_LED_Timer++;
	
    Card.LEDsUpdateTime++;
    Card.OpenLoadTestTime++;
    Card.SerialPortTimer++;
	
    if ( Card.State == MAIN )
    {
		if ( Card.IO_LED_Timer == 500 || Card.IO_LED_Timer == 1000 )
			Card.IO_LED_Update_Flag = true;
    }
    else
		Card.IO_LED_Timer = 0;
	
#if ( CARD_TYPE == IOTYPE_AI621 )
	
    if( Card.LEDsUpdateTime >= LED_UPDATE_TIME_VAL )
    {
		Card.LED_Update_Flag = true;
		Card.LEDsUpdateTime = 0;
    }
    
    if( Card.OpenLoadTestTime >= TC_OPEN_LOAD_TIME_VAL )
    {
		Card.OpenLoadTestFlag = 1;
		Card.OpenLoadTestTime = 0;
    }
#endif
    
#if ( ( CARD_TYPE == IOTYPE_AO672 ) || ( CARD_TYPE == IOTYPE_AO673 ) )    // NOTE : HOT Redundancy is implemented only for AO Cards
     HotRedundancyHandler();
#endif
	
    tmpData  = Card.GeneralLED;		// Put the previous LED condition
    tmpData |= 0x07;				// Turn first 3 LEDs off (RUN, STDBY & ERROR)
	
	// =============== During Auto Test if it fails =============
    if( Card.IOFault == 1 )
    {
        if( Card.LedBlinkCounter200ms == 200 )
        {
            tmpData &= 0xFB;
            Card.GeneralLED = tmpData;
        }
        else if( Card.LedBlinkCounter200ms  == 400 )
        {
            tmpData |= 0x04;
            Card.GeneralLED = tmpData;
            Card.LedBlinkCounter200ms = 0 ;
            if ( Card.IOFaultCounter++ == 20 )
                Card.IOFault = 0;
        }
    }

	// ======================================================
    
    switch( Card.State )
    {
	  case OUT_OF_SERVICE:
		tmpData &= 0xFB;
		Card.GeneralLED = tmpData;
        break;
	  case  IDLE:
		if( Card.LedBlinkCounter1s == 1000 )
		{
			tmpData |= 0x01;
			Card.GeneralLED = tmpData;
		}
		else if( Card.LedBlinkCounter1s == 2000 )
		{
			tmpData &= 0xFE;
			Card.GeneralLED = tmpData;
			Card.LedBlinkCounter1s = 0 ;
		}
        break;
	  case  MAIN:
		tmpData &= 0xFE;		// LED4 (RUN) ON
		Card.GeneralLED = tmpData;
        break;
	  case  BACKUP:
		tmpData &= 0xFD;		// LED5 (STDBY) ON
		Card.GeneralLED = tmpData;
        break;
	  case  FAIL_SAFE_MAIN:
		if( Card.LedBlinkCounter500ms == 500 )
		{
			tmpData &= 0xFA;
			Card.GeneralLED = tmpData;
		}
		else if( Card.LedBlinkCounter500ms  == 1000 )
		{
			tmpData &= 0xFF;
			Card.GeneralLED = tmpData;
			Card.LedBlinkCounter500ms = 0 ;
		}
		break ;
	  case FAIL_SAFE_BACKUP:
		if( Card.LedBlinkCounter500ms == 500 )
		{
			tmpData &= 0xF9;
			Card.GeneralLED = tmpData;
		}
		else if( Card.LedBlinkCounter500ms  == 1000 )
		{
			tmpData &= 0xFF;
			Card.GeneralLED = tmpData;
			Card.LedBlinkCounter500ms = 0 ;
		}
        break;
    }  
    
    LCPC_SetLEDs( Card.GeneralLED );

}

/***********************************************************************************
*  USART FUNCTIONS
***********************************************************************************/
/***********************************************************************************
*  Function   : LCPC_USARTInit
*----------------------------------------------------------------------------------
*  Object     : this function initializes the USART.
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  baudrate		   |  The baudrate of USART
*------------------+-----------------------------------------------------------------
************************************************************************************/
void LCPC_SHInputThread(void)
{
    
}

/***********************************************************************************
*  Function   : putchar
*----------------------------------------------------------------------------------
*  Object     : this function sends a character in UART transmit procedure
*
*  Return     : charachter
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  c			   |  character to be sent
*------------------+-----------------------------------------------------------------
***********************************************************************************/
//Int putchar(Int c)
//{
//    USART0_DATA = c;
//	
//    Card.SerialPortTimer = 0;
//    while((Card.SerialPortTimer < 10) && ( USART0_WAITTX_FLG ) == 0 )
//    {
//	    if (watchdogEnable == WDT_ENABLE)
//		    feedWD(deviceHandle);
//    }
//	
//	//    if ( ( USART0_WAITTX_FLG ) == 0 )
//	//      Card.LCPCWarning.Bit.ResetViaWatchdog = 1;
//	//    else
//	//      Card.LCPCWarning.Bit.ResetViaWatchdog = 0;
//	
//    USART0_SETTX_FLG;
//    return c;
//}

/*************************************************************************************
*  Function   : LCPC_DefBackplaneAddress
*------------------------------------------------------------------------------------
*  Object     : Call this function to define Backplane Address pins
*
*  Return     : None.
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void LCPC_DefBackplaneAddress( void )
{   
    BP_ADDR0_DEF;
    BP_ADDR1_DEF;
    BP_ADDR2_DEF;
    BP_ADDR3_DEF;
    BP_ADDR4_DEF;
    BP_ADDR5_DEF;
}

///*************************************************************************************
//*  Function   : LCPC_IOExpLEDAddInit
//*------------------------------------------------------------------------------------
//*  Object     : it is called to initialize LPC card's IO Expander chip (MAX7313_U2)
//*               The 8 lower ports are defined as Inputs and 8 higher ports are defined as 
//*               outputs to drive LEDs on P10 to P15
//*
//*  Return     : RET_SUCCESS			Function is done successfully
//*               ERR_IICBUS			IIC bus error
//*               ERR_NOTRESPOND		Device not responding
//*               ERR_TIMEOUT			IC function timed out
//*               ERR_REPEAT			Error during execution of functionRET_SUCCESS
//*
//*	Note	: deprecated
//*------------------------------------------------------------------------------------
//*  Input Arguments |  Description
//*------------------+-----------------------------------------------------------------
//*  None			   |  ---
//*------------------+-----------------------------------------------------------------
//*************************************************************************************/
//void LCPC_IOExpLEDAddInit( void )
//{
//    //LEDs Definition
//	LED1_OUTPUT_DEF;
//	LED2_OUTPUT_DEF;
//	LED3_OUTPUT_DEF;
//	LED4_OUTPUT_DEF;
//	LED5_OUTPUT_DEF;
//	LED1_INPUT_DEF;
//	LED2_INPUT_DEF;
//	LED3_INPUT_DEF;
//	LED4_INPUT_DEF;
//	LED5_INPUT_DEF;
//}

/*************************************************************************************
*  Function   : LCPC_GetBackplaneAddress
*------------------------------------------------------------------------------------
*  Object     : Call this function to retrieve Backplane Address
*
*  Return     : back plane address
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar LCPC_GetBackplaneAddress( void )
{
    UChar data = 0x3F;
    //	The generated BP address byte  from hardware BP address is:
    //	MSB 0, BP1, BP0, BP3, 0, BP2, BP5, BP4 LSB	
    
    if ( BP_ADDR0_READ != 0 )
        data &= 0xFE;          // 1 1 1 1  1 1 1 0
    if ( BP_ADDR1_READ != 0 )
        data &= 0xFD;          // 1 1 1 1  1 1 0 1
    if ( BP_ADDR2_READ != 0 )
        data &= 0xFB;          // 1 1 1 1  1 0 1 1
    if ( BP_ADDR3_READ != 0 )
        data &= 0xF7;          // 1 1 1 1  0 1 1 1
    if ( BP_ADDR4_READ != 0 )
        data &= 0xEF;          // 1 1 1 0  1 1 1 1
    if ( BP_ADDR5_READ != 0 )
        data &= 0xDF;          // 1 1 0 1  1 1 1 1 
	
    return data;
}

/***************************************************************************************
*           File DAQ FUNCTIONS
****************************************************************************************/
/*************************************************************************************
*  Function   : LCPC_InternalWrite_Byte
*------------------------------------------------------------------------------------
*  Object     : This function has the duty of writing to specified address of Internal File
*               
*               the global interrupt flag cleared during the running function to avoid these problems if you access to File
*               by interrupt.
*
*  Return     : RET_SUCCESS			Function is done successfully
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  WordAdd		   |  A 2 bytes word address
*------------------+-----------------------------------------------------------------
*  DataByte		   |  Specified data to be written
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar LCPC_InternalWrite_Byte(UInt WordAdd, UChar DataByte)
{
    UChar RetVal = RET_SUCCESS;	
	FILE * pFile;


	pFile = fopen("/etc/DIO560/EEPROM.txt", "wb");
	fseek(pFile, WordAdd, SEEK_SET);
	fputc(DataByte+'0', pFile);
//    /*Wait for completion of previous write*/  
//    while( WAIT_FOR_INTERNAL_E2PROM_WRITE_COMPLETION );
    
     /*Set up address and data registers*/ 
//    INTERNAL_E2PROM_ADDRESS_REGISTER = WordAdd;
//    INTERNAL_E2PROM_DATA_REGISTER	 = DataByte;
    
	
	
    /*Set the command for writing to file*/ 
//    INTERNAL_E2PROM_WRITE_COMMAND;	
	
	
    
    return RetVal;
}

/*************************************************************************************
*  Function   : LCPC_InternalE2PRead 
*------------------------------------------------------------------------------------
*  Object     : This function has the duty of reading "DataSize" bytes start from 
*               specified address of Internal File memory (WordAdd).
*
*  Return     : RET_SUCCESS			Function is done successfully
*               ERR_TIMEOUT			Timed out
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  WordAdd		   |  A 2 bytes word address
*------------------+-----------------------------------------------------------------
*  DataBuf		   |  pointer to the data buffer
*------------------+-----------------------------------------------------------------
*  DataSize		   |  Size of the valid data in the buffer
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar LCPC_InternalRead(UInt WordAdd, UChar *DataBuf, UChar DataSize)
{
    UChar Counter, RetVal;					// General Buffer for Word Adresss and Data 
    
    RetVal = RET_SUCCESS;
	FILE * pFile;


	pFile = fopen("/etc/DIO560/EEPROM.txt", "rb");

	fgetc(pFile);
    for (Counter = 0; Counter < DataSize; ++Counter)
    {
//        // Wait for completion of previous write     
//        while( WAIT_FOR_INTERNAL_E2PROM_WRITE_COMPLETION );
//        
        // Set up address register 
//        INTERNAL_E2PROM_ADDRESS_REGISTER = WordAdd + Counter;
          fseek(pFile, WordAdd + Counter, SEEK_SET);
	    
//        // Start eeprom read by writing EERE 
//        INTERNAL_E2PROM_READ_COMMAND;
        
        // Return data from data register 
//        *(DataBuf + Counter) = INTERNAL_E2PROM_DATA_REGISTER;
	      *(DataBuf + Counter) = fgetc(pFile);  
	    
    }
	
	
    
    return RetVal;
}

///*************************************************************************************
//*  Function   : LCPC_ExternalE2PWrite_Byte
//*------------------------------------------------------------------------------------
//*  Object     : This function has the duty of writing to specified address of External 
//*				E2PROM memory through the IIC bus.
//*
//*  Return     : RET_SUCCESS			Function is done successfully
//*               ERR_IICBUS			IIC bus error
//*               ERR_NOTRESPOND		Device not responding
//*               ERR_TIMEOUT			IC function timed out
//*               ERR_REPEAT			Error during execution of function (Repetition may required)
//*
//*  Note(s)    : deprecated
//*
//*------------------------------------------------------------------------------------
//*  Input Arguments |  Description
//*------------------+-----------------------------------------------------------------
//*  WordAdd		   |  A 2 bytes word address
//*------------------+-----------------------------------------------------------------
//*  DataByte		   |  Specified data to be written
//*------------------+-----------------------------------------------------------------
//*************************************************************************************/
//UChar LCPC_ExternalE2PWrite_Byte(UInt WordAdd, UChar DataByte)
//{
//    UChar data[3], RetVal;		    // General variables
//    ULong RepCntr;				    // Counter big enough to keep repeat number
//    
//    RetVal = ERR_UNKNOWN;
//    if( WORDADD_SIZE == 2 )
//    {// EEPROM needs 2 bytes word address
//        data[0] = WordAdd >> 8;			// First word address
//        data[1] = WordAdd;				// Second word address
//        data[2] = DataByte;				// Data byte
//		
//        RetVal = LCPC_IICWrite(LCPC_E2PROM_SLA,data, 3);
//        RepCntr = 1;		
//        while(RepCntr++ <IIC_REPEAT && RetVal == IICERR_SLAACK)			
//            RetVal = LCPC_IICWrite(LCPC_E2PROM_SLA,data, 3);	
//    }
//    else if( WORDADD_SIZE == 1 )
//    {		// EEPROM needs 1 byte word address
//        WordAdd &= E2PROM_ADDMASK;		// Mask the address
//        data[0] = WordAdd & 0xff;		// Word address
//        data[1] = DataByte;				// Data Byte
//		
//        RetVal = LCPC_IICWrite(LCPC_E2PROM_SLA+(WordAdd >> 8),data, 2);	// Write to EEPROM 
//        RepCntr = 1;		
//        while(RepCntr++ < IIC_REPEAT && RetVal == IICERR_SLAACK)			
//            RetVal = LCPC_IICWrite(LCPC_E2PROM_SLA,data, 2);	
//    }
//	
//    return LCPC_Translate_IICRetVal( RetVal );
//}
//
///*************************************************************************************
//*  Function   : LCPC_ExternalE2PWrite_Page
//*------------------------------------------------------------------------------------
//*  Object     : This function has the duty of writing to specified address of External E2PROM
//*               memory through the IIC bus.
//*
//*  Return     : RET_SUCCESS			Function is done successfully
//*               ERR_IICBUS			IIC bus error
//*               ERR_NOTRESPOND		Device not responding
//*               ERR_TIMEOUT			IC function timed out
//*               ERR_REPEAT			Error during execution of function (Repetition may required)
//*
//*  Note(s)    : deprecated
//*
//*------------------------------------------------------------------------------------
//*  Input Arguments |  Description
//*------------------+-----------------------------------------------------------------
//*  WordAdd		   |  A 2 bytes word address
//*------------------+-----------------------------------------------------------------
//*  DataBuf		   |  pointer to the data buffer
//*------------------+-----------------------------------------------------------------
//*  DataSize		   |  Size of the valid data in the buffer
//*------------------+-----------------------------------------------------------------
//*************************************************************************************/
//UChar LCPC_ExternalE2PWrite_Page(UInt WordAdd, UChar *DataBuf, UChar DataSize)
//{
//    UChar data[PAGE_SIZE+2], RetVal;		// General Buffer for Word Adresss and Data 
//    UInt d;
//    ULong RepCntr;					    // Counter big enough to keep repeat number
//    
//    if(DataSize > PAGE_SIZE) 				// Check if the DataSize is greater than Page Size return ERRORE
//        return ERR_PAGESIZE;				// set error
//    
//    RetVal = ERR_UNKNOWN;					// Default value
//    if( WORDADD_SIZE == 2 )
//    {// EEPROM needs 2 bytes word address
//        data[0]= WordAdd >> 8;				// First word address
//        data[1]= WordAdd;					// Second word address
//        for(d = 0; d < DataSize; d++)	
//            data[d + 2] = DataBuf[d];			// Fill the buffer with data
//		
//        RetVal = LCPC_IICWrite(LCPC_E2PROM_SLA, data, DataSize + 2);  		// Write to E2PROM
//        RepCntr = 1;		
//        while(( RepCntr++ < IIC_REPEAT ) && ( RetVal == IICERR_SLAACK ))			
//            RetVal = LCPC_IICWrite(LCPC_E2PROM_SLA, data, DataSize + 2);  	// Write to E2PROM
//    }
//    else if( WORDADD_SIZE == 1 )
//    {// EEPROM needs 1 byte word address
//        WordAdd &= E2PROM_ADDMASK;			    // Mask the address
//        data[0] = WordAdd & 0xff;				// word address
//        for( d = 0; d < DataSize; d++ )	
//            data[d + 1] = DataBuf[d];						// Fill the buffer with data
//		
//        RetVal = LCPC_IICWrite(LCPC_E2PROM_SLA + (WordAdd >> 8),data,DataSize + 1);		// Write to EEPROM  
//        RepCntr = 1;		
//        while((  RepCntr++ < IIC_REPEAT ) && ( RetVal == IICERR_SLAACK ))			
//            RetVal = LCPC_IICWrite(LCPC_E2PROM_SLA + (WordAdd >> 8),data,DataSize + 1);	// Write to EEPROM  
//    }
//	
//    return LCPC_Translate_IICRetVal( RetVal );
//}
//
///*************************************************************************************
//*  Function   : LCPC_ExternalE2PRead_Current
//*------------------------------------------------------------------------------------
//*  Object     : This function has the duty of reading a byte from the address which has 
//*               been set in last write instruction.
//*
//*  Return     : RET_SUCCESS			Function is done successfully
//*               ERR_IICBUS			IIC bus error
//*               ERR_NOTRESPOND		Device not responding
//*               ERR_TIMEOUT			IC function timed out
//*               ERR_REPEAT			Error during execution of function (Repetition may required)
//*
//*  Note(s)    : deprecated
//*
//*------------------------------------------------------------------------------------
//*  Input Arguments |  Description
//*------------------+-----------------------------------------------------------------
//*  data		 	   |  Read data will be placed here
//*------------------+-----------------------------------------------------------------
//*************************************************************************************/
//UChar LCPC_ExternalE2PRead_Current(UChar data)
//{
//    UChar RetVal;
//    ULong RepCntr;												    // Counter big enough to keep repeat number
//    
//    RetVal = LCPC_IICRead(LCPC_E2PROM_SLA, &data, 1);					// Read from EEPROM
//    RepCntr = 1;		
//    while(( RepCntr++ < IIC_REPEAT ) && ( RetVal == IICERR_SLAACK ))			
//        RetVal = LCPC_IICRead(LCPC_E2PROM_SLA, &data, 1);				// Read from EEPROM
//    
//    return LCPC_Translate_IICRetVal( RetVal );
//}
//
///*************************************************************************************
//*  Function   : LCPC_ExternalE2PRead 
//*------------------------------------------------------------------------------------
//*  Object     : This function has the duty of reading "DataSize" bytes start from 
//*               specified address of External E2PROM memory (WordAdd) through the IIC bus.
//*
//*  Return     : RET_SUCCESS			Function is done successfully
//*               ERR_IICBUS			IIC bus error
//*               ERR_NOTRESPOND		Device not responding
//*               ERR_TIMEOUT			IC function timed out
//*               ERR_REPEAT			Error during execution of function (Repetition may required)
//*
//*  Note(s)    : deprecated
//*
//*------------------------------------------------------------------------------------
//*  Input Arguments |  Description
//*------------------+-----------------------------------------------------------------
//*  WordAdd		   |  A 2 bytes word address
//*------------------+-----------------------------------------------------------------
//*  DataBuf		   |  pointer to the data buffer
//*------------------+-----------------------------------------------------------------
//*  DataSize		   |  Size of the valid data in the buffer
//*------------------+-----------------------------------------------------------------
//*************************************************************************************/
//UChar LCPC_ExternalE2PRead(UInt WordAdd, UChar *DataBuf, UChar DataSize)
//{
//    UChar data[2], RetVal;				// General Buffer for Word Adresss and Data 
//    ULong RepCntr;					    // Counter big enough to keep repeat number
//    // In this part we do a dummy write to set the word address 
//    RetVal = ERR_UNKNOWN;					// Default value
//	
//    if( WORDADD_SIZE == 2 )
//    {// EEPROM needs 2 bytes word address
//        data[0]= WordAdd >> 8;				// First word address
//        data[1]= WordAdd;					// Second word address
//        RetVal = LCPC_IICWrite(LCPC_E2PROM_SLA, data, 2);  		// Write to E2PROM
//        RepCntr = 1;
//        while(( RepCntr++ < IIC_REPEAT ) && ( RetVal == IICERR_SLAACK ))			
//            RetVal = LCPC_IICWrite(LCPC_E2PROM_SLA, data, 2);  	// Write to E2PROM
//    }
//    else if( WORDADD_SIZE == 1 )
//    {// EEPROM needs 1 byte word address
//        WordAdd &= E2PROM_ADDMASK;			                    // Mask the address
//        data[0]=WordAdd & 0xff;				                    // word address
//        RetVal = LCPC_IICWrite(LCPC_E2PROM_SLA + (WordAdd >> 8), data, 1);		// Write to EEPROM  
//        RepCntr = 1;		
//        while(( RepCntr++<IIC_REPEAT ) && ( RetVal == IICERR_SLAACK ))			
//            RetVal = LCPC_IICWrite(LCPC_E2PROM_SLA + (WordAdd >> 8), data, 1);	// Write to EEPROM  
//    }
//    // Now we do the Read action. 
//    if( RetVal == IIC_SUCCESS )											// Do it if dummy write was successful
//        RetVal = LCPC_IICRead(LCPC_E2PROM_SLA, DataBuf,DataSize);		// Read from EEPROM
//    
//    return LCPC_Translate_IICRetVal( RetVal );
//}
//
///*************************************************************************************
//*  Function   : LCPC_ExternalE2PTest
//*------------------------------------------------------------------------------------
//*  Object     : Call this function to test the External EEPROM memory. Be aware calling this function 
//*               may manipulate EEPROM data.
//*
//*  Return     : RET_SUCCESS			Function is done successfully
//*               ERR_IICBUS			IIC bus error
//*               ERR_NOTRESPOND		Device not responding
//*               ERR_TIMEOUT			IC function timed out
//*               ERR_REPEAT			Error during execution of function (Repetition may required)
//*
//*  Note(s)    : deprecated
//*
//*------------------------------------------------------------------------------------
//*  Input Arguments |  Description
//*------------------+-----------------------------------------------------------------
//*  None			   |  ---
//*------------------+-----------------------------------------------------------------
//*************************************************************************************/
//UChar LCPC_ExternalE2PTest(void)
//{
//    UChar RetVal, data[PAGE_SIZE], i;	
//    
//    for(i = 0; i < PAGE_SIZE; i++)	
//        data[i] = i;		// Fill the buffer with test data
//	
//    RetVal = LCPC_ExternalE2PWrite_Page(((PAGE_NUMBER-1) * PAGE_SIZE), data, PAGE_SIZE);  // Fill the final page with test data
//    
//    for( i = 0; i < PAGE_SIZE; i++ )	
//        data[i] = 0;		// Clear the buffer
//	
//    if( RetVal == RET_SUCCESS )								// Continue if the last operation was successful
//        RetVal = LCPC_ExternalE2PRead(((PAGE_NUMBER - 1) * PAGE_SIZE), data, PAGE_SIZE);    // Read first page
//    
//    if( RetVal == RET_SUCCESS )
//    {
//        for(i = 0; i < PAGE_SIZE; i++)	
//        {
//            if(data[i] != i)
//            {								    // Check the read data with test data
//                RetVal = ERR_TESTFAILED;					    // Report error
//                break;											
//            }
//        }
//    }
//    return RetVal;
//}
/************************************************************************************
*           TEMPERATURE SENSOR FUNCTIONS
*************************************************************************************/
/*************************************************************************************
*  Function   : LCPC_TempSensorInit
*------------------------------------------------------------------------------------
*  Object     : to initialize the Temp Sensor (TCN75) < Refer to datasheet>
*
*  Return     : RET_SUCCESS			Function is done successfully
*               ERR_IICBUS			IIC bus error
*               ERR_NOTRESPOND		Device not responding
*               ERR_TIMEOUT			IIC function timed out
*               ERR_REPEAT			Error during execution of function (Repetition may required)
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  IOEX_SLA		   |  IIC SLA Address of the specified temperature sensor
*------------------+-----------------------------------------------------------------
*  FaultQueue	   |  Number of reads for reporting temperaturer
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar LCPC_TempSensorInit()
{
    UChar RetVal;	
	
    // Reading CPU Temp 
	FILE *temperatureFile;
	double T;
	temperatureFile = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
	if (temperatureFile == NULL) 
	{
		RetVal = ERR_NOTRESPOND;
	}
	else
	{
		RetVal = RET_SUCCESS;
		
	}
  
    // Find the error and report it through SSH  
    switch( RetVal )
    {   
      case ERR_NOTRESPOND:
        printf("%s",errLCTempSensNotRes);
        break;
      default:
	    printf("%s",msgLCTempSensInit);
	    break;
    }
    
    return RetVal;
}

/*************************************************************************************
*  Function   : LCPC_TempSensorTest
*------------------------------------------------------------------------------------
*  Object     : to test the Temp Sensor (TCN75) by writing and reading its registers
*
*  Return     : RET_SUCCESS			Function is done successfully
*               ERR_IICBUS			IIC bus error
*               ERR_NOTRESPOND		Device not responding
*               ERR_TIMEOUT			IC function timed out
*               ERR_TESTFAILED		Temp sensor test failed
*               ERR_REPEAT			Error during execution of function (Repetition may required)
*
*  Note(s)    : FOR DEBUG ONLY
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  IOEX_SLA		   |  UChar LCPC_TempSensorTest(UChar IOEX_SLA)
*------------------+-----------------------------------------------------------------
*************************************************************************************/
//UChar LCPC_TempSensorTest(UChar IOEX_SLA)
//{
//    UChar RetVal;
//    UInt testVal;	
//    
//    RetVal = LCPC_TempSensorWrite( IOEX_SLA, TMPS_HYST, 0x5e80 );		// Fill a register with a test value	
//    
//    if( RetVal == IIC_SUCCESS )
//    {
//        RetVal = LCPC_TempSensorRead( IOEX_SLA, TMPS_HYST, &testVal );	// Check the data
//        if( RetVal == IIC_SUCCESS )
//        {
//            if(testVal != 0x5e80)	
//                RetVal = ERR_TESTFAILED;				// Report mismatch error
//            
//            if( RetVal == IIC_SUCCESS )
//            {
//                RetVal = LCPC_TempSensorWrite( IOEX_SLA, TMPS_SET, 0xf000 );	// Fill a register with a test value
//                if( RetVal == IIC_SUCCESS )
//                {
//                    RetVal = LCPC_TempSensorRead( IOEX_SLA, TMPS_SET, &testVal );// Check the data
//                    if( RetVal == IIC_SUCCESS )
//                    {
//                        if(testVal != 0xf000)	
//                            RetVal = ERR_TESTFAILED;	    // Report mismatch error
//                    }
//                }
//            }
//        }
//    }
//    
//    return LCPC_Translate_IICRetVal( RetVal );
//}

/*************************************************************************************
*  Function   : LCPC_TempSensorReadTemp
*------------------------------------------------------------------------------------
*  Object     :  to read Temperature data from Temp Sensor (TCN75)
*
*  Return     : RET_SUCCESS			Function is done successfully
*               ERR_IICBUS			IIC bus error
*               ERR_NOTRESPOND		Device not responding
*               ERR_TIMEOUT			IC function timed out
*               ERR_REPEAT			Error during execution of function (Repetition may required)
*
*  Note(s)    : TCN75 has the accuracy of 0.5 degree, but this function's accuracy is 1 
*				degree Centigrade.
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  IOEX_SLA		   |  IIC SLA Address of the specified temperature sensor
*------------------+-----------------------------------------------------------------
*  pTemperature	   |  a pointer to a UInt variable to keep read data
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar LCPC_TempSensorReadTemp(SChar* pTemperature)
{
    UChar RetVal;											        // Global variable
//    UInt tmpInt;
    
//	RetVal = LCPC_TempSensorRead(IOEX_SLA, TMPS_TEMP, &tmpInt);	

	// Convert the read temperature to a signed Char (1 degree accuracy)
//    *pTemperature = (Char)(tmpInt >> 8);	
	FILE *temperatureFile;
	double T;
	temperatureFile = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
	if (temperatureFile != NULL) 
		RetVal = RET_SUCCESS; 
	fscanf(temperatureFile, "%lf", &T);
	T/= 1000;
	*pTemperature = T;
	//for Debug only
	printf("The temperature is %4.1f C.\n", T);
	fclose(temperatureFile); 
	return RetVal;
}

/*************************************************************************************
*  Function   : LCPC_CheckTemp
*------------------------------------------------------------------------------------
*  Object     : This function is called to check temperature
*
*  Return     : RET_OVERHEAT		Over heat detected
*               RET_HYSTHEAT		Temperature is in hysteresis margin
*               RET_NORMHEAT		Normal heat detected
*               ERR_IICBUS			IIC bus error
*               ERR_NOTRESPOND		Device not responding
*               ERR_TIMEOUT			IC function timed out
*               ERR_REPEAT			Error during execution of function (Repetition may required)
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  IOEX_SLA		   |  IIC SLA Address of the specified temperature sensor
*------------------+-----------------------------------------------------------------
*  Temperature	   |  Read value of Temperature by TCN75
*------------------+-----------------------------------------------------------------
*  SetPoint		   |  Input SetPoint for temperature to decide over heat, ...
*------------------+-----------------------------------------------------------------
*  TempHyst		   |  Input TempHyst for temperature to decide hysterisis margin.
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar LCPC_CheckTemp(SChar* Temperature, SChar SetPoint, UChar TempHyst)
{
    UChar RetVal;
    
	// Read the current temperature
    RetVal = LCPC_TempSensorReadTemp( Temperature );	
//	*Temperature = 80;
    if( RetVal == RET_SUCCESS )
    {								// Read is done successfully
        if( *Temperature > SetPoint )	
            RetVal = RET_OVERHEAT;	// Over heat detected!
        else if( *Temperature < (SetPoint - TempHyst))	
            RetVal = RET_NORMHEAT;  // Normal heat is detected.
        else 
            RetVal = RET_HYSTHEAT;	// Temperature has reached to the hysteresis margin							
    }
    
    return RetVal;
}

/**************************************************************************************
*           EXTERNAL RAM FUNCTIONS
***************************************************************************************/
/*************************************************************************************
*  Function   : LCPC_RAMCheck
*------------------------------------------------------------------------------------
*  Object     : This function is called to to test if the external RAM is ok or not.
*
*  Return     : RET_SUCCESS		    RAM is ok
*               RET_TESTFAILED  	RAM has error
*
*  Note(s)	  : deprecated
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  StartAddress    |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
//UChar LCPC_RAMCheck(UInt StartAddress)
//{
//    UChar i, RetVal, Data;
//    UChar *RAM_Pointer = (UChar *) (StartAddress);
//    
//    RetVal = RET_SUCCESS;						// Default value
//    for(i = 0; i < CHECKMARGIN; i++)
//    {
//        *(RAM_Pointer + i) = i + 0xc5;			// A random number is put in the memory cell
//    }
//    for(i = 0; i < CHECKMARGIN; i++)
//    {
//        Data = *(RAM_Pointer + i);
//        if(Data != i + 0xc5)	
//            RetVal = ERR_TESTFAILED;
//    }
//    return RetVal;
//}

/*************************************************************************************
*  Function   : LCPC_Check
*------------------------------------------------------------------------------------
*  Object     : it checks all the hardware components of the LPC board and returns 
*				the result. Also some of the Alerts are generated by this function
*
*  Return     : status of LCPC
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar LCPC_Check( void )
{
    UChar RetVal, tmpVar;
	RetVal = RET_SUCCESS;
	
    // Check the LCPC temperature
    tmpVar = LCPC_CheckTemp( &Card.TemperatureLCPC, Setting.Bit.Generic.OverHeatLimit, Setting.Bit.Generic.OverHeatBand );
    if (tmpVar == RET_OVERHEAT)
    {
        RetVal = ERR_TESTFAILED;			// Error detected during execution of function	
        Card.LCPCWarning.Bit.OverheatLCPC = 1;		// Set LCPC over heat warning bit		
        Card.LCPCError.Bit.TemperatureSensor = 0;	// Reset LCPC Temp Sensor Error bit
		
		// FOR DEBUG ONLY: print out overheat warning
		// printf(errOverHeat);  
    }
    else if (tmpVar == RET_NORMHEAT)
    {
        Card.LCPCWarning.Bit.OverheatLCPC = 0;		    // Reset LCPC over heat warning bit
        Card.LCPCError.Bit.TemperatureSensor = 0;		// Reset LCPC Temp Sensor Error bit
    }
    else if (tmpVar == RET_HYSTHEAT)					// Between Normal heat and Overheat
        Card.LCPCError.Bit.TemperatureSensor = 0;		// Reset LCPC Temp Sensor Error bit
    else
    {								
        RetVal = ERR_TESTFAILED;	// Error detected during execution of function	
        Card.LCPCError.Bit.TemperatureSensor = 1;		// Set LCPC Temp Sensor Error bit

		// FOR DEBUG ONLY: print out Error in communication with Temp sensor
		// printf(errTempSensReadErr);
    }
    
    if( Card.HasDiag == 0 )
    {
     
        if(( Card.LCPCError.D8 != Card.PreviousLCPCError.D8 ) || ( Card.LCPCWarning.D8 != Card.PreviousLCPCWarning.D8 ))
        {

            printf(errLCPCErrorWarning, Card.LCPCError.D8, Card.LCPCWarning.D8);
            Card.PreviousLCPCError.D8   = Card.LCPCError.D8;
            Card.PreviousLCPCWarning.D8 = Card.LCPCWarning.D8;
            Card.HasDiag = 1;
        }
    }
    
    return RetVal;
}

/*************************************************************************************
*  Function   : LCPC_Get_Type
*------------------------------------------------------------------------------------
*  Object     : Reads the LCPC version.
*
*  Return     : LCPC version
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
//UChar LCPC_Get_Type( void )
//{
//    UChar  LCPC_Version;
//	
////    LED1_INPUT_DEF;
////    LED2_INPUT_DEF;
////    LED3_INPUT_DEF;
////    LED4_INPUT_DEF;
////    LED5_INPUT_DEF;
////    __delay_cycles(8);
//    
////    LCPC_Version = READ_LCPC_VERSION;
//	
////    LED1_OUTPUT_DEF;
////    LED2_OUTPUT_DEF;
////    LED3_OUTPUT_DEF;
////    LED4_OUTPUT_DEF;
////    LED5_OUTPUT_DEF;
//    
//    return LCPC_Version;
//	
//}

//***********************************************************************************
// No More
//***********************************************************************************