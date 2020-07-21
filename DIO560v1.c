#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <sys/time.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include <mcp23017.h>
#include <semaphore.h>
//#include <sys/timerfd.h>


#include "DIOAPI/GeneralFunctions.h"
#include "DIOAPI/WDManager.h"
#include "DIOAPI/terminal.h"
#include "DIOAPI/Common.h"
#include "DIOAPI/Hardware.h"
#include "DIOAPI/DIO550.h"
#include "DIOAPI/CAN.h"
Char msgInitIODone[]   			= "\r INFO:  IO Card Initialized Successfuly.\n";
Char errInitIOError[]  			= "\r ERROR: IO Card Initialized with Errors.\n";

Char msgLCPInitDone[]			= "\r INFO:  LCPC Initialization is done successfully.\n\n";
Char errLCPInitError[]			= "\r ERROR: LCPC Initialization caused error.\n";

Char errHRTimedOut[]  			= "\r ERROR: Changed State HR Time Out = %d\n";
Char errCAN3Failure[]  			= "\r ERROR: CAN3 Failure ==> HR ByPass \n";
Char errIOCheckFail[]  			= "\r ERROR: Failed On IO Card Check\n";
Char msgChangCardState[]		= "\n\r\n >> Change State: %d => %d \n";
Char msgCANLoopBackStart[]		= "\r INFO: CAN_%d LOOPBACK\n";
Char errCANLoopBackFail[]		= "\r ERROR: Loopback Failed On CAN_%d!\n";
Char msgCANLoopBackDone[]		= "\r INFO: CAN LoopBack Check: Done.\n";
Char msgCANLoopBackCheck[]		= "\r\n\r INFO: CAN Loopback Checking ...\n";
Char errCANInitLoopBack[]		= "\r ERROR: Failed in CAN_Init for loopback with %d\n";
Char errCANInitNormal[]			= "\r ERROR: Failed in CAN_Init for normal operation with %d\n";
Char errCANLinkTimeOut[]		= "\r ERROR: CAN LINK TIME OUT{%d ms}!!\n";
Char msgMapcsVer1[]				= "\r INFO: MAPCS card version:VER1 \n";
Char msgMapcsVer2A[]			= "\r INFO: MAPCS card version:VER2A \n";
Char errMapcsVer[] 				= "\r ERROR: MAPCS Version did't intialized!\n";
Char msgHotActive[]				= "\r INFO: MAPCS Hot redundancy is active \n";
Char msgHotDeActive[]			= "\r INFO: MAPCS Hot redundancy is unactive \n";
Char errHotRedundancy[] 	    = "\r ERROR: Hot redundancy did't intialized!\n";

//unsigned char cnt = 0;
//unsigned char nIOCards = 0;
//
//int rs485_port = 0;
//int rs232_port = 0;

pthread_t tid[7]; 
struct timespec tm;  
int deviceHandle = 0;

int watchdogEnable = 0 ;  	// WDT is Disabled by default
//int watchdogtimeout  = 1;
///////// Device Type /////////
///////////////////////////////
int boardType = RTU_451;
///////////////////////////////

void timeTag();
unsigned char bitReverse(unsigned char x);

void signalHandler(int sigNo)
{
	//if (sigNo == SIGINT)
	printf("\033[1;36m");
	printf("\nSIGINT recieved...\nProgram terminated by user request.\n");
	printf("\033[0m");
	if (watchdogEnable != WDT_ENABLE)
	{
		write(deviceHandle, "V", 1);
		printf("\033[1;33m");
		printf("Watchdog has been disabled... (Development mode)\n");
		printf("\033[0m");
	}
		
	exit(0);
}

UChar Check_Mapcs_Ver(void)
{
	UChar Retval;
	Retval = RET_SUCCESS;
	FILE * file_pointer; 
	file_pointer = fopen("/etc/DIO560/Mapcs_Ver.txt", "r");    //reset the pointer
	if(file_pointer == NULL)
	{
		printf("Unable to open file.\n");
		printf("Please check you have read/write previleges.\n");
		Retval = ERR_TIMEOUT;
	}	
	Card.MAPCS_VER = getc(file_pointer) - '0';
//	printf("Read String |%d|\n", Card.MAPCS_VER);
	if (Retval == RET_SUCCESS)
	{
		if (Card.MAPCS_VER == MAPCS1)
			printf("%s", msgMapcsVer1);
		else if (Card.MAPCS_VER == MAPCS2A)
			printf("%s", msgMapcsVer2A);
		else
		{
			printf("%s", errMapcsVer); 
			Card.MAPCS_VER = MAPCS1;
		}
	} 
	return Retval;
}

UChar Check_HotRedundancyActive(void)
{
	UChar Retval;
	Retval = RET_SUCCESS;
/*****************************************************/
/******/	//for AIOs only
//	FILE * file_pointer; 
//	file_pointer = fopen("/etc/DIO560/HotRedundancyActive.txt", "r");      //reset the pointer
//	if(file_pointer == NULL)
//	{
//		printf("Unable to open file.\n");
//		printf("Please check you have read/write previleges.\n");
//		Retval = ERR_TIMEOUT;
//	}	
//	Card.HotRedundancyActive = getc(file_pointer) - '0';
/*****************************************************/	
	
/*****************************************************/	
//In state without HotRedundancy	
	Card.HotRedundancyActive = 0;	
/****************************************************/	
	
//	printf("Read String |%d|\n", Card.HotRedundancyActive);
	if (Retval == RET_SUCCESS)
	{
		if (Card.HotRedundancyActive == 1)
			printf("%s", msgHotActive);
		else if (Card.HotRedundancyActive == 0)
			printf("%s", msgHotDeActive);
		else
		{
			printf("%s", errHotRedundancy); 
			Card.HotRedundancyActive = 1;
			printf("%s", msgHotActive);
		}
	} 
	return Retval;  
}

/***********************************************************************************
*  Function   : Run ()
*----------------------------------------------------------------------------------
*  Object     : Data Conversion, IO Card Checking, Alert Management common in all
* 				input/output Cards
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None            |  ---
*------------------+-----------------------------------------------------------------
***********************************************************************************/
void Run(void)
{
	if (watchdogEnable == WDT_ENABLE)
		feedWD(deviceHandle);   // to kick watchdog and avoid watchdog reset in 0.5sec
	
	// there is an special status only in Output Cards, because their function is
	// more important than others, so there is a special CAN controller for them
	// and we check their status to shift in Hot Redundancy Switch mode or not
 
#if ( (CARD_TYPE == IOTYPE_AO672) || (CARD_TYPE == IOTYPE_AO673) )

    if(Card.HotRedundancyActive)
	{
		if (IOCard.HRSwitch == 1) // Hot Redundancy Switch
			{
				// CAN2 is reserved for Hot Redundancy Status in Output Cards
				if(IsCANBusAvailabe(CAN_CHIP2) == RET_SUCCESS)
				{
					Card.NewState = MAIN;
					ENABLE_OUTPUTS; 			// enable outputs in voltage to current conv.
					SetOutputs(); 			// set outputs based on CARD state
					printf(errHRTimedOut, (Card.HotRedundancyRxTimer + HOTREDUNDANCY_RX_INTERVAL));
					Card.HotRedundancyFailCounter = 0;
					Card.HotRedundancyTxTimer = 0;
					Card.HotRedundancyRxTimer = 0;
				}
				else
				{
					Card.LCPCError.Bit.Link3 = 1;  		// CAN2's connection error
					Card.HotRedundancyRxTimer = 0; 		// reset Rx Timer
					Card.HotRedundancyFailCounter = 0; 	// reset Failure Timer
					printf(errCAN3Failure);
				}
				IOCard.HRSwitch = 0;
			}
	}
#endif // CARD_TYPE == Output...
	
	Card.State = Card.NewState; 			//save new state of Card as current state
	
#if ( (CARD_TYPE != IOTYPE_ANALYZER) && (CARD_TYPE != IOTYPE_CALIBRATOR) )
	
		// if there is one of the working states for one of the IO Cards
		if(Card.State != OUT_OF_SERVICE)
	{
		// firstly check Local Processor temperature sensor
          
		LCPC_Check();	
                
		if (watchdogEnable == WDT_ENABLE)
			feedWD(deviceHandle);
		
		// Test the card and check the Diagnostics
		if(IOCard_Check() != RET_SUCCESS)
		{
			Card.NewState = OUT_OF_SERVICE;
			printf("%s",errIOCheckFail);
		}
		if (watchdogEnable == WDT_ENABLE)
			feedWD(deviceHandle);
		
		// Check if settings have changed:
		if(Card.NewSettingReceived)
		{
			Card.NewSettingReceived = 0;
			ApplyNewSetting();                      
		}
		
#ifdef HART_ENABLED
		//        BlockTransfer();
#endif
	}
	
#endif // if CARD_TYPE != ( Calibrator or Analyzer )
	
	if (watchdogEnable == WDT_ENABLE)
		feedWD(deviceHandle);
	Card.State = Card.NewState;
	
	RunCycle();
	
	if (Card.State != Card.PreviousState)
	{

		if (Card.IsCalibration == false)			
			printf(msgChangCardState, Card.PreviousState, Card.State);
		
		Card.LedBlinkCounter1s      = 0;
		Card.LedBlinkCounter500ms   = 0;
		Card.LedBlinkCounter200ms   = 0;
		
		Card.Timer2_CounterH = 0; //for Hot Redundancy
		
		Card.PreviousState = Card.State;
	}
    
}
/*************************************************************************************
*  Function   : CheckLoopback
*------------------------------------------------------------------------------------
*  Object     : To check CAN in loop back mode
*
*  Return     : 0
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None            |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
UChar CheckLoopback(void)
{
	UChar CS;
	
	//Start Loopback checking
	CAN_TxMsg.Command = FCP_CMD_RESERVED_1;
	CAN_TxMsg.DataLen = 8;

	for (UChar id = 0; id < 3; id++)
	{
		if ((id != 2) || ((id == 2)&&(Card.HotRedundancyActive)))
		{
			printf(msgCANLoopBackStart, id);
		
			if (id == 0)
				CS = CAN_CHIP0;
			else if (id == 1)
				CS = CAN_CHIP1;
			else
				CS = CAN_CHIP2;
		
			for (UChar group = 0; group < 2; group++)
			{
				CAN_TxMsg.Group = group;
				for (UChar address = 0; address < 64; address++)
				{
					CAN_TxMsg.Address = address;
				
					if (CAN_SendOn(CS, 0, 1) != RET_SUCCESS)
					{
						if (watchdogEnable == WDT_ENABLE)
							feedWD(deviceHandle);
						return 1 ;
					}
				
					if (watchdogEnable == WDT_ENABLE)
						feedWD(deviceHandle);
					delayMicroseconds(1000);  //1000 us delayMicroseconds
					if (watchdogEnable == WDT_ENABLE)
						feedWD(deviceHandle);
				
					if (UnwantedPacketReceived == 1)
					{
						Card.NewState = OUT_OF_SERVICE;
						printf(errCANLoopBackFail, id);
						UnwantedPacketReceived = 0;
					}
				}
			}
		}
	}
	printf("%s",msgCANLoopBackDone);
	
	return 0 ;
}
void TestIODef(void)
{
	LED1_OUTPUT_DEF;
	LED2_OUTPUT_DEF;
	LED3_OUTPUT_DEF;
	LED4_OUTPUT_DEF;
	SET_LED1_LOW;
	SET_LED2_LOW;
	SET_LED3_LOW;
	SET_LED4_LOW;
	BP_ADDR0_DEF;
	BP_ADDR1_DEF;
	BP_ADDR2_DEF;
	BP_ADDR3_DEF;
	BP_ADDR4_DEF;
	BP_ADDR5_DEF;
	
	BOARD_ADRRESS_1_DEF;
	BOARD_ADRRESS_2_DEF;
	
	BOARD_ADRRESS_1_HIGH;
	BOARD_ADRRESS_2_HIGH;
	
//	INPUT_DATA_BYTE_DEF;
//	OUTPUT_DATA_BYTE_DEF;
	
	GROUP1_SELECT_IN_DEF;
	GROUP2_SELECT_IN_DEF;
	GROUP1_SELECT_OUT_DEF;
	
	GROUP1_SELECT_IN_HIGH;
	GROUP2_SELECT_IN_HIGH;
	GROUP1_SELECT_OUT_LOW;
	
	FIELD_POWER1_DEF;
	FIELD_POWER2_DEF;
	FIELD_POWER3_DEF;
	FIELD_POWER4_DEF;
	STATUS1_DEF;
	
	OUTPUT_ENABLE_DEF;
	OUTPUT_DISABLE;
}

/*************************************************************************************
*  Function   : CalcTimer4value()
*------------------------------------------------------------------------------------
*  Object     : This function sets the timer4 settings. 
*               
*
*  Return     : None. 
*
*  Note(s)    : 
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*************************************************************************************/
void CalcTimer4Value(UChar SampleCount)
{
#if(CARD_TYPE==IOTYPE_DI530)
	{
																					// SampleCount*500*TimeTrigCount*1uS Refresh
	}  
#elif(CARD_TYPE==IOTYPE_DI531)
	{
																      					// SampleCount*500*TimeTrigCount*1uS Refresh
	}
#elif(CARD_TYPE==IOTYPE_DIO550)
	{
																					// SampleCount*500*TimeTrigCount*1uS Refresh
	}    
#elif(CARD_TYPE==IOTYPE_AI622)
	{
																				   // SampleCount*1250*TimeTrigCount*1uS Refresh
	      
	}
#elif(CARD_TYPE==IOTYPE_AI623)
	{ 
																				  // SampleCount*1250*TimeTrigCount*1uS Refresh
		
	}
#elif(CARD_TYPE==IOTYPE_AI624)
	{ 
																				  // SampleCount*1250*TimeTrigCount*1uS Refresh
		
	}    
#endif
	//printf_P("c=%x ",CARD_TYPE);  
	//printf_P("%x ",TCNT4L);
	//printf_P("%x \r\n",TCNT4H);
}




/***********************************************************************************
*  Function   : main ()
*----------------------------------------------------------------------------------
*  Object     : main loop of project
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None            |  ---
*------------------+-----------------------------------------------------------------
***********************************************************************************/
int main(int argc, char *argv[])
{
	UChar res;
	

	

//	UChar DVal;
//	// Required variables for the strategy 
//	unsigned int AI_Val10[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
//
//	unsigned char DO_Val03[4] = { 0xAA, 0x55, 0x00, 0x00 };
//	unsigned char DI_Val02[4] = { 0x00, 0x00, 0x00, 0x00 };
	
//	struct DO_Settings DO_defaultSettings;
//	struct DI_Settings DI_defaultSettings;
//
//	struct AO_Settings AO_defaultSettings;
//	struct AI_Settings AI_defaultSettings;
//
//	struct TI_Settings TI_defaultSettings;

//	struct IOCardStatus curIOStatus[MAX_NUM_IO_CARDS];
//	char Hardware[10], HWrevision[10], HWserial[20];

	printf("\033[1;32m");
	printf("\n ***** DIO560 Started ***** \n");
	printf("\033[0m");
	
	wiringPiSetup();
	wiringPiSetupGpio();
//#ifdef DIO_560
//	mcp23017Setup(PIN_Base_560 , 0x20);
//#endif //
	
//	LCPC_Check();
//	Check_Mapcs_Ver();
//	Check_HotRedundancyActive();
	if(argc > 1) 
	{
		if (!strncasecmp(argv[1], "-wd", 3)) 
		{
			watchdogEnable = WDT_ENABLE;
			printf("\033[1;31m");
			printf("\n** NOTE: The program will be run with ENABLED watchdog. **\n");
		}
	}

	// ----------------------------- Watchdog ---------------------------------
	if(watchdogEnable == WDT_ENABLE)
	{
		deviceHandle = initWD("/dev/watchdog");
		if (deviceHandle < 0)
		{
			printf("\n Watchdog initialization error!!!\n\n");
		}
		else
		{
			if (setTimeoutWD(deviceHandle, WDT_TIMEOUT) != -1)
				printf("Watchdog initialized. --> Timeout has been sat to %d seconds.\n", WDT_TIMEOUT);
			printf("\033[0m");
		}
	}
	else
	{
		printf("\033[1;33m");
		printf("\n> NOTE: RTU was run with disabled watchdog. **\n\n");
		printf("\033[0m");
	}
	
	printf("\033[1;35m");
	printf("> Device Model:\t\t%d\n", getDeviceModel());
	printf("> Device Processor:\t%d\n", getDeviceProcessor());
	printf("> Device Manufacturer:\t%d\n\n", getDeviceManufacturer());
	
	printf("\033[0m");
	
		
	
//	CAN0_LOOPBACK_DEF;
//	CAN1_LOOPBACK_DEF;
	//	CAN2_LOOPBACK_DEF;
	
	CAN0_LOOPBACK_ENABLE;
	CAN1_LOOPBACK_ENABLE;
	//	CAN2_LOOPBACK_ENABLE;
	
/**************************************************************************/
//	sem_init(&canerr, 0, 0);
//	sem_init(&canrec, 0, 0);
//	sem_init(&cansend, 0, 0);
//	sem_init(&bashIN, 0, 0);
//	sem_init(&timer0, 0, 0);
//	sem_init(&timer2, 0, 0);
//	sem_init(&timer4, 0, 0);
//	sem_init(&can1_En, 0, 0);
//	sem_init(&can0_En, 0, 0);
//	sem_init(&can_En, 0, 0);
//	
//	// Initialize LCPC Card:
//	if(LCPC_Init(CARD_TYPE) == RET_SUCCESS)
//	    printf("%s", msgLCPInitDone);
//	else
//	{
//		printf("%s", errLCPInitError);
//		Card.NewState = OUT_OF_SERVICE;
//		
//		// FOR DEBUG ONLY: to keep the sequence of errors in case they occured 
//        printf("\r Fail_1\n");
//	}
//	
//	if (watchdogEnable == WDT_ENABLE)
//		feedWD(deviceHandle);
//	
//	LCPC_Check();
//	
//	if (watchdogEnable == WDT_ENABLE)
//		feedWD(deviceHandle);
//	
//	IOCard.IOError.D8           = 0;
//	IOCard.PreviousIOError.D8   = 0;
//	IOCard.IOWarning.D8         = 0;
//	IOCard.PreviousIOWarning.D8 = 0;
//	Card.IsCalibration = false;
//	
//	// Initialize IO Card:
//	if(IOCard_Init() == RET_SUCCESS)
//	    printf("%s", msgInitIODone);
//	else
//	{
//		printf("%s", errInitIOError);
//		Card.NewState = OUT_OF_SERVICE;
//		
//		// FOR DEBUG ONLY: to keep the sequence of errors in case they occured  
//        printf("%s", "\r Fail_2\n");
//	}
//	
	//============================================================
	//  CAN Loopback check
	//============================================================
	
    //if ( (CARD_TYPE == IOTYPE_AO672) || (CARD_TYPE == IOTYPE_AO673) )
    Check_HotRedundancyActive(); 
    
	printf("%s",msgCANLoopBackCheck);
	if (watchdogEnable == WDT_ENABLE)
		feedWD(deviceHandle);
	res = CAN_Init(MODE_LOOPBACK);
	if (res != 0)
	{
		printf(errCANInitLoopBack, res);
		Card.NewState = OUT_OF_SERVICE;
		
		// FOR DEBUG ONLY: to keep the sequence of errors in case they occured 
        printf("\r Fail_3\n");
	}
	else
	{
		CheckLoopback();
	}
	
	if (watchdogEnable == WDT_ENABLE)
		feedWD(deviceHandle);
	
	if (Card.NewState != OUT_OF_SERVICE)
	{
		if (watchdogEnable == WDT_ENABLE)
			feedWD(deviceHandle);
		
		if (CAN_Init(MODE_NORMAL) != 0)
		{
			printf(errCANInitNormal, res);
			Card.NewState = OUT_OF_SERVICE;
			
			// FOR DEBUG ONLY: to keep the sequence of errors in case they occured 
            printf("\r Fail_4\n");
		}
	}
	
	if (watchdogEnable == WDT_ENABLE)
		feedWD(deviceHandle);
	
#if(CARD_TYPE==IOTYPE_DI530)
	{
		IOCard.Timer4SampleCount = 1;
		CalcTimer4Value(IOCard.Timer4SampleCount);
		SetTimer4(); //set timer for sampling  
	}
#elif(CARD_TYPE==IOTYPE_DI531)
	{
		IOCard.Timer4SampleCount = 1;
		CalcTimer4Value(IOCard.Timer4SampleCount);
		SetTimer4(); //set timer for sampling  
	}
#elif(CARD_TYPE==IOTYPE_DIO550)
	{
		IOCard.Timer4SampleCount = 1;
		CalcTimer4Value(IOCard.Timer4SampleCount);
		SetTimer4(); //set timer for sampling    
	}    
#elif(CARD_TYPE==IOTYPE_AI622)
	{
		IOCard.Timer4SampleCount = 5;
		CalcTimer4Value(IOCard.Timer4SampleCount);
		SetTimer4(); //set timer for sampling
	}
#elif(CARD_TYPE==IOTYPE_AI623)
	{ 
		IOCard.Timer4SampleCount = 5;
		CalcTimer4Value(IOCard.Timer4SampleCount);	
		SetTimer4(); //set timer for sampling
	} 
#elif(CARD_TYPE==IOTYPE_AI624)
	{ 
		IOCard.Timer4SampleCount = 5;
		CalcTimer4Value(IOCard.Timer4SampleCount);	
		SetTimer4(); //set timer for sampling
	}     
#endif	   
	Check_Mapcs_Ver();

    
	Run();

	if (watchdogEnable == WDT_ENABLE)
		feedWD(deviceHandle);
	
	while (true)
	{
		if (watchdogEnable == WDT_ENABLE)
			feedWD(deviceHandle);
		
#if ( CARD_TYPE == IOTYPE_AI621 )
		if (Card.LED_Update_Flag == true)
		{
			LEDsUpdater();
			Card.LED_Update_Flag = false;
		}
#endif
		if (Card.RxReady == 1)
		{
			// FOR DEBUG ONLY: to print out Card.State and CAN received Command 
            //printf_P( "\r {%d} RxReady 0x%X\n", Card.State, CAN_RxMsg.Command );
			
            SET_CAN_LED_BLINK;
			
			DoOnRxReady(CAN_RxMsg.Command, CAN_RxMsg.DataLen);
			Card.RxReady = 0;
		}
		else
		{
			RESET_CAN_LED_BLINK;
			if ((Card.State == MAIN) || (Card.State == BACKUP))
			{
				// CAN Communication activity check   
#ifndef HART_TEST // if ------NOT DEFINED------ HART_TEST 
								if(Card.IsCalibration == false)			
									if(Card.Timer2_CounterA >= MIN_LINK_TIMEOUT)
				{
					// if CAN LINK TIMER timed-out on both Links, Card will change it's 
					// own state to FAIL-SAFE (sth happened to both CAN-LINK, so IO-CARD
					// cannot get its new state from CAN anymore).
					if(Card.State == MAIN)
						Card.NewState = FAIL_SAFE_MAIN;
					else
						Card.NewState = FAIL_SAFE_BACKUP;
						
					printf(errCANLinkTimeOut, Card.Timer2_CounterA);
					CAN_ExpectedReceiveCount = 0;
				}
#endif
			}
			else
			{
				if (watchdogEnable == WDT_ENABLE)
					feedWD(deviceHandle);
				delayMicroseconds(100); 
				if (watchdogEnable == WDT_ENABLE)
					feedWD(deviceHandle);
			}
			
#ifdef  HART_ENABLED
			
#ifdef HART_TEST
			Card.NewState = MAIN;
#endif
			if ((Card.State == MAIN) && (Card.PreviousState == MAIN))
			{
				HART_MainStateMachine();
			}
#endif
            
			if (Card.IsCalibration == true)			
				Card.NewState = MAIN;
		}
		if (watchdogEnable == WDT_ENABLE)
			feedWD(deviceHandle);
		Run();
	}
//**********************************************************************************/
	
	
//	Run();
	
//	// wiringPi initiation
//	if(initGPIO(boardType) == ERROR)
//	{
//		printf("Error while initiating GPIO!");
//		return 0;
//	}
	
//	IOsetLED(RUN_LED, LED_ON);
//	delayMicroseconds(1000);
//	IOsetLED(STBY_LED, LED_ON);
//	delayMicroseconds(1000);
//	IOsetLED(STATUS_LED, LED_ON);
//	delayMicroseconds(1000);
//	IOsetLED(FLT_LED, LED_ON);
//
//	delayMicroseconds(500);
//
//	IOsetLED(RUN_LED, LED_OFF);
//	IOsetLED(STBY_LED, LED_OFF);
//	IOsetLED(STATUS_LED, LED_OFF);
//	IOsetLED(FLT_LED, LED_OFF);

	// Initiate the system
//	if(sysInit() == ERROR)
//	{
//		printf("System Initialization Failed...\n");
//		return (-1);
//	}

	//	return 0;
	
//	TestIODef();
//	
//	if (LCPC_Init(CARD_TYPE) == RET_SUCCESS)
//		printf("%s",msgLCPInitDone);
//	else
//	{
//		printf("%s",errLCPInitError);
//		Card.NewState = OUT_OF_SERVICE;
//		
//		// FOR DEBUG ONLY: to keep the sequence of errors in case they occured 
//        printf("\r Fail_1\n");
//	}
//	
	////////////////////////////////////////////////////////
//	    			Main program loop                 //
////////////////////////////////////////////////////////
//	while(1)
//	{
//		if (watchdogEnable == WDT_ENABLE)
//			feedWD(deviceHandle);
		
//		if (!watchdogEnable)
//		{
//			printf("\r Disable watchdog.\n");
//			write(deviceHandle, "V", 1);
//		}
//		// if time tag is needed this function should be uncommented
//		//timeTag();
//
//		Sync(); 		// Sync command to all I/O cards

		//serialPrintf(rs485_port, "DO_Val = %d \r\n", DO_Val03[1]);
		//serialPrintf(rs232_port, "DO_Val(reversed) = %d \r\n", DO_Val31[0]);

//		if(getCardsStatus(pluggedCards, curIOStatus))   // Get all cards status
//		{
//			IOsetLED(FLT_LED, LED_ON);
//			//printf("Check the HW configurations!\n");
//			//return ERROR;
//		}
//		else
//		{
//			IOsetLED(FLT_LED, LED_OFF);
//		}

		//MPUsetLED(STBY_LED, cnt & 0x10);
//		IOsetLED(RUN_LED, !(cnt & 0x01));
		//MPUsetLED(STBY_LED, !(cnt & 0x02));
		//MPUsetLED(FLT_LED, !(cnt & 0x04));
		//MPUsetLED(STATUS_LED, !(cnt & 0x08));

		// Interpret the cards status and fix probable issues on the cards state.
//		fixCardsState(nIOCards, curIOStatus, DI_defaultSettings, DO_defaultSettings, AI_defaultSettings, TI_defaultSettings, AO_defaultSettings);
//
//		// ---------- Access the IOs ----------------
//		// Read AI cards input
//		AI_GetValues(1, 0, AI_Val10);   // TI

		// Set DO cards output
//		DO_SetValues(1, 1, DO_Val03);
//
//		// Read DI cards input
//		DI_GetValues(1, 2, DI_Val02);

		//-------------------------------------------
//		for (int i = 0; i < 4; i++)
//			printf("  %X  ", DI_Val02[i]);
//		printf("\n");

		// User strategy shoule be placed here //
		// e.g. A simple Strategy would be:
//		DO_Val03[0] = DI_Val02[0];
//		DO_Val03[1] = cnt;
//		//-------------------------------------------//
//
//		cnt++;
//		//MPUsetLED(STAT_LED_2, cnt & 0x01);
//
//		delayMicroseconds(100000);   //100 ms
		
		
//		/*Test Routine for IOs
//		 **/
//		
//		//LED
//        SET_LED1_HIGH;
//		SET_LED2_HIGH;
//		SET_LED3_HIGH;
//		SET_LED4_HIGH;
//		
//		delayMicroseconds(100);
//		
//		SET_LED1_LOW;
//		SET_LED2_LOW;
//		SET_LED3_LOW;
//		SET_LED4_LOW;
//		delayMicroseconds(100);

//		
		//STATUS
//		printf(BGCYAN);
//		delayMicroseconds(200);
//		DVal = ReadDOStatus();
//		printf(FGRED);
//		printf("DOStatus is :%d\n", DVal);
//		
//		//FieldPower
//		delayMicroseconds(200);
//		DVal = ReadFieldPowerStatus();
//		printf(ATTRESET);
//		printf("FieldPowerStatus is :%d\n", DVal);
//		
//		
//		//OUTPUT
//		OUTPUT_DATA_BYTE_DEF;
//		GROUP1_SELECT_IN_HIGH;
//		GROUP2_SELECT_IN_HIGH;
//		BOARD_ADRRESS_1_HIGH;
//		BOARD_ADRRESS_2_HIGH;
//		GROUP1_SELECT_OUT_HIGH;
//		delayMicroseconds (1);
//		DVal = 0xAA;
////		DVal = 0x55;
////		DVal = 0x00;
////		DVal = 0xFF;
//		OUTPUT_ENABLE;
//		SetOUTData(DVal);
//		delayMicroseconds (1);
////		digitalWrite(D7, HIGH);
//		BOARD_ADRRESS_1_HIGH;
//		BOARD_ADRRESS_2_HIGH;
//		GROUP1_SELECT_OUT_LOW;
//
//		
//		
////		//Address
////		
////		BOARD_ADRRESS_2_HIGH;
////		BOARD_ADRRESS_1_LOW;
////		GROUP1_SELECT_OUT_LOW;
////		OUTPUT_DISABLE;
////		GROUP2_SELECT_IN_HIGH;       
////		GROUP1_SELECT_IN_HIGH;
////		delayMicroseconds (1);
////		DAdd = ~ReadINData();
////		BOARD_ADRRESS_1_HIGH;
////		BOARD_ADRRESS_2_LOW;
////		delayMicroseconds (1);
////		DAdd = ((~ReadINData()) << 8) | DAdd;
////		BOARD_ADRRESS_2_HIGH;
////		printf(FGYELLOW);
////		printf("CardAddress is :%lu\n", DAdd);
////		printf(ATTRESET);
//		
//		
//		
//		
//		//Data IN
//		OUTPUT_DISABLE;
////		CaptureInputs();
//		
////		GROUP2_SELECT_IN_HIGH;       
////		GROUP1_SELECT_IN_LOW;
////		delayMicroseconds (1);
////		DIn = ~ReadINData();
////		GROUP1_SELECT_IN_HIGH;
////		
////		GROUP1_SELECT_IN_HIGH;       
////		GROUP2_SELECT_IN_LOW;
////		delayMicroseconds (1);
////		DIn = ((~ReadINData()) << 8) | DIn;
////		GROUP2_SELECT_IN_HIGH;
//
////		GROUP1_SELECT_IN_HIGH;   
////		GROUP2_SELECT_IN_LOW;
//		delayMicroseconds (1);
//		CaptureInputs();
//		DIn = IOCard.Inputs.D32;
//		
//		delayMicroseconds(200);
//		printf(FGGREEN);
//		printf("IOCardDataIn is :%lu\n", DIn);
//		printf(ATTRESET);
//		fflush(stdout);

//	}
//
//	return SUCCESS;
}

