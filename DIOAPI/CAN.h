/***********************************************************************************
*  File      : CAN.h
*----------------------------------------------------------------------------------
*  Object    : This file contains required CAN's definitions.
*
*  Contents  : global variables and definitions
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

#ifndef __CAN_H_
#define __CAN_H_

/***********************************************************************************
*  General Header files
***********************************************************************************/
#include "Common.h"
#include "MCP2515.h"
#include "HART.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/can/error.h>
#include "lib.h"

Int IOCAN_Socket; 	//can0
Int CPUCAN_Socket; 	//can1
Int errCnt;
sem_t canerr, canrec, cansend, bashIN, timer0, timer2, timer4; 
sem_t	can_En, can0_En, can1_En;
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
           CAN Hardware related Definitions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* first 2 CAN interrupts should have highest priority than other external interrupts*/
//#define	CAN0_IE                 PORTD2  // Assigned hardware pinout
//#define CAN1_IE                 PORTD3  // Assigned hardware pinout
//#define CAN2_IE                 PORTE6  // Assigned hardware pinout
//
//#define CAN_SPI_SS              PORTB0  // Assigned hardware pinout
//#define CAN_SPI_SCK             PORTB1  // Assigned hardware pinout
//#define CAN_SPI_MOSI            PORTB2  // Assigned hardware pinout
//
#define CAN_CS0                 0  // Assigned hardware pinout
#define CAN_CS1                 1  // Assigned hardware pinout
#define CAN_CS2                 2  // Assigned hardware pinout
//
//// low level sensitive
//#define CAN0_INTTYPE_DEF		EICRA &= ~(1 << ISC21);  EICRA &= ~(1 << ISC20);
//#define CAN1_INTTYPE_DEF		EICRA &= ~(1 << ISC31);  EICRA &= ~(1 << ISC30);
//#define CAN2_INTTYPE_DEF		EICRB &= ~(1 << ISC31);  EICRB &= ~(1 << ISC30);
//
//#define CAN0_INTERRUPT_VECT   	INT2_vect
//#define CAN1_INTERRUPT_VECT   	INT3_vect
//#define CAN2_INTERRUPT_VECT   	INT6_vect

// #define	Jump2BootloaderProgram	asm("CLI") // deprecated

#define CAN_CHIP0               (1 << CAN_CS0)
#define CAN_CHIP1               (1 << CAN_CS1)
#define CAN_CHIP2               (1 << CAN_CS2)

//#define SPI_SS                  (1 << CAN_SPI_SS)
//#define SPI_SCK                 (1 << CAN_SPI_SCK)
//#define SPI_MOSI                (1 << CAN_SPI_MOSI)
//
//#define MCP_SPI_INIT   DDRB  |= SPI_SS|SPI_SCK|SPI_MOSI|CAN_CHIP0|CAN_CHIP1|CAN_CHIP2
//#define MCP_SPI_MODE_INIT			SPCR = NULL
//#define MCP_SPI_SETTING_INIT		SPSR = NULL
//	
//#define MCP_SPI_MODE_ENABLE			SPCR |= (1 << SPE) | (1 << MSTR);   // Enable SPI, Configure in Master mode	 		
//#define MCP_SPI_SETTING_SET			SPSR |= (1 << SPI2X);               // Double rate the SPI clock	
//
//#define MCP_SPI_DATA				SPDR
//#define MCP_SPI_FLAG_CHECK			SPSR & ( 1 << SPIF ) 
//
//#define MCP_CS_ENABLE  PORTB  = PINB & ~cs
//#define MCP_CS_DISABLE PORTB |= cs

// Pay special attention to this line! It's right just here!
#define MCP_INTERRUPT_ENABLE            sem_post(&can_En)    
#define MCP_INTERRUPT_DISABLE           while(sem_trywait(&can_En)  ==  0)

#define MCP_CS0_INTERRUPT_ENABLE   	    sem_post(&can0_En)
#define MCP_CS1_INTERRUPT_ENABLE   	    sem_post(&can1_En)
//#define MCP_CS2_INTERRUPT_ENABLE   	

#define MCP_CS0_INTERRUPT_DISABLE  	    while(sem_trywait(&can0_En) ==  0)    
#define MCP_CS1_INTERRUPT_DISABLE  	    while(sem_trywait(&can1_En) ==  0)    
//#define MCP_CS2_INTERRUPT_DISABLE  	

//#define	CAN0_LOOPBACK_DEF	    			
//#define	CAN1_LOOPBACK_DEF	    	
//#define	CAN2_LOOPBACK_DEF	    	

#define CAN0_LOOPBACK_ENABLE	        can0LoopbackEnable()	
#define CAN0_LOOPBACK_DISABLE		    can0LoopbackDisable()
#define CAN1_LOOPBACK_ENABLE		    can1LoopbackEnable()
#define CAN1_LOOPBACK_DISABLE		    can1LoopbackDisable()
//#define CAN2_LOOPBACK_ENABLE		
//#define CAN2_LOOPBACK_DISABLE		

/***********************************************************************************
* Farineh CAN Protocol Commands
***********************************************************************************/
#define FCP_CMD_SYNC			    0x00
#define FCP_CMD_ROLE_IDENTIFY		0x01
#define	FCP_CMD_RESERVED_1		    0x02
#define	FCP_CMD_RESERVED_2		    0x03
#define FCP_CMD_SET_GET_STATE		0x04
#define FCP_CMD_SET_GET_DATA		0x05
#define FCP_CMD_SET_SETTING			0x06
#define FCP_CMD_DO_CALIBRATION		0x07
#define FCP_CMD_INITIATE_TRANSFER   0x08
#define FCP_CMD_DOWNLOAD_DATA       0x09
#define FCP_CMD_UPLOAD_DATA         0x0A
#define FCP_CMD_MAPCS2A_EXTENSION        0x0E
//#define FCP_CMD_GET_CALIBRATION		0x08

#define FCP_CMD_HOTREDUNDANCY	    0x0F

/***********************************************************************************
*  General Definitions
***********************************************************************************/
#define MAX_SPI_RETRY               300      // Defined retry value for SPI
#define MAX_SPI_ERROR               200      // Maximum allowed errord in SPI of CAN before swtiching to Out of Service

#define MAX_QUEUE_SIZE    20    // Maximum size of the received packets buffer
#define TX_TIMEOUT        15    // Defined timeout for sending packets
#define MIN_LINK_TIMEOUT  3000 	// Defined timeout for switching to FailSafeMode(3s)

#define HOT_REDUNDANCY_FAIL_COUNT   2

// Main card sends a Heart beat to the backup card
#define HOTREDUNDANCY_TX_INTERVAL   7

// Backup card Checks whether the Heart beat from the main is recieved or not!
#define HOTREDUNDANCY_RX_INTERVAL   8

//#define CAN3_TIMEOUT    			3   // 3ms time out to send heart beat on CAN3
#define MAX_NUM_CONTROLLERS         2   // max number of CAN Controllers = 2

/***********************************************************************************
*  External variables, Structure definition
***********************************************************************************/
typedef struct
{
    UChar Result;
    UChar Data;
}tSPI_Response;

typedef struct
{
    UChar ID[4];   // 11 bit or 29 bit (for both 4 bytes is enough)
    UChar DataLen; // number of bytes of data to follow(0-8)
    UChar Data[8]; // Data bytes
	
    UChar Command; // Farineh CAN Protocol Commands(SET_GET_STATE,SET_GET_DATA,...)
    UChar Address; // Address of source/destination (4bit: rack/ 2bit: slot)
    UChar CS;      // ChipSelect
    UChar Group;   // Input = 0 , Output = 1
    UChar Handle;
}tCANMsg;
extern tCANMsg   CAN_TxMsg;   // transmitter CAN message structure
extern tCANMsg   CAN_RxMsg;   // receiver    CAN message structure

typedef struct
{
    SChar   Head;   // symbol of Head of FIFO, the first read    item (OUT)
    SChar   Tail;   // symbol of Tail of FIFO, the first written item (IN)
    SChar   Length; // its role is like a counter in FIFO sturcture
	
    tCANMsg Msg[ MAX_QUEUE_SIZE ] ;
}tMessageQueue;	

typedef struct
{
    UChar           Mode ;
    UChar           IsFullTxBuff0;
    UChar           IsFullTxBuff1;
    UChar           IsFullTxBuff2;
    bool            TXDONE;
    tMessageQueue   TxQueue;
}tCANController;	

extern struct    Q_Buffer MLDE_DataReq;
extern UChar     BlockTransferTimer;
extern tCANMsg   CAN_TxMsg;
extern tCANMsg   CAN_RxMsg;
extern tCANMsg   CAN_TestMsg;
extern tCANController  CAN[ MAX_NUM_CONTROLLERS ] ;
extern struct Q_Buffer  MLDE_DataConf;
extern UChar UnwantedPacketReceived;
extern UChar CAN_ExpectedReceiveCount;
//filter and mask structure for socketcan
struct can_filter rfilter[2];
/***********************************************************************************
*  Defining strings used for printing an error/warning used in print_P function
***********************************************************************************/
#define msgCANError        "\r\n CAN%x not Responding!"
#define msgCANSendTimeOut  "\r\n CAN Send Timeout T: %x"

/***********************************************************************************
* CAN Function Prototypes
***********************************************************************************/
UChar CAN_Init( UChar mode );
UChar CAN_Send( void );
UChar CAN_SendOn( UChar cs, UChar useTxQueue, UChar useSpecificAddress );
UChar CAN_SendHeartBeat( UChar cs );
UChar IsCANBusAvailabe( UChar cs );

void  FetchMessage( UChar cs, UChar startRegister );
UChar GetRxMessage();

void  MCP_SPI_Init(void);
UChar MCP_SPI_Read(void);
void  MCP_SPI_Write( UChar data );
void  MCP_SPI_WriteBuffer( UChar cs, UChar txRegister, tCANMsg msg );

UChar MCP_REG_Read( UChar cs, UChar address );
void  MCP_REG_Write( UChar cs, UChar address , UChar data );
void  MCP_REG_BitModify( UChar cs,UChar address, UChar data, UChar mask );

UChar MCP_Init( UChar cs, UChar mode );
void  MCP_SoftReset( UChar cs );
void  MCP_RTS( UChar cs, UChar txRegister );
UChar MCP_ReadStatus( UChar cs );
UChar MCP_ReadRxStatus (UChar cs);

UChar PreProcessMessage( UChar command, UChar dataLen );
void  ProcessRxQueue();
void  InterruptRoutine( UChar cs, UChar id );

void ProcessPairResponse( UChar PairState );
UChar EncodeSID_H( UInt command, UInt address );
UChar EncodeSID_L( UInt group, UInt command );
UInt  DecodeSID( UChar sidh, UChar sidl );

Int sendFrame(Int CANSocket, Int canID, UChar dLen, UChar canData[]);
Int receiveFrame(Int CANSocket, struct can_frame *cf);
Int can0_init();
Int can1_init();
void prIntRcvFrame(struct can_frame rcvFrame);
void can0LoopbackDisable(void);
void can0LoopbackEnable(void);	
void can1LoopbackDisable(void);
void can1LoopbackEnable(void);	
void loopBackSetting(void);
#endif  //__CAN_H_

//***********************************************************************************
// No More
//***********************************************************************************