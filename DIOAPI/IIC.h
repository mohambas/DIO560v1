/***********************************************************************************
*  File     : IIC.h
*----------------------------------------------------------------------------------
*  Object   : it contains required definitions for IIC.
*
*  Contents : IIC-related constants, function prototypes, ...
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
*  General Header file
***********************************************************************************/
#ifndef __IIC_H_
#define __IIC_H_

#include "Config.h"

/***************************************************************************************
*       General definitions
****************************************************************************************/
#define TWSR_MASK		    0xf8	// Mask the prescaler bits to zero

// IIC Settings (refer to ATmega128 data sheet) 
#define IIC_TWBR            0x0a    // IIC Bit rate Register setting.  SCL freq= (CPU CLK Freq)/(16+2*(TWBR)*(4^TWPS))=8MHz/(32)=250KHz
#define IIC_TWPS            0x00    // IIC prescaler Register setting.
#define IIC_RPU			    0x03	// IIC Pullup resistors for SDA & SCL signals

// Timeout is reported if the Counter reach this value (waiting for TWINT) 
// As I measured, each 1000000 counts generates about 2 Secs delay
#define IIC_TIMEOUT_INT		2000	// Thought less that 5 msec (Upto the code!)

// Number of IIC basic read or write functions to be repeated if device is not responding  
#define IIC_REPEAT			1000	// Thought about 60 msec (Upto the code!)

/**************************************************************************************
*       Status Codes (refer to ATmega128 data sheet)
***************************************************************************************/
#define MT_START		0x08	// A START condition has been transmitted
#define MT_REPSTART		0x10	// A repeated START condition has been transmitted
#define MT_SLA_ACK 		0x18	// SLA+W has been transmitted; ACK has been received (Master Transmit)
#define MT_SLA_NACK 	0x20	// SLA+W has been transmitted; NOT ACK has been received (Master Transmit)
#define MR_SLA_ACK 		0x40	// SLA+R has been transmitted; ACK has been received (Master Receive)
#define MR_SLA_NACK 	0x48	// SLA+R has been transmitted; NOT ACK has been received (Master Receive)
#define MT_DATA_ACK 	0x28	// Data byte has been transmitted; ACK has been received (Master Transmit)
#define MT_DATA_NACK 	0x30	// Data byte has been transmitted; NOT ACK has been received (Master Transmit)
#define MR_DATA_ACK 	0x50	// Data byte has been received; ACK has been returned (Master Receive)
#define MR_DATA_NACK 	0x58	// Data byte has been received; NOT ACK has been returned (Master Receive)

/*************************************************************************************
*           IIC Function return values
**************************************************************************************/
#define IIC_SUCCESS		0		// Function is done successfully
#define IICERR_START	1		// Start condition not transmitted
#define IICERR_SLAACK	2		// SLA ACK not received (EEPROM is bussy for example doing last write instruction)
#define IICERR_DATAACK	3		// DATA ACK not received (Write) or transmitted (Read)
#define IICERR_DATAEND	4		// Final data byte NOT ACK status has not been detected (Read)
#define ERR_UNKNOWN		5		// Unknown error happened
#define IICERR_TIMEOUT	6		// TWINT detection Timeout error

/**************************************************************************************
*           Temperature Sensor:
***************************************************************************************/
#define TMPS_TEMP		0x00	// Pointer value for Temperature register 
#define TMPS_CNFG		0x01	// Pointer value for Configuration register 
#define TMPS_HYST		0x02	// Pointer value for Hystersis register 
#define TMPS_SET		0x03	// Pointer value for Temperature Setpoint register 

/**************************************************************************************
*           Functions 
***************************************************************************************/

void  LCPC_IICInit(void);
UChar LCPC_IICBusy(void);
UChar LCPC_IICWrite(UChar SLA, UChar* pDataBuf, UChar DataSize);
UChar LCPC_IICRead(UChar SLA, UChar* pDataBuf, UChar DataSize);
UChar LCPC_IICGetStatus(void);

//  NOTE-------------NOTE--------------NOTE: deprecated	
//// IO Expander (MAX7313):
//UChar LCPC_IOExpWrite(UChar IOEX_SLA, UChar RegAdd, UChar RegData);
//UChar IOExpander_Read(UChar IOEX_SLA, UChar RegAdd, UChar* pRegData);

// Temperature Sensor:
UChar LCPC_TempSensorWrite(UChar SLA, UChar RegPointer, UInt RegData);
UChar LCPC_TempSensorRead(UChar SLA, UChar RegPointer, UInt* pRegData);

#endif  //__IIC_H_

//***********************************************************************************
// No More
//***********************************************************************************