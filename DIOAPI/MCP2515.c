/***********************************************************************************
*  File      : CAN.c
*----------------------------------------------------------------------------------
*  Object    : it includes required functions for Initializing and handling CAN
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
#include "CAN.h"
#ifdef HART_ENABLED
	#include "HART.h"
#endif

#include "Config.h"
#include "Common.h"

/***********************************************************************************
*  Function   : MCP_SPI_Init ()
*----------------------------------------------------------------------------------
*  Object     : it is called by the CAN initialization function to initialize the
*               SPI interface of the ATMega128, ATMega2560 and ATMega1280 chip.
*
*		 		The SPI port is configured for interface with MCP2515, the CAN
*				controller chip (microchip company).
*				
*				The configuration is as follows:
*               - Mode 00 or Mode 11
*               - SPI interrupt enable
*               - Data order is MSB first
*               - The ATMega128, ATMega2560 and ATMega1280 is master of the SPI bus
*               - The clock polarity is zero. This means that when CS is disable,
*				   the clock is 0 and first edge is rising edge.
*               - Clock phase is 0. This means that in the leading edge of clock
*				   after running data is sampled.
*               - Double SPI speed bit is set, so the SPI rate is : Fosc/2
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
***********************************************************************************/
void MCP_SPI_Init(void)
{
    UChar cs;
//    MCP_SPI_INIT;  // Define pins of SPI_SS, SPI_SCK, SPI_MOSI, CAN0,1,2 chip select
//	
//    cs = CAN_CHIP0;
//    MCP_CS_DISABLE;// Disable the CAN0's chip select
//    cs = CAN_CHIP1;
//    MCP_CS_DISABLE;// Disable the CAN1's chip select
//    cs = CAN_CHIP2;
//    MCP_CS_DISABLE;// Disable the CAN2's chip select             
//	
//	MCP_SPI_MODE_INIT;			
//	MCP_SPI_SETTING_INIT;	
//	
//	MCP_SPI_MODE_ENABLE;		 // Enable SPI, Configure in Master mode	 		
//	MCP_SPI_SETTING_SET;		 // Double rate the SPI clock	
	
}

/***********************************************************************************
*  Function   : MCP_SoftReset
*----------------------------------------------------------------------------------
*  Object     : it is called to reset the CAN controller by software.
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  cs			   |  chip select of desired CAN Controller 0,1,2
*------------------+-----------------------------------------------------------------
***********************************************************************************/
void MCP_SoftReset( UChar cs )
{
//    MCP_CS_ENABLE;
//	
//    MCP_SPI_Write( CAN_CMD_RESET );
//	
//    MCP_CS_DISABLE;
	
	switch (cs)
	{
	case CAN_CHIP0:		
		system("sudo ip link set can0 down");
		system("sudo ip link set can0 up");
		break;
	case CAN_CHIP1:
		system("sudo ip link set can1 down");
		system("sudo ip link set can1 up");
		break;
	}
	
}

/***********************************************************************************
*  Function   : MCP_Init
*----------------------------------------------------------------------------------
*  Object     : To initialize the CAN slave chip (MCP2515).
*
*  Return     : 0:         successful
*               otherwise: failed   (for example 2 means "CAN link error")
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  cs			   |  chip select of desired CAN Controller 0,1,2
*------------------+-----------------------------------------------------------------
*  mode			   |  mode of initialization: NORMAL, LOOP_BACK, ...
*------------------+-----------------------------------------------------------------
***********************************************************************************/
UChar MCP_Init( UChar cs, UChar mode )
{
    // At first, disable appropriate CAN interrupt
    switch( cs )
    {
	  case CAN_CHIP0:		
		MCP_CS0_INTERRUPT_DISABLE;
        break;
	  case CAN_CHIP1:
		MCP_CS1_INTERRUPT_DISABLE;
        break;
	  case CAN_CHIP2:
		MCP_CS2_INTERRUPT_DISABLE;
        break;
    }
	
	// FOR DEBUG ONLY: to check out backplane address
    // printf_P( "\r Card.BPAddress=0x%X\n", Card.BPAddress ) ;
	
	// reset requested CAN controller by software
    MCP_SoftReset( cs );
    // Check CANCTRL Register due to its initial value!   
	// MODE_CONFIG | MCP_CANCTRL_CLKEN_S = 0x84, but MODE_MASK is 0xE0
    // then 0x04 to enable clock out is useless!!!!
    MCP_REG_BitModify( cs, CANCTRL, MODE_CONFIG | MCP_CANCTRL_CLKEN_S, MODE_MASK );
    if(( MCP_REG_Read( cs, CANCTRL ) & MODE_MASK ) != MODE_CONFIG )
    {
		// Check the previous value
        if( cs == CAN_CHIP0 )
            Card.LCPCError.Bit.Link1 = 1;     // Set CAN0 link error
        else if( cs == CAN_CHIP1 )
            Card.LCPCError.Bit.Link2 = 1;     // Set CAN1 link error
        else if( cs == CAN_CHIP2 )
            Card.LCPCError.Bit.Link3 = 1;     // Set CAN2 link error
		
//        MCP_CS_DISABLE ;
        MCP_INTERRUPT_ENABLE;
        return 2;
    }
	
	// Synchronization Jump With (SJW) ==> 0 ==>1 TQ
	// Baud Rate Prescaler (BRP) = 0   ==> TQ = 2 x (BRP + 1)/FOSC ==> TQ = 2 T_OSL
	MCP_REG_Write( cs, CNF1, CNF1_SJW | CNF1_BRP );     // 0x00
	// The SAM bit controls how many times RXCAN pin is sampled: SAM = 1 ==> 3times
	// The BTLMODE bit controls how the length of PS2 is determined.
	//     BTLMODE = 1 the length of PS2 is determined by PHSEG2<2:0> bits of CNF3
	// The PRSEG<2:0> bits set length of propagation segment ==> 4 TQ
    // The PHSEG1<2:0> bits set length of PS1 ==> PS1 Length =(PHSEG1 + 1)*TQ = 3 TQ	
	MCP_REG_Write( cs, CNF2, MCP_CNF2_SAM_S | MCP_CNF2_BTLMODE_S | ( MCP_CNF2_PRSEG1_S | MCP_CNF2_PRSEG0_S ) | MCP_CNF2_PHSEG11_S );    // 0xD3
    // The PHSEG2<2:0> bits set length in TQ’s of PS2 ==> 4 TQ
	MCP_REG_Write( cs, CNF3, MCP_CNF3_PHSEG21_S | MCP_CNF3_PHSEG20_S );     // 0x03
	//  //1TQ, 3TQ, 4TQ, 4TQ
	//	MCP_REG_Write( cs, CNF1, CNF1_SJW | CNF1_BRP );
	//	MCP_REG_Write( cs, CNF2, MCP_CNF2_SAM_S | MCP_CNF2_BTLMODE_S | ( MCP_CNF2_PRSEG1_S ) | MCP_CNF2_PHSEG11_S | MCP_CNF2_PHSEG10_S );
	//	MCP_REG_Write( cs, CNF3, MCP_CNF3_PHSEG21_S | MCP_CNF3_PHSEG20_S );
	
    MCP_REG_Write( cs, BFPCTRL,   BFPCTRL_Default );
    MCP_REG_Write( cs, TXRTSCTRL, TXRTSCTRL_Default );
    
    if( mode != MODE_LISTENONLY )
    {
      
        MCP_REG_Write( cs, RXB0CTRL, REC_ONLY_VALID_STD | MCP_RXB0CTRL_BUKT_S );
        MCP_REG_Write( cs, RXB1CTRL, REC_ONLY_VALID_STD );
		
        // MASK setting M0        
        MCP_REG_Write( cs, RXM0SIDH, 0xFC );
        if(Card.Group!=INOUT_GROUP)
         MCP_REG_Write( cs, RXM0SIDL, 0x20 );
        else
         MCP_REG_Write( cs, RXM0SIDL, 0x00 );
        
        MCP_REG_Write( cs, RXM0EID8, 0x00 );
        MCP_REG_Write( cs, RXM0EID0, 0x00 );
        // MASK setting M1
        MCP_REG_Write( cs, RXM1SIDH, 0xFC );
        if(Card.Group!=INOUT_GROUP)	
         MCP_REG_Write( cs, RXM1SIDL, 0x20 );	
        else
         MCP_REG_Write( cs, RXM1SIDL, 0x00 ); 
        
        MCP_REG_Write( cs, RXM1EID8, 0x00 );
        MCP_REG_Write( cs, RXM1EID0, 0x00 );
        
       if(Card.Group!=INOUT_GROUP)
        {
        // Filter 0
        MCP_REG_Write( cs, RXF0SIDH, 0 );	
        MCP_REG_Write( cs, RXF0SIDL, ( Card.Group << 5 ));
        MCP_REG_Write( cs, RXF0EID8, 0 );	
        MCP_REG_Write( cs, RXF0EID0, 0 );	
        // Filter 1
        MCP_REG_Write( cs, RXF1SIDH, ( Card.BPAddress << 2 ));
        MCP_REG_Write( cs, RXF1SIDL, ( Card.Group << 5 ));	
        MCP_REG_Write( cs, RXF1EID8, 0 );	
        MCP_REG_Write( cs, RXF1EID0, 0 );	
        // Filter 2
        MCP_REG_Write( cs, RXF2SIDH, 0 );	
        MCP_REG_Write( cs, RXF2SIDL, ( Card.Group << 5 ));	
        MCP_REG_Write( cs, RXF2EID8, 0 );	
        MCP_REG_Write( cs, RXF2EID0, 0 );	
        // Filter 3
        MCP_REG_Write( cs, RXF3SIDH, ( Card.BPAddress << 2 ));	
        MCP_REG_Write( cs, RXF3SIDL, ( Card.Group << 5 ));
        MCP_REG_Write( cs, RXF3EID8, 0 );	
        MCP_REG_Write( cs, RXF3EID0, 0 );	
        // Filter 4
        MCP_REG_Write( cs, RXF4SIDH, 0 );	
        MCP_REG_Write( cs, RXF4SIDL, ( Card.Group << 5 ));	
        MCP_REG_Write( cs, RXF4EID8, 0 );	
        MCP_REG_Write( cs, RXF4EID0, 0 );	
        // Filter 5
        MCP_REG_Write( cs, RXF5SIDH, ( Card.BPAddress << 2 ));	
        MCP_REG_Write( cs, RXF5SIDL, ( Card.Group << 5 ));
        MCP_REG_Write( cs, RXF5EID8, 0 );	
        MCP_REG_Write( cs, RXF5EID0, 0 );	
        }
        else
        {
        // Filter 0
        MCP_REG_Write( cs, RXF0SIDH, 0 );	
        MCP_REG_Write( cs, RXF0SIDL, 0 );
        MCP_REG_Write( cs, RXF0EID8, 0 );	
        MCP_REG_Write( cs, RXF0EID0, 0 );	
        // Filter 1
        MCP_REG_Write( cs, RXF1SIDH, ( Card.BPAddress << 2 ));
        MCP_REG_Write( cs, RXF1SIDL, 0 );	
        MCP_REG_Write( cs, RXF1EID8, 0 );	
        MCP_REG_Write( cs, RXF1EID0, 0 );	
        // Filter 2
        MCP_REG_Write( cs, RXF2SIDH, 0 );	
        MCP_REG_Write( cs, RXF2SIDL, 0 );	
        MCP_REG_Write( cs, RXF2EID8, 0 );	
        MCP_REG_Write( cs, RXF2EID0, 0 );	
        // Filter 3
        MCP_REG_Write( cs, RXF3SIDH, ( Card.BPAddress << 2 ));	
        MCP_REG_Write( cs, RXF3SIDL, 0 );
        MCP_REG_Write( cs, RXF3EID8, 0 );	
        MCP_REG_Write( cs, RXF3EID0, 0 );	
        // Filter 4
        MCP_REG_Write( cs, RXF4SIDH, 0 );	
        MCP_REG_Write( cs, RXF4SIDL, 0 );	
        MCP_REG_Write( cs, RXF4EID8, 0 );	
        MCP_REG_Write( cs, RXF4EID0, 0 );	
        // Filter 5
        MCP_REG_Write( cs, RXF5SIDH, ( Card.BPAddress << 2 ));	
        MCP_REG_Write( cs, RXF5SIDL, 0 );
        MCP_REG_Write( cs, RXF5EID8, 0 );	
        MCP_REG_Write( cs, RXF5EID0, 0 );	          
        }
    }
    else
    {
        MCP_REG_Write( cs, RXB0CTRL, REC_ALL_VALID | MCP_RXB0CTRL_BUKT_S );
        MCP_REG_Write( cs, RXB1CTRL, REC_ALL_VALID );
		
        // MASK setting M0
        MCP_REG_Write( cs, RXM0SIDH, 0x00 );
        MCP_REG_Write( cs, RXM0SIDL, 0x00 );
        MCP_REG_Write( cs, RXM0EID8, 0x00 );
        MCP_REG_Write( cs, RXM0EID0, 0x00 );
        // MASK setting M1
        MCP_REG_Write( cs, RXM1SIDH, 0x00 );	
        MCP_REG_Write( cs, RXM1SIDL, 0x00 );	
        MCP_REG_Write( cs, RXM1EID8, 0x00 );
        MCP_REG_Write( cs, RXM1EID0, 0x00 );
    }
    MCP_REG_Write( cs, TXB0CTRL , TXB0PRIORITY );       // 0x03
    MCP_REG_Write( cs, TXB1CTRL , TXB1PRIORITY );       // 0x02
    MCP_REG_Write( cs, TXB2CTRL , TXB2PRIORITY );       // 0x01
	
    MCP_REG_Write( cs, CANINTE , MCP_CANINTE_TX0IE_S | MCP_CANINTE_TX1IE_S | MCP_CANINTE_TX2IE_S | MCP_CANINTE_RX1IE_S | MCP_CANINTE_RX0IE_S );                	
	
	//========= NOTE : The IOCard's CAN should not work in One Shot Mode ===============
	//    if(cs == CAN_Chip2)
	//        MCP_REG_BitModify( cs, CANCTRL, mode | MCP_CANCTRL_CLKEN_S | MCP_CANCTRL_OSM_S, MODE_MASK | MCP_CANCTRL_OSM_M );
	//    else
	MCP_REG_BitModify( cs, CANCTRL, mode | MCP_CANCTRL_CLKEN_S, MODE_MASK );
	
    // Enable appropriate CAN interrupt
    switch( cs )
    {
	  case CAN_CHIP0:		
		MCP_CS0_INTERRUPT_ENABLE;
		if( mode != MODE_LOOPBACK )
			CAN0_LOOPBACK_DISABLE;
        break;
	  case CAN_CHIP1:
		MCP_CS1_INTERRUPT_ENABLE;
		if( mode != MODE_LOOPBACK )
			CAN1_LOOPBACK_DISABLE;
        break;
	  case CAN_CHIP2:
		MCP_CS2_INTERRUPT_ENABLE;
		if( mode != MODE_LOOPBACK )
			CAN2_LOOPBACK_DISABLE;
        break;
    }
	
//    MCP_CS_DISABLE;
	
    return RET_SUCCESS;
}

/***********************************************************************************
*  Function   : MCP_SPI_Read ()
*----------------------------------------------------------------------------------
*  Object     : This function is called whenever a read from the SPI bus is occured
*               When data recieved in the recieve buffer, the SPI end of transmition
*				 flag is set and an interrupt is requested
*
*  Return     : Content of the SPI Data register
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
***********************************************************************************/
UChar MCP_SPI_Read(void)
{
//    MCP_SPI_DATA = NULL;        // it is really necessary to write NULL in SPDR!
	
    ULong ResponseWaite=0;
//	//  wait until data byte is received or time out occurred
//    while( !( MCP_SPI_FLAG_CHECK )&&(ResponseWaite<10000))
//    {
//      		//__watchdog_reset();
//		//__delay_cycles(1);
//                ResponseWaite++;
//    }
//	
//    return MCP_SPI_DATA ;
	
	
	
	
}

/***********************************************************************************
*  Function   : MCP_REG_Read
*----------------------------------------------------------------------------------
*  Object     : To read data from register beginning at selected address.
*
*  Return     : Ret_Data:  content of the register
*               If CS pin is not raised to 1, the consequent addresses CAN be read.
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  cs              |  chip select of desired CAN Controller 0,1,2
*------------------+-----------------------------------------------------------------
*  address         |  Address of register & MCP2515 chip
*------------------+-----------------------------------------------------------------
***********************************************************************************/
UChar MCP_REG_Read( UChar cs, UChar address )
{
    UChar Ret_Data;
	
//    MCP_CS_ENABLE;                 // CAN CS goes Low.
//	
//    MCP_SPI_Write( CAN_CMD_READ ); // send CAN_CMD_READ
//    MCP_SPI_Write( address );      // Set the Address		
//    Ret_Data = MCP_SPI_Read();     // Read the requsted register
//	
//    MCP_CS_DISABLE;                // CAN CS goes High.
	
    return Ret_Data;
}


/***********************************************************************************
*  Function   : MCP_SPI_Write ()
*----------------------------------------------------------------------------------
*  Object     : This function is called whenever a write to the SPI bus is occured
*               When data is written to SPI data register & transmission compeleted,
*			    SPI flag (end of transmition) is set and an interrupt is requested
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  data			   |  Data to be written
*------------------+-----------------------------------------------------------------
***********************************************************************************/
void MCP_SPI_Write( UChar data )
{
//    MCP_SPI_DATA = data;
//    ULong ResponseWaite=0;
//	
//    //while( !( MCP_SPI_FLAG_CHECK ));// SPI interrupt flag check
//    
//   
//    //	Set the SPI data
//    
//    while( !( MCP_SPI_FLAG_CHECK ))//&&(ResponseWaite<100000))
//    {
//      		//__watchdog_reset();
//		//__delay_cycles(1);
//                ResponseWaite++;
//    }
	    
}

/***********************************************************************************
*  Function   : MCP_REG_Write
*----------------------------------------------------------------------------------
*  Object     : To write data to the register at selected address.
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  cs              |  chip select of desired CAN Controller 0,1,2
*------------------+-----------------------------------------------------------------
*  address         |  Address of register & MCP2515 chip
*------------------+-----------------------------------------------------------------
*  data		       |  data to be written
*------------------+-----------------------------------------------------------------
***********************************************************************************/
void MCP_REG_Write( UChar cs, UChar address , UChar data)
{
//    MCP_CS_ENABLE;                        // CAN CS goes Low.
//	
//    MCP_SPI_Write( CAN_CMD_WRITE );
//    MCP_SPI_Write( address );
//    MCP_SPI_Write( data );
//	
//    MCP_CS_DISABLE;                       // CAN CS goes High.
}

/***********************************************************************************
*  Function   : MCP_REG_BitModify
*----------------------------------------------------------------------------------
*  Object     : To write data to the register at unmasked locations. The mask byte
*				determines which bits in the register will	be allowed to change.
*				a ‘1’ in the mask byte will allow a bit in the register to change
*				a ‘0’ will not.
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  cs              |  chip select of desired CAN Controller 0,1,2
*------------------+-----------------------------------------------------------------
*  address         |  Address of register & MCP2515 chip
*------------------+-----------------------------------------------------------------
*  data		       |  data to be written
*------------------+-----------------------------------------------------------------
*  mask		       |  mask of register
*------------------+-----------------------------------------------------------------
***********************************************************************************/
void MCP_REG_BitModify( UChar cs, UChar address, UChar data, UChar mask )
{
//    MCP_CS_ENABLE; 
//	
//    MCP_SPI_Write( CAN_CMD_BitModify );
//    MCP_SPI_Write( address );
//    MCP_SPI_Write( mask );
//    MCP_SPI_Write( data );
//	
//    MCP_CS_DISABLE;
}
/***********************************************************************************
*  Function   : MCP_SPI_WriteBuffer
*----------------------------------------------------------------------------------
*  Object     : To write data to Tx buffer Data locations.
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  cs              |  chip select of desired CAN Controller 0,1,2
*------------------+-----------------------------------------------------------------
*  txRegister      |  Tx Buffer Number( CAN_TXBUffData0,1,2 )
*------------------+-----------------------------------------------------------------
*  msg		       |  data to be written
*------------------+-----------------------------------------------------------------
***********************************************************************************/
void MCP_SPI_WriteBuffer( UChar cs, UChar txRegister, tCANMsg msg )
{
//    MCP_CS_ENABLE;
//	
//	//  Putting the MCP's TX buffer 0 in writing mode
//    MCP_SPI_Write( CAN_CMD_LoadTXBuffer | txRegister );
//	
//	//  Write the data register
//    for( UChar i = 0; i < msg.DataLen; i++ )
//        MCP_SPI_Write( msg.Data[i] );
//	
//    MCP_CS_DISABLE;
}


///***********************************************************************************
//*  Function   : MCP_RTS
//*----------------------------------------------------------------------------------
//*  Object     : To order the controller to start message transmission
//*
//*  Return     : None
//*
//*------------------------------------------------------------------------------------
//*  Input Arguments |  Description
//*------------------+-----------------------------------------------------------------
//*  cs              |  chip select of desired CAN Controller 0,1,2
//*------------------+-----------------------------------------------------------------
//*  txRegister      |  Tx Buffer Number( CAN_TXBUffData0,1,2 )
//*                  |        
//*                  |  address  |  Address Points to
//*                  |  ===========================
//*                  |  0x01			TXB0		
//*                  |  0x02			TXB1		
//*                  |  0x04			TXB2	
//*------------------+-----------------------------------------------------------------
//***********************************************************************************/
//void MCP_RTS( UChar cs, UChar txRegister )
//{
//    //MCP_Interrupt_Disable;
//    MCP_CS_ENABLE;
//	
//    MCP_SPI_Write( CAN_CMD_RTS | txRegister );
//	
//    MCP_CS_DISABLE;
//    //MCP_Interrupt_Enable;
//}



/***********************************************************************************
*  Function   : MCP_ReadStatus
*----------------------------------------------------------------------------------
*  Object     : it is called to read status bits for transmit and recieve functions
*   			 Bit    |   Status
*			     ===========================
*				 0		  CANINTF.RX0IF
*				 1		  CANINTF.RX1IF
* 				 2		  TXB0CNTRL.TXREQ
*				 3		  CANINTF.TX0IF
*				 4		  TXB1CNTRL.TXREQ
*				 5		  CANINTF.TX1IF
*				 6		  TXB2CNTRL.TXREQ
* 				 7		  CANINTF.TX2IF
*
*  Return     : Ret_Data: read status of MCP2515
*
*  Note(s)    : FOR DEBUG ONLY
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  cs              |  chip select of desired CAN Controller 0,1,2
*------------------+-----------------------------------------------------------------
***********************************************************************************/
UChar MCP_ReadStatus (UChar cs)
{
    UChar Ret_Data = 0;
//	
//    MCP_CS_ENABLE;
//	
//	MCP_SPI_Write(CAN_CMD_ReadStatus);    // Set the command		
//	Ret_Data = MCP_SPI_Read();		    // Read the status
//	
//    MCP_CS_DISABLE;
	
    return Ret_Data;
}

/***********************************************************************************
*  Function   : MCP_ReadSRxStatus
*----------------------------------------------------------------------------------
*  Object     : to read status that indicates filter match and message type of
*				 recieved message
*
*               Bits 7,6    |  Received Message
*			     ==================================
*				 0 0			No RX message
*				 0 1			Message in RXB0
*				 1 0			Message in RXB1
*				 1 1			Messages in both buffers
*
*				 Bits 4,3    |  Msg Type Received
*				 =====================================
*				 0 0			Standard data frame
*				 0 1			Standard remote frame
*				 1 0			Extended data frame
* 				 1 1			Extended remote frame
*
*				 Bits 2,1,0  |  Filter Match
* 				 =====================================
*				 0 0 0			RXF0	
*				 0 0 1			RXF1	
*				 0 1 0			RXF2	
*				 0 1 1			RXF3	
*				 1 0 0			RXF4	
*				 1 0 1			RXF5	
*				 1 1 0			RXF0(rollover to RXB1)	
*				 1 1 1			RXF1(rollover to RXB1)	
*
*  Return     : Ret_Data: read status of MCP2515
*
*  Note(s)    : FOR DEBUG ONLY
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  cs              |  chip select of desired CAN Controller 0,1,2
*------------------+-----------------------------------------------------------------
***********************************************************************************/
UChar MCP_ReadRxStatus (UChar cs)
{
    UChar Ret_Data = 0;
	
//    MCP_CS_ENABLE;
//	
//	MCP_SPI_Write(CAN_CMD_RXStatus);	// Set the command	
//	Ret_Data = MCP_SPI_Read();		// Read the status
//	
//    MCP_CS_DISABLE;
	
    return Ret_Data;
}

//***********************************************************************************
// No More
//***********************************************************************************