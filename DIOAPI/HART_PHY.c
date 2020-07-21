/***********************************************************************************
*  File      : HART_PHY.C
*----------------------------------------------------------------------------------
*  Object    : it includes required functions for Initializing and handling Physical
* 				layer of HART Master.
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
#include "Config.h"
#include "Common.h"

#ifdef HART_ENABLED

/***********************************************************************************
*  Specific Header files for HART enabled-IOCards
***********************************************************************************/
#include "HART.h"

/***********************************************************************************
*  Local variables
***********************************************************************************/
extern tUARTBuffer UART_RX_Buffer;
extern tHARTMSG    HART_RxMsg;
extern UInt data_byte;
tHARTMSG HART_TxMsg;

/***********************************************************************************
*  Local Definitions
***********************************************************************************/
#define	RET_SUCCESS		0	// Function is done successfully
#define ERR_CHECKSUM	1	

#define POLLING_ADDRRESS_TYPE       0
#define UNIQUE_ADDRRESS_TYPE        1

/***********************************************************************************
*  Function   : Send_Data_UART
*----------------------------------------------------------------------------------
*  Object     : To send HART Blocks on UART
*
*  Return     : RET_SUCCESS : 0
*				otherwise   : FAIL
*				
*----------------------------------------------------------------------------------
*  Variable Name     |IN |OUT|GLB|  Utilization
*--------------------+---+---+---+-------------------------------------------------
*  Data      		 | X |   |   |  Data to be sent to UART
*--------------------+---+---+---+-------------------------------------------------
*  DataLen           | X |   |   |  Data length
*--------------------+---+---+---+-------------------------------------------------
***********************************************************************************/
UChar Send_Data_UART( UChar *Data, UChar DataLen )
{
    INRTS_LOW;       // request to send HART
    __delay_cycles(80);
    for( UChar i = 0 ; i < DataLen ; i++ )
    {
        USART2_SETTX_FLG;
        USART2_DATA = *( Data + i);
        while (!(USART2_WAITTX_FLG));
    }
    INRTS_HIGH;
    Card.UART3_RX_Timer = 0;
	
    return RET_SUCCESS;
}

/***********************************************************************************
*  Function   : Fill_UART_Buffer
*----------------------------------------------------------------------------------
*  Object     : To fill UART Buffer
*
*  Return     : DataLen: Data Length
*				
*  Note(s)	  : FOR DEBUG ONLY
***********************************************************************************/
UChar Fill_UART_Buffer( void )
{
    UChar pointer;
    for ( UChar i=0 ; i < HART_TxMsg.PELength; i++ )
        UART_TX_Buffer.Byte[i] = 0xFF;
	
    UART_TX_Buffer.Byte[HART_TxMsg.PELength] = HART_TxMsg.Delimiter;
    pointer = HART_TxMsg.PELength + 1;
    if( HART_TxMsg.AddressType )
    {
        for( UChar i = pointer ; i < pointer + 5 ; i++ )
            UART_TX_Buffer.Byte[i] = HART_TxMsg.Address[i - HART_TxMsg.PELength - 1];
		
        pointer = pointer + 5;
    }
    else
    {
		UART_TX_Buffer.Byte[pointer++] = HART_TxMsg.Address[0];
	}
	
    UART_TX_Buffer.Byte[pointer++] = HART_TxMsg.Command;
    UART_TX_Buffer.Byte[pointer++] = HART_TxMsg.ByteCount;
    UART_TX_Buffer.Byte[pointer++] = HART_TxMsg.CheckByte;
	
    return pointer;
}

/***********************************************************************************
*  Function   : Decode_HART_Message
*----------------------------------------------------------------------------------
*  Object     : This function reads enable request service access point buffer
*
*  Return     : DataLen: Data Length
*				
***********************************************************************************/
UChar  Decode_HART_Message( void )
{
    UChar Temp;
    UChar Pointer =0;
    UChar *RX_Buffer_Pointer;
	
    for(UChar i = 0 ; i < 20 ; i++)
    {
        if ( UART_RX_Buffer.Byte[i] == 0xFF )
            HART_RxMsg.Preamble[i] = UART_RX_Buffer.Byte[i];
        else
        {
            HART_RxMsg.PELength=i;
            i=20;
        }
    }
    HART_RxMsg.Delimiter = UART_RX_Buffer.Byte[ HART_RxMsg.PELength ];
    Pointer              = HART_RxMsg.PELength + 1;
	
    HART_RxMsg.NumberOfExpansionByte = ( (HART_RxMsg.Delimiter & 0x60) >> 5);
    HART_RxMsg.FrameType             = (  HART_RxMsg.Delimiter & 0x07);
    HART_RxMsg.PhysicalLayerType     = (  HART_RxMsg.Delimiter & 0x18);
    if ( HART_RxMsg.Delimiter & 0x80 )
    {
        HART_RxMsg.AddressType = UNIQUE_ADDRRESS_TYPE;
        for(UChar i = 0 ; i < 5 ; i++)
            HART_RxMsg.Address[i] = UART_RX_Buffer.Byte[Pointer + i ];
        Pointer += 5;
    }
    else
    {
        HART_RxMsg.AddressType = POLLING_ADDRRESS_TYPE;
        HART_RxMsg.Address[0]  = UART_RX_Buffer.Byte[Pointer];
        Pointer++;
    }
	
    for(UChar i = 0 ; i < HART_RxMsg.NumberOfExpansionByte ; i++)
        HART_RxMsg.ExpansionByte[i] = UART_RX_Buffer.Byte[Pointer + i ];
	
    Pointer += HART_RxMsg.NumberOfExpansionByte;
	
    HART_RxMsg.Command   = UART_RX_Buffer.Byte[Pointer];
    Pointer++;
	
    HART_RxMsg.ByteCount = UART_RX_Buffer.Byte[Pointer];
    Pointer++;
	
    for(UChar i = 0 ; i < HART_RxMsg.ByteCount ; i++)
        HART_RxMsg.DATA[i] = UART_RX_Buffer.Byte[Pointer + i ];
	
    Pointer += HART_RxMsg.ByteCount;
	
    data_byte= HART_RxMsg.ByteCount;
    HART_RxMsg.CheckByte = UART_RX_Buffer.Byte[Pointer];
	
    RX_Buffer_Pointer = &(UART_RX_Buffer.Byte[HART_RxMsg.PELength]);
    Temp = Checksum(RX_Buffer_Pointer, (Pointer-HART_RxMsg.PELength));
    if( Temp != HART_RxMsg.CheckByte )
    {
        return ERR_CHECKSUM;    // TODO : Set The Check Sum Error Flag
    }
    else
    {
        return RET_SUCCESS;
    }
}

#endif

//***********************************************************************************
// No More
//***********************************************************************************

