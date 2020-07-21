/***********************************************************************************
*  File      : Common.h
*----------------------------------------------------------------------------------
*  Object    : it defines all Card-related variables and some constants
*
*  Contents  : required Card Type, Module Type, E2PROM type, debug's console prints,
*			   Configurations and Definitions
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
//
#ifndef __COMMON_H_
				  
#define __COMMON_H_
//#pragma  once

/***********************************************************************************
 *  Header files
 ***********************************************************************************/
//#include <intrinsics.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
//#include <pgmspace.h>
#include <string.h>

#include "Config.h"			// global configuration header file
#include "Hardware.h"
#include "LCPC.h"		// local processor (ATMEGA1280) header file
#include "DIO550.h"
/***********************************************************************************
 *  Global variables
 ***********************************************************************************/
// these 3 variables are different for different IO cards
extern union  SettingUnion  Setting;
extern struct IOCardStruct	IOCard;
extern struct PCCardStruct  Card;
/***********************************************************************************
 *  Common definitions/messages between different IO Cards
 ***********************************************************************************/
// size of Card Setting, to be based on varied setting union size
#define SIZE_OF_SETTING			( sizeof( union SettingUnion ) )

// these definitions are used to show 0: received setting from MPU is OK, 1: its size
// is invalid or 2: it is basically invalid (in Can.c file)
#define RET_SETTING_OK              0
#define RET_SETTING_INVALID_SIZE    1
#define RET_SETTING_INVALID         2

// these definitions are used in DataBusSelect() subroutine 
// DataBusSelect() has different declarations in different IO files!
#define DESELECT_ALL            0
#define READ_BOARD_ADDRESS1     1
#define READ_BOARD_ADDRESS2     2
#define SET_AO_LEDs             3
#define READ_OUTPUT_STATUS      4
#define GROUP1_SELECT       5
#define GROUP2_SELECT       6
#define GROUP3_SELECT       7
#define GROUP4_SELECT       8
#define GROUP1_OUT_SELECT       5
#define GROUP2_OUT_SELECT       6
#define GROUP3_OUT_SELECT       7
#define GROUP4_OUT_SELECT       8
#define READ_OUTPUT_STATUS1     9
#define READ_OUTPUT_STATUS2     10
#define SET_DO_LEDs_1           11
#define SET_DO_LEDs_2           12

/***********************************************************************************
 *  Check the IO card type and include the required header file
 ***********************************************************************************/
//#if   ( CARD_TYPE == IOTYPE_DI530 ) 
//    #include "DI530.h"
//#elif ( CARD_TYPE == IOTYPE_DI531 )
//    #include "DI531.h"
//#elif ( CARD_TYPE == IOTYPE_DO580 )
//    #include "DO580.h"
//#elif ( CARD_TYPE == IOTYPE_DO571 )
//    #include "DO571.h"
//#elif ( CARD_TYPE == IOTYPE_AI622 )
//    #include "AI622.h"
//#elif ( CARD_TYPE == IOTYPE_AO672 )
//    #include "AO672.h"
//#elif ( CARD_TYPE == IOTYPE_AI621 )
//    #include "AI621.h"
//#elif ( CARD_TYPE == IOTYPE_AI623 )
//    #include "AI623.h"
//#elif ( CARD_TYPE == IOTYPE_AI624 )
//    #include "AI624.h"
//#elif ( CARD_TYPE == IOTYPE_AO673 )
//    #include "AO673.h"
//#elif ( CARD_TYPE == IOTYPE_DIO550 )
//    #include "DIO550.h"
//#elif ( CARD_TYPE == IOTYPE_DO591 )
//    #include "DO591.h"
//#endif


/***********************************************************************************
 *  Common Subroutine Declarations in different IO Cards
 ***********************************************************************************/
UChar   IOCard_Init(void);
UChar   IOCard_Check( void );
void    RunCycle( void );

void    DefineSelectPins( void );
void    DataBusSelect( UChar Select);

// both of them are used in CAN interface
UChar   ProcessMessage( UChar rxCommand, UChar dataLen );
void    DoOnRxReady( UChar rxCommand, UChar dataLen );

// both of them are used in apply new setting and setting check (received from MPU)
UChar   IsValidSetting( void ) ;
void    ApplyNewSetting( void );
UChar Check_Mapcs_Ver(void);
UChar Check_HotRedundancyActive(void);
#endif  //__COMMON_H_

//***********************************************************************************
// No More
//***********************************************************************************