/***********************************************************************************
*  File      : Config.h
*----------------------------------------------------------------------------------
*  Object    : it defines all Card-related constants
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

#ifndef __CONFIG_H_
#define __CONFIG_H_

/***********************************************************************************
*  Type Definitions
***********************************************************************************/
typedef unsigned char   UChar;           // unsigned  8  bit
typedef unsigned int    UInt ;           // unsigned  16 bit
typedef unsigned long   ULong ;          // unsigned  32 bit
typedef char            Char;            // unSigned  8  bit
typedef int             Int;             // unSigned  16 bit
typedef long            Long;            // unSigned  32 bit
typedef signed char     SChar;			 // Signed     8 bit
typedef signed int      SInt;            // Signed    16 bit
typedef signed long     SLong;           // Signed    32 bit

/***********************************************************************************
*  General Definitions
***********************************************************************************/
//#define		TRUE	1
//#define		FALSE   0
#define		HIGH	1
#define		LOW	    0
#define     SUCC    1
#define     FAIL    0

/***********************************************************************************
*  IO Card Type, to be used in CAN Protocol
***********************************************************************************/
#define	IOTYPE_DI530	0x28   // DI530, 32ch, Digital In,  Group Based Diagnos
#define	IOTYPE_DI531	0x29   // DI531, 32ch, Digital In,  Group Based Diagnos
#define IOTYPE_DO580	0x48   // DO580, 32ch, Digital Out, Group Based Diagnos
#define IOTYPE_DO571	0x58   // DO571, 16ch, Digital Out, Channel Based Diagnos
#define IOTYPE_AI621	0x68   // AI621, 8ch,  Temperature In
#define IOTYPE_AI622	0xB8   // AI622, 8ch,  Analogue In,  4-20mA
#define IOTYPE_AI623	0x98   // AI623, 8ch,  Analogue In,  4-20mA, 0-10V, HART
#define IOTYPE_AO672	0xD8   // AO672, 8ch,  Analogue Out, 4-20mA
#define IOTYPE_AO673	0x88   // AO673, 8ch,  Analogue Out, 4-20mA, 0-10V, HART
#define	IOTYPE_DIO550	0x38   // DIO550, 10ch OUT, 20ch IN, Digital InOut,  Group Based Diagnos
#define IOTYPE_AI624	0xA8   // AI624, 8ch,  diff Analogue In,  4-20mA, 0-10V, HART
#define IOTYPE_DO591	0xC8   // DO580, 16ch, Digital Out,DryContact,No diagnostic
/***********************************************************************************
*  IO Card Code
***********************************************************************************/
// Bit(0-2):Revision, Bit(3-5):Version, Bit(6-8):CardType2,  Bit(9-10):CardType1
#define	DI530_CODE		0x244	
#define	DI531_CODE		0x244	//should change according hardware 
#define	DO580_CODE	    0x044	
#define	DO591_CODE	    0x0C0
#define DO571_CODE      0x083

#define	AO672_CODE	    0x486	
#define	AO673_CODE	    0xB10

#define	AI621_CODE	    0x704
#define	AI622_CODE	    0x6C5
#define	AI623_CODE	    0xA10
#define	AI624_CODE	    0x740
#define	DIO550_CODE	    0x3C0	
//  NOTE-------------NOTE--------------NOTE: deprecated	
////----------------------------------------------------------------------------------
//// NOTE NOTE:	Modify this line if IO card has been added or the hard wired type
////      		definer is changed.
////----------------------------------------------------------------------------------
//#define	BOOTLOAD_CODE		  0

//----------------------------------------------------------------------------------
// NOTE NOTE:	Modify this line based on the chosen card type.
//----------------------------------------------------------------------------------
#define CardTypeSelection  11

/***********************************************************************************
*  Basic Card Definitions e.g its CODE, MODULE, ... based on its TYPE
***********************************************************************************/
#if   CardTypeSelection == 1
	#define CARD_CODE				DI530_CODE
	#define CARD_TYPE				IOTYPE_DI530
	#define ModuleTypeSelection   0
	#define IS_INPUT    			1
#elif CardTypeSelection == 2
	#define CARD_CODE				DO580_CODE	
	#define CARD_TYPE				IOTYPE_DO580
	#define ModuleTypeSelection   0
#elif CardTypeSelection == 3
	#define CARD_CODE				AI622_CODE
	#define CARD_TYPE				IOTYPE_AI622
	#define ModuleTypeSelection   1
	#define IS_INPUT    			1
#elif CardTypeSelection == 4
	#define CARD_CODE				AO672_CODE
	#define CARD_TYPE				IOTYPE_AO672
	#define ModuleTypeSelection   1
#elif CardTypeSelection == 5
	#define CARD_CODE				AI621_CODE	
	#define CARD_TYPE				IOTYPE_AI621
	#define ModuleTypeSelection   0
	#define IS_INPUT    			1
#elif CardTypeSelection == 6
	#define HART_ENABLED
	#define CARD_CODE				AI623_CODE
	#define CARD_TYPE				IOTYPE_AI623
	//#define HART_TEST             // Defined for testing dummy commands from MPU
	//#define DEBUG_HART            // Defined for printing HART related logs
	#define ModuleTypeSelection  	0
	#define IS_INPUT    			1
#elif CardTypeSelection == 7
	#define HART_ENABLED
	#define CARD_CODE				AO673_CODE
	#define CARD_TYPE				IOTYPE_AO673
	//#define HART_TEST             // Defined for testing dummy commands from MPU
	//#define DEBUG_HART            // Defined for printing HART related logs
	#define ModuleTypeSelection   0
#elif CardTypeSelection == 9
	#define CARD_CODE				DO571_CODE
	#define CARD_TYPE				IOTYPE_DO571
	#define ModuleTypeSelection   0
#elif   CardTypeSelection == 10
	#define CARD_CODE				DI531_CODE
	#define CARD_TYPE				IOTYPE_DI531
	#define ModuleTypeSelection   0
	#define IS_INPUT                        1 
#elif   CardTypeSelection == 11
	#define CARD_CODE				DIO550_CODE
	#define CARD_TYPE				IOTYPE_DIO550
	#define ModuleTypeSelection   0
	#define IS_INOUT                        0
#elif CardTypeSelection == 12
	#define CARD_CODE				DO591_CODE	
	#define CARD_TYPE				IOTYPE_DO591
	#define ModuleTypeSelection   0
#elif CardTypeSelection == 13
	//#define HART_ENABLED
	#define CARD_CODE				AI624_CODE
	#define CARD_TYPE				IOTYPE_AI624
	//#define HART_TEST             // Defined for testing dummy commands from MPU
	//#define DEBUG_HART            // Defined for printing HART related logs
	#define ModuleTypeSelection  	1
	#define IS_INPUT    			1
//  NOTE-------------NOTE--------------NOTE: deprecated	
//#elif CardTypeSelection == 0
//	#define CARD_CODE				BOOTLOAD_CODE   // The output file is generated
	// // for underneath Bootloadr!
#endif

/***********************************************************************************
*  Module Type Selection base on its Card Type to add an optional hardware feature
***********************************************************************************/
// for example in DI530 schematic, page Address, part U23, thare are two defined pins
// i.e. pin B7 & B8 which are used to make 4 possible option (0-3), based on which 
// the user can add optional hardware feature on board later, i.e. to add HART feature 
// for AI622 or AO672
#define FIRST_MODULE_TYPE	0
#define SECOND_MODULE_TYPE	1
#define THIRD_MODULE_TYPE	2
#define FORTH_MODULE_TYPE	3

#if  ModuleTypeSelection == 0	        // 1st Module type based on user optinal feature
	#define MODULETYPE	FIRST_MODULE_TYPE
#elif  ModuleTypeSelection == 1	        // 2nd Module type based on user optinal feature
	#define MODULETYPE	SECOND_MODULE_TYPE
#elif  ModuleTypeSelection == 2	        // 3rd Module type based on user optinal feature
	#define MODULETYPE	THIRD_MODULE_TYPE
#elif  ModuleTypeSelection == 3	        // 4th Module type based on user optinal feature
	#define MODULETYPE	FORTH_MODULE_TYPE
#endif

//  NOTE-------------NOTE--------------NOTE: deprecated	
////----------------------------------------------------------------------------------
//// NOTE NOTE:	Modify this line based on EEPROM chip chosen for the LCPC
////				0: Internal E2PROM 		1: External E2PROM
////----------------------------------------------------------------------------------
//#define  E2PROMType    		1

//----------------------------------------------------------------------------------
// NOTE NOTE:	Modify this line based on Current Firmware version
//				Bit(0-2):Revision,	Bit(3-5):Version,	Bit(6-7):Reserved
//----------------------------------------------------------------------------------
#define FIRMWARE_VERSION     0x09	

#endif  //__CONFIG_H_

//***********************************************************************************
// No More
//***********************************************************************************

