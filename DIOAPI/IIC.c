/***********************************************************************************
*  File     : IIC.c
*----------------------------------------------------------------------------------
*  Object   : This file contains required functions for Initializing and 
*             handling IIC bus. Moreover, required functions for handling
*             peripherals on the IIC bus have been implemented here.
*
*  Contents : LCPC_IIC-related functions.
*
* ---------------------------------------------------------------------------------
*  Date     |	Description
* ----------+----------------------------------------------------------------------

* ----------+----------------------------------------------------------------------
***********************************************************************************/

/***********************************************************************************
*  General Header file
***********************************************************************************/
#include "IIC.h"
#include "Common.h"


/*********************************************************************************************************
* Name:         LCPC_IICInit
*
* Description:  This function is called by the initialization function to initialize
*               the IIC interface of chip in master mode.
*
* Returns    :  None
* Note(s)    :  The IIC port is configured for interface with some ICs such as
*		        the TCN75 and External EEPROM and ... .
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*********************************************************************************************************/
//void LCPC_IICInit(void)
//{
////    TWBR = IIC_TWBR;		// Set bit rate register (Baudrate)
////    TWSR = IIC_TWPS;        // Driver presumes prescaler to be 00.
////    PORTD = PIND | IIC_RPU;	// Activate IIC port pullups
////    TWDR = 0xFF;            // Default content = SDA released.
////    TWCR = (1 << TWEN);		// Enabling TWI (IIC) bus
//}

/*********************************************************************************************************
* Name:        LCPC_IICBusy
*
* Description: Call this function to test if the IIC_ISR is busy transmitting.
*
* Returns    : True (Busy) or False (Free)
*
* Note(s)    : IF IIC Interrupt Flag(TWINT) is set, then the Transceiver is busy
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
*********************************************************************************************************/
UChar LCPC_IICBusy(void)
{
//    return (!(TWCR & (1 << TWINT)));    // Check if the TWINT is set
}

/*********************************************************************************************************
* Name:         LCPC_IICWrite
*
* Description:  This function transmits DataSize bytes of data through IIC bus.
*
* Arguments  : 
*               SLA			IIC device slave address
*               pDataBuf	    Pointer to data buffer
*               DataSize 	Size of the data bytes for transmission
* Returns    :  
*               Content of the SPI Data register
*               IIC_SUCCESS			Function is done successfully
*               IICERR_START		Start condition not transmitted
*               IICERR_SLAACK		SLA ACK not received (EEPROM is bussy)
*               IICERR_DATAACK		DATA ACK not received
*               IICERR_TIMEOUT		Waiting for TWINT Timed out (IIC Error)
* Note(s)    :
*********************************************************************************************************/
UChar LCPC_IICWrite(UChar SLA, UChar* pDataBuf, UChar DataSize)
{
//    UChar i, RetVal;
//    ULong WaitCntr;
//  
//    RetVal = IIC_SUCCESS;										// Default value to return
//    
//    TWCR = (1 << TWINT)|(1 << TWSTA)|(1 << TWEN);				// TWI Enabled, TWINT cleared & START condition requested
//    
//    WaitCntr = 0;												    // Reset counter
//    while(( WaitCntr < IIC_TIMEOUT_INT ) && !(TWCR & (1 << TWINT)))    // Wait for TWINT to release or specified timeout to reach (estimated 2.4 Sec)
//    {	
//        __watchdog_reset();
//        WaitCntr++;												    // Count if TWINT not detected
//    };    			
//    if( WaitCntr == IIC_TIMEOUT_INT )	 
//        RetVal = IICERR_START;	    // Timeout error (IIC not released)
//    
//    if(RetVal == IIC_SUCCESS)   // Continue if no error happened
//    {								    
//        if ((TWSR & TWSR_MASK) != MT_START)						    // START condition has been sent
//            RetVal = IICERR_START;  							 
//        if(RetVal == IIC_SUCCESS)
//        {								    // Continue if no error happened
//            TWDR = SLA<<1;										    // 7 bits IIC Slave address + r/W# bit(0 with here)
//            TWCR = (1 << TWINT) |(1 << TWEN);						        // TWI Enabled, TWINT cleared 
//            WaitCntr = 0;												    // Reset counter
//            while (WaitCntr < IIC_TIMEOUT_INT && !(TWCR & (1 << TWINT)))
//            {	// Wait for TWINT to release or specified timeout to reach (estimated 2.4 Sec)
//                WaitCntr++;												    // Count if TWINT not detected
//                __watchdog_reset();
//            };    			
//            if(WaitCntr == IIC_TIMEOUT_INT)	 
//                RetVal = IICERR_TIMEOUT;	// Timeout error
//            
//            if(RetVal == IIC_SUCCESS)
//            {									// Continue if no error happened
//                if ((TWSR & TWSR_MASK) != MT_SLA_ACK)					    // SLA+R has been transmitted; ACK has been received (Master Receive)
//                    RetVal = IICERR_SLAACK;								 
//                
//                if(RetVal == IIC_SUCCESS)
//                {								    // Continue if no error happened
//                    for(i = 0; i < DataSize; i++)
//                    {					        // Transmitting "DataSize" data bytes
//                        TWDR = pDataBuf[i];							
//                        TWCR = (1 << TWINT) |(1 << TWEN);
//                        WaitCntr = 0;												    // reset counter
//                        while (WaitCntr < IIC_TIMEOUT_INT && !(TWCR & (1 << TWINT)))
//                        {	// Wait for TWINT to release or specified timeout to reach (estimated 2.4 Sec)
//                            WaitCntr++;												    // Count if TWINT not detected
//                            __watchdog_reset();
//                        };    			
//                        if(WaitCntr == IIC_TIMEOUT_INT)	 
//                            RetVal = IICERR_TIMEOUT;	// Timeout error
//                        
//                        if(RetVal == IIC_SUCCESS)
//                        {						            // Continue if no error happened
//                            if ((TWSR & TWSR_MASK) != MT_DATA_ACK)
//                            {	
//                                RetVal = IICERR_DATAACK;  // Set error
//                                break;					  // Don't continue if there is error
//                            }
//                        }
//                        else 
//                            break;						  // Stop the loop if error happened
//                    }
//                }
//            }
//        }
//    }
//    TWCR = (1 << TWINT)|(1 << TWEN)|(1 << TWSTO);		  // TWI Enabled, TWINT cleared & STOP condition requested	
//    
//	// FOR DEBUG ONLY: to check out wait counter if RetVal != successful
//    //if (RetVal) 
//	//	printf(" R: %x T0: %x T1: %x ", RetVal, WaitCntr/ 256, WaitCntr % 256);	
//    
//    return RetVal;
}

/*********************************************************************************************************
* Name:         LCPC_IICRead
*
* Description:  This function receives DataSize bytes of data through IIC bus.
*
* Arguments  : 
*               SLA 		    IIC device slave address
*               pDataBuf 	Pointer to data buffer
*               DataSize 	Size of the data bytes for transmission
* Returns    : 
*               IIC_SUCCESS			Function is done successfully
*               IICERR_START		    Start condition not transmitted
*               IICERR_SLAACK		SLA ACK not received (EEPROM is bussy)
*               IICERR_DATAACK		DATA ACK not transmitted
*               IICERR_DATAEND		Final data byte NOT ACK status has not been detected (Read)
* Note(s)    : 
*********************************************************************************************************/
//UChar LCPC_IICRead( UChar SLA, UChar *pDataBuf, UChar DataSize )
//{	
//    UChar i, RetVal;
//    ULong WaitCntr=0;
//  
//    RetVal = IIC_SUCCESS;										    // Default value to return is IIC_SUCCESS	
//    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);				// TWI Enabled, TWINT cleared & START condition requested
//    WaitCntr = 0;												    // Reset counter
//    while(( WaitCntr < IIC_TIMEOUT_INT ) && !(TWCR & (1 << TWINT)))    // Wait for TWINT to release or specified timeout to reach (estimated 2.4 Sec)
//    {	
//        WaitCntr++;												    // Count if TWINT not detected
//        __watchdog_reset();
//    };    			
//
//    if( WaitCntr == IIC_TIMEOUT_INT )	 
//        RetVal = IICERR_START;		// Timeout error (IIC not released)
//    
//    if( RetVal == IIC_SUCCESS )
//    {
//        if(( TWSR & TWSR_MASK ) != MT_START )						// START condition has been sent
//            RetVal = IICERR_START; 								    // set error
//        
//        if( RetVal == IIC_SUCCESS )
//        {								    // Continue if no error happened
//            TWDR = (SLA << 1) + 0x01;								    // 7 bits IIC slave address + r/W# bit(1 with here)
//            TWCR = (1 << TWINT) |(1 << TWEN);						    // TWI Enabled, TWINT cleared 
//            WaitCntr = 0;												// Reset counter
//            while (WaitCntr < IIC_TIMEOUT_INT && !(TWCR & (1 << TWINT)))  // Wait for TWINT to release or specified timeout to reach (estimated 2.4 Sec)
//            {	
//                WaitCntr++;												    // Count if TWINT not detected
//                __watchdog_reset();
//            };    			
//            if(WaitCntr == IIC_TIMEOUT_INT)	 
//                RetVal = IICERR_TIMEOUT;	// Timeout error
//            
//            if(RetVal == IIC_SUCCESS)
//            {
//                if ((TWSR & TWSR_MASK) != MR_SLA_ACK)					    // SLA+R has been transmitted; ACK has been received (Master Receive)
//                    RetVal = IICERR_SLAACK;								 
//            
//                if( RetVal == IIC_SUCCESS )
//                {								    // Continue if no error happened
//                    i=0;
//                    for(i = 0; i < DataSize - 1; i++)
//                    {						// Loop for reading (DataSize -1) bytes
//                        TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);		// TWI Enabled, TWINT cleared & ACK enabled
//                        WaitCntr = 0;												    // reset counter
//                        while (WaitCntr < IIC_TIMEOUT_INT && !(TWCR & (1 << TWINT)))    // Wait for TWINT to release or specified timeout to reach (estimated 2.4 Sec)
//                        {	
//                            WaitCntr++;												    // Count if TWINT not detected
//                            __watchdog_reset();
//                        };    			
//                        if(WaitCntr == IIC_TIMEOUT_INT)
//                        {
//                            RetVal = IICERR_TIMEOUT;					    // Timeout error
//                            break;
//                        }						
//                        pDataBuf[i] = TWDR;    							// Read data
//                        if ((TWSR & TWSR_MASK) != MR_DATA_ACK)
//                        {			// Check if ACK has been received
//                            RetVal = IICERR_DATAACK;					 
//                            break;										// Don't continue if there is error
//                        }
//                    }
//                    if(RetVal == IIC_SUCCESS)
//                    {						// Continue if no error happened
//                        TWCR = (1 << TWINT) | (1 << TWEN);				// No data ACK
//                        WaitCntr = 0;												    // Reset counter
//                        while (WaitCntr < IIC_TIMEOUT_INT && !(TWCR & (1 << TWINT)))    // Wait for TWINT to release or specified timeout to reach (estimated 2.4 Sec)
//                        {	
//                            WaitCntr++;												    // Count if TWINT not detected
//                            __watchdog_reset();
//                        };    			
//                        if(WaitCntr == IIC_TIMEOUT_INT)	 
//                            RetVal = IICERR_TIMEOUT;	    // Timeout error
//                        if(RetVal == IIC_SUCCESS)
//                        {							
//                            pDataBuf[i] = TWDR;    					// Read final data
//                            if ((TWSR & TWSR_MASK) != MR_DATA_NACK)	// Check for NACK
//                                RetVal = IICERR_DATAEND;  			// For debug purposes
//                        }
//                    }
//                }
//            }
//        }
//    }
//    
//    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);			// TWI Enabled, TWINT cleared & STOP condition requested	
//    
//	// FOR DEBUG ONLY: to check out wait counter if RetVal != successful
//    //if (RetVal) 
//    //	printf(" R: %x T0: %x T1: %x ", RetVal, WaitCntr/ 256, WaitCntr % 256);
//
//    return RetVal;
//}

/*********************************************************************************************************
* Name:         LCPC_IICGetStatus
*
* Description:  Call this function to fetch the state information of the previous operation.
*
* Arguments  : 
*               Status Code
* Returns    :  Error state
* Note(s)    :  The function will hold execution (loop)until the IIC_ISR has completed
*               with the previous operation. If there was an error, then the function
*               will return the IIC State code.
*
* Note(s)	 : FOR DEBUG ONLY
*********************************************************************************************************/
UChar LCPC_IICGetStatus(void)
{
//    UChar IIC_State;
//    
//    while( LCPC_IICBusy());	// Wait until IIC has completed the transmission.	(INFLOOP)
//        IIC_State = TWSR;
//        
//    return IIC_State;			// Return error state.
}

///*********************************************************************************************************
//* Name:         LCPC_IOExpWrite
//*
//* Description:  Call this function to fill one of the IO Expander (MAX7313) registers with
//*
//* Arguments  :  IOEX_SLA	IO Exp. slave address
//*               RegAdd		Address of the specified register
//*               RegData		Data to be written to specified register
//* Returns    :  IIC return values
//*
//* Note(s)    :  deprecated
//*********************************************************************************************************/
//UChar LCPC_IOExpWrite(UChar IOExpSLA, UChar RegAdd, UChar RegData)
//{
//    UChar data[2];				                // General buffer for register address and data
//    
//    data[0] = RegAdd;							// Fill the buffer
//    data[1] = RegData;
//
//    return LCPC_IICWrite( IOExpSLA, data, 2 );	// Write to IOExpander
//}
//
///*********************************************************************************************************
//* Name:         IOExpander_Read
//*
//* Description:  Call this function to Read one of the IO Expander (MAX7313) registers data.
//*
//* Arguments  :  IOEX_SLA	IO Exp. slave address
//*               RegAdd		Address of the specified register
//*               RegData		Data to be written to specified register
//*
//* Returns    :  IIC return values
//*
//* Note(s)    :  deprecated
//*********************************************************************************************************/
//UChar IOExpander_Read(UChar IOEX_SLA, UChar RegAdd, UChar* pRegData)
//{
//    UChar data[1], RetVal;						    // General Buffer for Register Adresss
//    // In this part we do a dummy write to set the register address 
//    RetVal = ERR_UNKNOWN;								// Default value
//    data[0]= RegAdd;									// Register address
//    RetVal = LCPC_IICWrite( IOEX_SLA, data, 1 );		// Write to IOExpander  
//    // Now we Read
//    if(RetVal == IIC_SUCCESS)							// Continue if dummy write was successful
//        RetVal = LCPC_IICRead( IOEX_SLA, pRegData,1 );	// Read from IOExpander
//    
//    return RetVal;
//}

/*********************************************************************************************************
* Name:         LCPC_TempSensorWrite
*
* Description:  Call this function to write in one of temperature sensor's registers.
*
* Arguments  : 
*               SLA			Temperature sensor slave address
*               RegPointer	Specifies the register to be written
*               RegData		Data value to be written in the register
* Returns    :  
*               IIC Errors
* Note(s)    : 
*********************************************************************************************************/
UChar LCPC_TempSensorWrite(UChar SLA, UChar RegPointer, UInt RegData)
{
    UChar RetVal, data[3];
    
    data[0] = RegPointer; 					// Fill the buffer Pointer Register value
    if(RegPointer == TMPS_CNFG)
    {// Configuration Register needs 1 byte data
        data[1] = RegData;
        RetVal = LCPC_IICWrite(SLA, data, 2);	// write to IIC
    }
    else
    {// Other registers need 2 Bytes
        data[1] = (RegData >> 8);
        data[2] = RegData;
        RetVal = LCPC_IICWrite(SLA, data, 3);	// write to IIC
    }
    
    return RetVal;
}

/*********************************************************************************************************
* Name:         LCPC_TempSensorRead
*
* Description:  Call this function to read from one of the temperatur sensor's registers.
*
* Arguments  : 
*               SLA			Temperature sensor slave address
*               RegPointer	Specifies the resister to be read
*               RegData		Keeps the read Data from the register 
* Returns    : 
*               IIC Errors
* Note(s)    : 
*********************************************************************************************************/
UChar LCPC_TempSensorRead( UChar SLA, UChar RegPointer, UInt* pRegData )
{
    UChar RetVal, data[2];
    
    data[0] = RegPointer; 					// Fill the buffer Pointer Register value
    RetVal = LCPC_IICWrite(SLA, data, 1);		// write to IIC

    if( RetVal == IIC_SUCCESS )
    {
        if( RegPointer == TMPS_CNFG )
        { // Configuration Register needs 1 byte data
            RetVal = LCPC_IICRead( SLA, data, 1 );
            *pRegData = data[0];
        }		
        else
        {   // Other registers need 2 Bytes
            RetVal = LCPC_IICRead( SLA, data, 2 );
            
            *pRegData = (UInt) data[0];
            *pRegData = (UInt) ((*pRegData) << 8);
            *pRegData |= (UInt) data[1];
        }
    }
    
    return RetVal;
}

//***********************************************************************************
// No More
//***********************************************************************************



