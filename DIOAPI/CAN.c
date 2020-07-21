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
#include "Config.h"
#include "Common.h"
#include "CAN.h"
#include "WDManager.h"
#include "terminal.h"


#ifdef HART_ENABLED
	#include "HART.h"
#endif

/***********************************************************************************
*  Local variables
***********************************************************************************/
UChar CAN_Mode;           // NORMAL, LOOP_BACK, LISTEN_ONLY, ...
tCANMsg CAN_RxMsg;        // CAN Rx message structure
tCANMsg CAN_TxMsg;        // CAN Tx message structure
tCANController  CAN[ MAX_NUM_CONTROLLERS ]; // an array of tCANController type

tMessageQueue RxQueue;    // Queue for received packets from all the CAN Controllers

UChar UnwantedPacketReceived;
UChar CAN_ExpectedReceiveCount; // number of packets that should be received from MPU

//extern tDataBuffer  HART_CAN_TX_Buffer;
//extern tDataBuffer  HART_CAN_RX_Buffer;
//extern bool ReqBuff[8][3];
//extern struct ReqDatas   AssetMngReq[8];
UChar Command;
extern int watchdogEnable;
//extern int watchdogtimeout;
extern int deviceHandle;
Char errCANIF[]            = "\r FATAL: CANERRIF! {CAN%d}\n";
Char errSettingSize[]	  = "\r ERROR: setting size mismatch\n";
Char msgHARTupload[]		  = "\r INFO: Initiate Request for Upload{%d} \n";
Char msgHARTdownload[]	  = "\r INFO: Initiate Request for Download{%d}  \n";
Char errCANTXFull[]	  	  = "\r ERROR: TX Queue Full!\n";
Char msgTransportInit[] 	  = "\r INFO: Transport Layer Initialized\n";
Char msgTransportNOTInit[] = "\r INFO: Transport Layer NOT Initialized \n";

Char errCANnotAvailable[]  = "\r ERROR: CAN Bus %d Unavailable !!\n";
Char msgWaitingACKonCAN[]  = "\r INFO: Waiting for Ack On CAN%d T=%d \n";
Char msgCANErrTimeOut[]    = "\r INFO: Time = %d OR CANErorr = %X \n";
Char msgHearBeatSent[]     = "\r INFO: Heart Beat Sent!! \n";
int s, i; 
int nbytes;
struct sockaddr_can addr;
struct ifreq ifr;
struct can_frame frame;
char buff[20] ;
char data[16];
char sharp = '#';
char zero = '0';
char temp[4] = "000";
//====================================================================================
//  SPI Methods
//====================================================================================
/***********************************************************************************
*  Function   : CAN0INTHandler ()
*----------------------------------------------------------------------------------
*  Object     : Define this interrupt handler to handle interrupt events (INT2)
*
*  Return     : None
*
***********************************************************************************/
//#pragma vector = CAN0_INTERRUPT_VECT
void *CAN0INTHandler(void)
{
	// FOR DEBUG ONLY: to check out if interrupt CAN_0 is occurred 
	// printf_P("\r CAN0 INT\n");
	
    MCP_CS0_INTERRUPT_DISABLE;
	
    //	in both CAN 0 & 1
	//	Reset the counter for warning of  the CAN's communication failure
    Card.Timer2_CounterA = 0;
    InterruptRoutine( CAN_CHIP0, 0 );
    Card.LCPCError.Bit.Link1 = 0;
	
    MCP_CS0_INTERRUPT_ENABLE;
}

/***********************************************************************************
*  Function   : CAN1INTHandler ()
*----------------------------------------------------------------------------------
*  Object     : Define this interrupt handler to handle interrupt events (INT3)
*
*  Return     : None
*
***********************************************************************************/
//#pragma vector = CAN1_INTERRUPT_VECT
void *CAN1INTHandler(void)
{
    MCP_CS1_INTERRUPT_DISABLE; 	
	
//	in both CAN 0 & 1:
//	Reset the counter for warning of  the CAN's communication failure
    Card.Timer2_CounterA = 0 ;
    InterruptRoutine( CAN_CHIP1, 1 ) ;
    Card.LCPCError.Bit.Link2 = 0;
	
    MCP_CS1_INTERRUPT_ENABLE;
}

/***********************************************************************************
*  Function   : CAN2INTHandler ()
*----------------------------------------------------------------------------------
*  Object     : Define this interrupt handler to handle interrupt events (INT6)
*
*  Return     : None
*
***********************************************************************************/
//#pragma vector = CAN2_INTERRUPT_VECT
//__interrupt void CAN2INTHandler(void)
//{
//    MCP_CS2_INTERRUPT_DISABLE; 	
//	
//    InterruptRoutine( CAN_CHIP2, 2 );
//    Card.LCPCError.Bit.Link3 = 0;
//	
//    MCP_CS2_INTERRUPT_ENABLE;
//}

//====================================================================================
//  Application Methods
//====================================================================================

/***********************************************************************************
*  Function   : InterruptRoutine ()
*----------------------------------------------------------------------------------
*  Object     : if GetRxMessage returns 0 (RET_SUCCESS or FAIL because of overflow)
*				 one message is read and RxQueue.Tail/RxQueue.Head are initilized in
*				 order to receive next message!
*
*  Return     : None
*		
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  cs              |  chip select  0,1,2
*------------------+-----------------------------------------------------------------
*  id              |  id of desired CAN Controller 0,1,2
*------------------+-----------------------------------------------------------------
***********************************************************************************/
void InterruptRoutine( UChar cs, UChar id )
{
    UChar value;
	
    if( Card.HasDiag == 0 )
    {
        Card.LCPCWarning.Bit.DataReceive   = 0 ; // CAN Receive Error (CANINTF_ERRIF)
        Card.LCPCWarning.Bit.DataTransmit  = 0;  // CAN Transmit Error(CANINTF_MERRF)
        Card.LCPCWarning.Bit.Communication = 0 ;
    }
	
    while( true )
    {
		//	The CANINTF register contains the corresponding interrupt flag bit for each
		//	8 interrupt source in MCP2515
		value = MCP_REG_Read( cs, CANINTF );// Read CANINTF register
        if( value == 0 )
            break ;
		
        // The Receive Buffer 0 Full Interrupt
        if(( value & MCP_CANINTF_RX0IF_M ) == MCP_CANINTF_RX0IF_S )
        {
            FetchMessage( cs, CAN_Buffer0_SIDH );
			
			// Reset the desired interrupt flag to enable another interrupt occurance
            MCP_REG_BitModify( cs, CANINTF, MCP_CANINTF_RX0IF_R,MCP_CANINTF_RX0IF_M);
        }
        // The Receive Buffer 1 Full Interrupt
        if(( value &  MCP_CANINTF_RX1IF_M ) ==  MCP_CANINTF_RX1IF_S )
        {
            FetchMessage( cs, CAN_Buffer1_SIDH );
			
			// Reset the desired interrupt flag to enable another interrupt occurance
            MCP_REG_BitModify( cs, CANINTF, MCP_CANINTF_RX1IF_R,MCP_CANINTF_RX1IF_M);
        }
		
        // The Transmit Buffer 0 Empty Interrupt
        if(( value & MCP_CANINTF_TX0IF_M ) == MCP_CANINTF_TX0IF_S )
        {
            MCP_REG_BitModify( cs, CANINTF, MCP_CANINTF_TX0IF_R,MCP_CANINTF_TX0IF_M);
            CAN[id].IsFullTxBuff0 = 0;
            Card.CAN_TX_Timeout_Timer[id] = 0;
            if ( !CAN[id].IsFullTxBuff0 && !CAN[id].IsFullTxBuff1 && !CAN[id].IsFullTxBuff2 )
                CAN[id].TXDONE = true;
        }
        // The Transmit Buffer 1 Empty Interrupt
        if(( value & MCP_CANINTF_TX1IF_M ) == MCP_CANINTF_TX1IF_S )
        {
            MCP_REG_BitModify( cs, CANINTF, MCP_CANINTF_TX1IF_R,MCP_CANINTF_TX1IF_M);
            CAN[id].IsFullTxBuff1 = 0;
            Card.CAN_TX_Timeout_Timer[id] = 0;
            if ( !CAN[id].IsFullTxBuff0 && !CAN[id].IsFullTxBuff1 && !CAN[id].IsFullTxBuff2 )
                CAN[id].TXDONE = true;
        }
        // The Transmit Buffer 2 Empty Interrupt
        if(( value & MCP_CANINTF_TX2IF_M ) == MCP_CANINTF_TX2IF_S )
        {
            MCP_REG_BitModify( cs, CANINTF, MCP_CANINTF_TX2IF_R,MCP_CANINTF_TX2IF_M);
            CAN[id].IsFullTxBuff2 = 0;
            Card.CAN_TX_Timeout_Timer[id] = 0;
            if ( !CAN[id].IsFullTxBuff0 && !CAN[id].IsFullTxBuff1 && !CAN[id].IsFullTxBuff2 )
                CAN[id].TXDONE = true;
        }
		
        // The Message Error Interrupt
        if(( value & MCP_CANINTF_MERRF_M ) ==  MCP_CANINTF_MERRF_S )
        {
			//Error Int is not necessarilly related to RX
            Card.LCPCWarning.Bit.DataTransmit = 1;
            MCP_REG_BitModify(cs, CANINTF, MCP_CANINTF_MERRF_R, MCP_CANINTF_MERRF_M);
			
			//===================================================================
            Card.CAN_EFLG = MCP_REG_Read( cs, EFLG );
            MCP_REG_Write( cs, EFLG, 0x00 );
			//===================================================================
        }
        // The Error Interrupt
        if(( value & MCP_CANINTF_ERRIF_M ) == MCP_CANINTF_ERRIF_S )
        {
		    //Error Int is not necessarilly related to RX
            Card.LCPCWarning.Bit.DataReceive = 1;
            MCP_REG_BitModify(cs, CANINTF, MCP_CANINTF_ERRIF_R, MCP_CANINTF_ERRIF_M);
			
			//==========================================================================
            Card.CAN_EFLG = MCP_REG_Read( cs, EFLG );
            MCP_REG_Write( cs, EFLG, 0x00 );
			//==================================================================================
        }
		
	    if (watchdogEnable == WDT_ENABLE)
		    feedWD(deviceHandle);
    }
	
    if(( Card.LCPCWarning.Bit.DataReceive  == 1 ) || ( Card.LCPCWarning.Bit.DataTransmit == 1 ))
    {
        Card.CAN_EFLG = MCP_REG_Read( cs, EFLG );
        MCP_REG_Write( cs, EFLG, 0x00 );
		
		//	EWARN: Error Warning Flag bit
		//	--- Set when TEC or REC is equal to or greater than 96 (TXWAR or RXWAR = 1)
		//	(Transmit Error Counter) or (Receive Error Counter)
        if(( Card.CAN_EFLG & MCP_EFLG_EWARN_M ) == MCP_EFLG_EWARN_S )
        {
			//	in this condition, we should initilize CAN again!
            Card.LCPCWarning.Bit.Communication = 1;
			
            printf( errCANIF, cs ) ;  
            CAN_Init( CAN_Mode );
        }
		
		//	RX0OVR: Receive Buffer 0 Overflow Flag bit
		//	--- Set when a valid message is received for RXB0 and CANINTF.RX0IF = 1
        if(( Card.CAN_EFLG & MCP_EFLG_RX0OVR_M ) == MCP_EFLG_RX0OVR_S )
        {
			// its flag should be reset manually by MCU
            MCP_REG_BitModify( cs, CANINTF, MCP_CANINTF_RX0IF_R,MCP_CANINTF_RX0IF_M);

			// FOR DEBUG ONLY: CAN RX0 overflow
            printf("\r_1_\n");
        }
		
		//	RX1OVR: Receive Buffer 1 Overflow Flag bit
		//	--- Set when a valid message is received for RXB1 and CANINTF.RX1IF = 1
        if(( Card.CAN_EFLG & MCP_EFLG_RX1OVR_M ) == MCP_EFLG_RX1OVR_S )
        {
			// its flag should be reset manually by MCU
            MCP_REG_BitModify( cs, CANINTF, MCP_CANINTF_RX1IF_R,MCP_CANINTF_RX1IF_M);
            
			// FOR DEBUG ONLY: CAN RX0 overflow
			printf("\r_2_\n");
        }
    }
	
	// Card.LCPCWarning.Bit.DataReceive = 0 ;
	// Card.LCPCWarning.Bit.DataTransmit = 0 ;
	
    //Send messages in the queue if there is an empty txBuffer
    if( CAN[id].TxQueue.Length != 0 )
    {
		//	if IsFullTxBuff0 == 0, it means that Tx Buffer is empty!
		//	So, if you are here in Interrupt routine and there is an empty Tx Buffer,
		//	That's Great!!! send some messages from queue immediately!
		if(!CAN[id].IsFullTxBuff0 || !CAN[id].IsFullTxBuff1 || !CAN[id].IsFullTxBuff2)
            CAN_SendOn( cs, 1, 0 ) ;
    }
	
    if( CAN_ExpectedReceiveCount != 0 )
    {
        if( RxQueue.Length == CAN_ExpectedReceiveCount )
            CAN_ExpectedReceiveCount = 0;
        else
            return;
    }
	
    ProcessRxQueue() ;
}


/***********************************************************************************
*  Function   : FetchMessage ()
*----------------------------------------------------------------------------------
*  Object     : it reads from one of the 2 RX buffers (no need to mention address)
*
*  Return     : None
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  cs              |  CAN_CHIP0,1,2 => MCP2515 ChipSelect pins in uController
*------------------+-----------------------------------------------------------------
*  startRegister   |  CAN's Buffer0/1 SIDH (Standard ID)
*                  |  startRegister format: 1001 0nm0 (nm: 00, 01, 10, 11)
*------------------+-----------------------------------------------------------------
***********************************************************************************/
void FetchMessage( UChar cs, UChar startRegister )
{
    UChar n;
	
//    MCP_CS_ENABLE;
	
	//	ReadRXBuf Command + Start Register to read(nm: 00 or 10==> 0x90 or 0x94 )
	//	This command orders MCP2515 to send message ingredients start from Address
	//	0x61(RXB0SIDH) or 0x71(RXB1SIDH)
    MCP_SPI_Write( CAN_CMD_ReadRXBuffer | startRegister );
	
	//	Increment Tail by one, after each message reception, in FIFO philosophy this tail
	//	should be read as "head" later in GetRxMessage()!!!
    RxQueue.Tail++;
    if( RxQueue.Tail == MAX_QUEUE_SIZE ) // MAX QUEUE SIZE is limited!!!
    {
        //TODO: Report Buffer-Full
        return;
    }
	
    n = RxQueue.Tail ;  //Save current queue position
	
    //----------------------------------------
    //Read all the protocol data unit (PDU): SID= Standard ID/ EID= Extended ID
    //----------------------------------------
	
	//	if startRegister == CAN_buffer0_SIDH :: RXB0SIDH, RXB0SIDL, RXB0EID8, RXB0EID0
	//	if startRegister == CAN_buffer1_SIDH :: RXB1SIDH, RXB1SIDL, RXB1EID8, RXB1EID0
    for( UChar i = 0; i < 4; i++ ) // Read the first 4 ones from RX buffer
        RxQueue.Msg[n].ID[i] = MCP_SPI_Read();
	
	//  the 2nd read register is RXB0SIDL or RXB1SIDL
    RxQueue.Msg[n].ID[1] &= 0xE0 ;   // SIDL has only 3-bits (Cmd1 Cmd0 IO - - - - -)
	
	//  RXB0DLC or RXB1DLC (Data length Control)
	RxQueue.Msg[n].DataLen = MCP_SPI_Read();
    RxQueue.Msg[n].DataLen &= 0x0F ; // Only the lowest 4-bits
	
	//	RXB0D0-D7 or RXB1D0-D7
    for( UChar i = 0; i < RxQueue.Msg[n].DataLen; i++ )
        RxQueue.Msg[n].Data[i] = MCP_SPI_Read();
	
    RxQueue.Msg[n].CS = cs;
	
	//	it does not actually show the lenght concept, instead it's the symbol of counter
	//	in FIFO RW routines, we increment RxQueue.Lenght here, then in GetRxMessage func
	//	firstly check if it isn't 0, then read the message and decrement RxQueue.Lenght
    RxQueue.Length++ ;
	
//	MCP_CS_DISABLE ;
}

/***********************************************************************************
*  Function   : GetRxMessage ()
*----------------------------------------------------------------------------------
*  Object     : it reads the first data from message FIFO queue
*
*  Return     : RET_SUCCESS : if there is data to read
*               1           : if there is no data
*				FAIL        : if it is overflow. (FAIL is like RET_SUCCESS = 0)
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
***********************************************************************************/
UChar GetRxMessage()
{
	//	check if RxQueue.Length != 0, to read received message
    if( RxQueue.Length != 0 )
    {
        RxQueue.Head++;
        if( RxQueue.Head == MAX_QUEUE_SIZE )
            return FAIL;  
		
		//	move the appropriate RxQueue.Msg to CAN_RxMsg
        CAN_RxMsg = RxQueue.Msg[ RxQueue.Head ];
		
        RxQueue.Length-- ;
		
        return RET_SUCCESS;
    }
	
    return 1 ;
}

/***********************************************************************************
*  Function   : PreProcessMessage
*----------------------------------------------------------------------------------
*  Object     : To preprocess received message (common for all cards) and extract
*               what to do for all commands except FCP_CMD_SET_GET_DATA, actually
*				it processes messages which are recieved in CAN interface and it is
*				called when its flag is set in the CAN interrupt service routine
*               and processes the  message that is received.
*
*  Return     : 0 : successful (Call ProcessMessage( command, dataLen ) )
*				1 : otherwise (different reasons)
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  command		   |  received command 
*------------------+-----------------------------------------------------------------
*  dataLen		   |  Data Length
*------------------+-----------------------------------------------------------------
***********************************************************************************/
UChar rxReqCounter = 0;
UChar PreProcessMessage( UChar command, UChar dataLen )
{
    UChar extraByte    = 0;
    UChar remainedSize = 0;
//	
//#ifdef HART_ENABLED
//    UChar ByteCount;
//#endif
//	
//    if( CAN_Mode == MODE_LOOPBACK )
//    {
//        if((( CAN_RxMsg.Address != 0 ) && ( CAN_RxMsg.Address != Card.BPAddress )) ||
//           (( CAN_RxMsg.Address == 0 ) && ( CAN_RxMsg.Group != Card.Group )&&(Card.Group!=INOUT_GROUP)) ||
//			   ( CAN_RxMsg.Command != CAN_TxMsg.Command ) ||
//				   ( CAN_RxMsg.DataLen != CAN_TxMsg.DataLen ))
//        {
//            __watchdog_reset() ;
//			
//			// FOR DEBUG ONLY: check out CAN message cs, command, group, ...
//			printf_P("\r UNWANTED: {%d} Command=0x%X Len=%d\n", CAN_RxMsg.CS, command, dataLen );
//			printf_P("\r Address=0x%X, Group=%d\n", CAN_RxMsg.Address, CAN_RxMsg.Group );
//            __watchdog_reset() ;
//            UnwantedPacketReceived = 1 ;
//            __watchdog_reset() ;
//            return 0 ;
//        }
//        return 0 ;
//    }
//	
//    // Decode the command byte (Please refer to Farineh Internal CAN Protocol doc)
//    switch( command )
//    {
//	  case FCP_CMD_SET_GET_STATE:            
//		IOCard.DataIndex = 0;       // NOTE : Reseting the IO Card Data Index
//		if( dataLen == 0 )   // Send your status to MPU
//		{
//			rxReqCounter++ ;
//			/*******************************************************************
//			*  response to SET_GET_STATE with Data Length field 0
//			*------------------------------------------------------------------
//			*	Byte0: DS CS2 CS1 CS0 CT3 CT2 CT1 CT0
//			*	DS: Diagnostic State -->> 1: it has, 0: it doesn't have
//			*
//			*	Card State :
//			*	State				Code	CS2 CS1 CS0
//			*	Idle				1		0 	0 	1
//			*	Main				2		0	1	0
//			*	Backup				3		0	1	1
//			*	Main   Fail Safe	4		1	0	0
//			*	Backup Fail Safe	5		1	0	1
//			*	Out of Service		6		1	1	0
//			*
//			*	Card Type:
//			* 	Type				Code	CT3 CT2 CT1 CT0
//			*	DI (DI530)	2		0	0	1	0
//			*   DO (DO580)	4 		0	1	0	0
//			* 	DO (DO571)	5		0	1	0	1
//			*	AI (AI621)	6		0	1	1	0	(Temperature)
//			*	AO (AO673)	8		1	0	0	0
//			*	AI (AI623)	9		1	0	0	1
//			*	AI (AI622)	B		1	0	1	1
//			*   AO (AO672)	D		1	1	0	1
//			*	
//			*	Byte0:	As described formerly
//			*	Byte1:	for (AI622, AO672): 	HART response byte
//			*			otherwise		  :		0
//			*
//			*  if Diagnostic State == 0
//			*  Byte2:  for (AI622, AO672):		HART Command
//			*			otherwise		  :		0
//			*
//			*  if Diagnostic State == 1
//			*	Byte2:	IOPC Card Error Byte
//			*	Byte3:	IOPC Card Warning Byte
//			*	Byte4:	IO Card Error Byte
//			*	Byte5:	IO Card Warning Byte
//			*	for DO571 module
//			*	Byte6:	IO Card Warning Byte2
//			*	Byte7:	IO Card Warning Byte3
//			*
//			*******************************************************************/						
//			
//                       
//                        /*if(Card.MAPCS_VER==MAPCS1)
//                        {
//                            CAN_TxMsg.Command = FCP_CMD_SET_GET_STATE;
//                            CAN_TxMsg.DataLen = 3;                            
//                            // Get the current state
//                            CAN_TxMsg.Data[0] = ( Card.HasDiag << 7 )|( Card.State << 4 )|( Card.Type >> 4 );             			
//                            CAN_TxMsg.Data[1] = rxReqCounter; // it is based on new definition 
//                            CAN_TxMsg.Data[2] = Command;      // it is not HART, it is ZERO!
//                        }
//                        else if(Card.MAPCS_VER==MAPCS2A)
//                        {
//                            CAN_TxMsg.Command = FCP_CMD_SET_GET_STATE;
//                            CAN_TxMsg.DataLen = 3;                            
//                            // Get the current state
//                            CAN_TxMsg.Data[0] = ( Card.HasDiag << 7 )|( Card.State << 4 )|( 0xC8 >> 4 );             			
//                            CAN_TxMsg.Data[1] = ( Card.Type >> 4 );//rxReqCounter;
//                            CAN_TxMsg.Data[2] = Command;      // it is not HART, it is ZERO!
//                        }  
//                        //removed by vaghari
//#ifdef  HART_ENABLED
//			CAN_TxMsg.Data[1] = Card.HARTState; // the same HART response byte
//
//#endif*/                        
//                            CAN_TxMsg.Command = FCP_CMD_SET_GET_STATE;
//                            CAN_TxMsg.DataLen = 3;                            
//                            // Get the current state
//                            CAN_TxMsg.Data[0] = ( Card.HasDiag << 7 )|( Card.State << 4 )|( Card.Type >> 4 );  
//                            if(Card.MAPCS_VER==MAPCS1)                    
//                             CAN_TxMsg.Data[1] = rxReqCounter&0x7F; // it is based on new definition 
//                            else                         
//                             CAN_TxMsg.Data[1] = (rxReqCounter&0x7F)|0x80; // it is based on new definition 
//                             
//
//                            
//                            CAN_TxMsg.Data[2] = Command;      // it is not HART, it is ZERO!
//                            
//#ifdef  HART_ENABLED
//                        if(Card.MAPCS_VER==MAPCS1)
//			 CAN_TxMsg.Data[1] = Card.HARTState&0x7F; // the same HART response byte
//                        else
//                         CAN_TxMsg.Data[1] = (Card.HARTState&0x7F)|0x80;; // the same HART response byte 
//
//#endif
//	
////printf_P("\r MAPCS%x=%x \r\n",Card.MAPCS_VER,CAN_TxMsg.Data[1]);
//			if( Card.HasDiag )
//			{
//                          //printf_P("\r %x %x %x %x %xdiag \n",CAN_TxMsg.Data[2] ,CAN_TxMsg.Data[3] ,CAN_TxMsg.Data[4],CAN_TxMsg.Data[5],CAN_TxMsg.Data[5]);
//                              
//				CAN_TxMsg.DataLen = 6;
//				CAN_TxMsg.Data[2] = Card.PreviousLCPCError.D8; //overwrite byte2
//				CAN_TxMsg.Data[3] = Card.PreviousLCPCWarning.D8;
//				CAN_TxMsg.Data[4] = IOCard.PreviousIOError.D8;
//				CAN_TxMsg.Data[5] =IOCard.PreviousIOWarning.D8;
//                                //printf_P("%x \r\n",CAN_TxMsg.Data[5]);
//                               
//
//#if (CARD_CODE == AI622_CODE || CARD_CODE == AI623_CODE || CARD_CODE == AI624_CODE)
//				CAN_TxMsg.DataLen = 7;
//				CAN_TxMsg.Data[6] = IOCard.PreviousIOWarning2.D8;
//#endif
//	
//#if (CARD_CODE == AI621_CODE)
//                                //if(Card.MAPCS_VER!=MAPCS1)
//                                //{
//				 CAN_TxMsg.DataLen = 7;
//				 CAN_TxMsg.Data[6] = IOCard.PreviousIOWarning2.D8;
//                                 //printf_P("5=%x 6=%x\r\n",IOCard.PreviousIOWarning.D8,IOCard.PreviousIOWarning2.D8 );
//                                //}
//#endif                                
//
//#if (CARD_CODE == DO571_CODE)
//				CAN_TxMsg.DataLen = 8;
//				CAN_TxMsg.Data[6] = IOCard.PreviousIOWarning2.D8;
//				CAN_TxMsg.Data[7] = IOCard.PreviousIOWarning3.D8;
//#endif
//				Card.HasDiag = 0 ;
//			}
//			
//			CAN_Send();
//			Card.HARTState = 0;
//			
//			// FOR DEBUG ONLY: check out number of channel and related data
//			// if( IOCard.Data[IOCard.AnalogChannel].D32 > 100 )
//			// 		printf_P("\n D[%d]=%d \r", IOCard.AnalogChannel, IOCard.Data[IOCard.AnalogChannel].D32 );   
//		}
//		else if( dataLen == 2 ) // Change IO Card state based on MPU request
//		{
//			if( Card.State != OUT_OF_SERVICE )
//			{
//				// Set the new state based on MPU request
//                         //if((Card.Group!=INOUT_GROUP)||((Card.Group==INOUT_GROUP)&&(CAN_RxMsg.Group==Card.Group)))
//                         //{
//                  				Card.NewState = CAN_RxMsg.Data[0];	
//				if(( Card.PreviousState == FAIL_SAFE_MAIN ) || ( Card.PreviousState == FAIL_SAFE_BACKUP ))
//				{
//					Card.HasDiag = 0;
//					Card.PreviousLCPCError.D8   = 0;
//					Card.PreviousLCPCWarning.D8 = 0;
//					IOCard.PreviousIOError.D8   = 0;
//					IOCard.PreviousIOWarning.D8 = 0;
//				}
//                        // }
//			}
//		}
//        break ;
//		
//	  case FCP_CMD_SET_SETTING:           
//		//Terminate Setting download
//		if( dataLen == 4 )
//		{
//			if(( CAN_RxMsg.Data[0] == 0x12 ) && ( CAN_RxMsg.Data[1] == 0x34 ) && ( CAN_RxMsg.Data[2] == 0x56 ) && ( CAN_RxMsg.Data[3] == 0x78 ))
//			{
//				// MPU sends Terminate Setting Download Command to IO Card!
//				Card.SettingOffset = 0;
//				return 1 ;
//			}
//		}
//		
//		//	Check the size of setting provided by MPU in the first byte
//		if( Card.SettingOffset == 0 )
//		{
//			if( CAN_RxMsg.Data[ 0 ] != Card.SizeOfSetting )
//			{
//				CAN_TxMsg.Data[0] = RET_SETTING_INVALID_SIZE ;
//				CAN_TxMsg.Command = FCP_CMD_SET_SETTING ;
//				CAN_TxMsg.DataLen = 1;
//				CAN_Send();	
//                               
//				printf_P(errSettingSize) ;
//				return 1 ;
//			}
//			else    //First packet, including size of setting
//			{
//				//Compute the number of packets that should be received from MPU
//				CAN_ExpectedReceiveCount = ((Card.SizeOfSetting+1) / 8) ;
//				if(( Card.SizeOfSetting + 1 ) % 8 )
//					CAN_ExpectedReceiveCount++;
//				
//				CAN_ExpectedReceiveCount--;//The first packet is already received
//				
//				// FOR DEBUG ONLY: to check out receive count
//				// printf_P( "\r ->%d \n", CAN_ExpectedReceiveCount ) ;
//				
//				CAN_TxMsg.Data[0] = RET_SETTING_OK ;
//				CAN_TxMsg.Command = FCP_CMD_SET_SETTING ;
//				CAN_TxMsg.DataLen = 1;
//				CAN_Send();
//				extraByte = 1 ;
//				dataLen -= extraByte ;
//			}
//		}
//		
//		remainedSize = Card.SizeOfSetting - Card.SettingOffset ;
//		// In case of 2 bytes remaining in the last segment, one byte is added to prevent command conflict
//		if( dataLen > remainedSize )
//			dataLen = remainedSize ;
//		
//		if( dataLen <= remainedSize )
//		{
//			for( int i = 0; i < dataLen; i++ )
//                        {
//				Setting.D8[ i + Card.SettingOffset ] = CAN_RxMsg.Data[ i + extraByte ] ;
//                                //printf_P("%d ",Setting.D8[ i + Card.SettingOffset ]);
//                        }
//			
//			Card.SettingOffset += dataLen;
//		}
//		
//		// FOR DEBUG ONLY: to check out data length
//		  //printf_P( "\r%d\n", dataLen ) ;
//		
//		if( Card.SettingOffset >= Card.SizeOfSetting )
//		{
//			Card.SettingOffset = 0 ;
//			
//			if( IsValidSetting() )
//			{
//				CAN_TxMsg.Command = FCP_CMD_SET_SETTING ;
//				CAN_TxMsg.DataLen = 1;
//				CAN_TxMsg.Data[0] = RET_SETTING_OK ;
//				CAN_Send();
//				Card.NewSettingReceived  = 1 ;
//			}
//			else
//			{
//				CAN_TxMsg.Command = FCP_CMD_SET_SETTING ;
//				CAN_TxMsg.DataLen = 1;
//				CAN_TxMsg.Data[0] = RET_SETTING_INVALID ;
//				CAN_Send();
//			}
//		}
//        break ;
//		
//	  case FCP_CMD_HOTREDUNDANCY:
//		switch( Card.State )
//		{
//		  case MAIN:
//			// Card.HotRedundancyRxTimer   = 0;
//			// Card.HotRedundancyFailCounter = 0;
//			break;
//		  case BACKUP:
//			ProcessPairResponse( 0 );
//			break;
//		}
//		return 1;
//				
//#ifdef HART_ENABLED
//	  case FCP_CMD_INITIATE_TRANSFER:
//		//	This command is used for initiating HART data downloading or uploading.
//		//	A message with Data Length 3 is sent from MPU. 3 bytes of data are:
//		//	Byte0:	1	initiates a request for download
//		//			2 	initiates a request for upload
//		//	Byte1: Number of bytes to be downloaded (only valid for download)
//		//	Byte2: CH on which hart cmd is going to be sent (only valid for download)
//		
//		if( dataLen == 3 )              // Initiate Request From MPU
//		{
//			//				IO module sends a 4 byte length acknowledge message to MPU
//			if (CAN_RxMsg.Data[0] == 2) // Initiate Request for Upload
//			{
//				//	Ack. To upload Initiation command:
//				//	Byte0:		2
//				//	Byte1:		1
//				//	Byte2:		Number of HART Data ready to be uploaded to MPU
//				//	Byte3:		channel
//				
//				CAN_TxMsg.Command = FCP_CMD_INITIATE_TRANSFER;
//				CAN_TxMsg.DataLen = 4;
//				CAN_TxMsg.Data[0] = 2;  // 1: Download  2: UPload
//				CAN_TxMsg.Data[1] = 1;  // Positive
//				CAN_TxMsg.Data[2] = HART_CAN_TX_Buffer.ByteCount;
//				CAN_TxMsg.Data[3] = HART_CAN_TX_Buffer.Channel;
//				HART_CAN_TX_Buffer.Pointer = 0;
//				HART_CAN_TX_Buffer.DataReadyToSend = true;
//				CAN_Send();
//				__watchdog_reset();
//				//printf_P(msgHARTupload , HART_CAN_TX_Buffer.ByteCount);
//			}
//			else if (CAN_RxMsg.Data[0] == 1) // Initiate Request for Download
//			{
//				// Ack. To download Initiation command:
//				// Byte0:		1
//				// Byte1:		1
//				// Byte2:		0xFF
//				// Byte3:		channel
//				
//				HART_CAN_RX_Buffer.ByteCount = CAN_RxMsg.Data[2];  
//				HART_CAN_RX_Buffer.Channel   = CAN_RxMsg.Data[1];
//				HART_CAN_RX_Buffer.Pointer   = 0;
//				HART_CAN_RX_Buffer.DataReadyToSend = false;
//				CAN_TxMsg.Command = FCP_CMD_INITIATE_TRANSFER;
//				CAN_TxMsg.DataLen = 4;
//				CAN_TxMsg.Data[0] = 1; // 1: Download  2: UPload
//				CAN_TxMsg.Data[1] = 1;
//				CAN_TxMsg.Data[2] = 0xFF;
//				CAN_TxMsg.Data[3] = HART_CAN_RX_Buffer.Channel;
//				CAN_Send();
//				__watchdog_reset();
//				//printf_P(msgHARTdownload, HART_CAN_RX_Buffer.ByteCount);
//			}
//		}
//		break ;
//		
//	  case FCP_CMD_UPLOAD_DATA:  // Upload HART Data to MPU
//		//	By this command in a 1 byte length message,MPU requests the IO module
//		//	to send the requested number of HART Data to MPU.If dataSize> 8 bytes
//		//	IO send them in more than one message consequently and since MPU
//		//	already knows number of data,it collects data from IO card responses.
//		//
//		//	Byte0:		HART Data 1
//		//	Byte1:		HART Data 2
//		//	Byte2:		HART Data 3
//		//	Byte3:		HART Data 4
//		//	Byte4:		HART Data 5
//		//	Byte5:		HART Data 6
//		//	Byte6:		HART Data 7
//		//	Byte7:		HART Data 8
//		
//		ByteCount = CAN_RxMsg.Data[0];
//		SendBlock(ByteCount);
//		break;
//		
//	  case FCP_CMD_DOWNLOAD_DATA: // Download MPU Data to HART
//		
//		for( UChar i = 0; i < dataLen; i++ )
//		{
//			HART_CAN_RX_Buffer.Byte[i+HART_CAN_RX_Buffer.Pointer]=CAN_RxMsg.Data[i];
//			__watchdog_reset();
//		}
//		
//		//	By this command MPU sends HART cmd and its parameters data to IO module.
//		//	If this number of data is more than 8bytes MPU sends in more than one
//		//	message and since IO module already knows number of data from previously
//		//	received Initiate download command, it will collect data from consequent
//		//	DOWNLOAD_DATA messages.
//		//
//		//	Byte0:		HART Command
//		//	Byte1:		HART Data 1
//		//	Byte2:		HART Data 2
//		//	Byte3:		HART Data 3
//		//	Byte4:		HART Data 4
//		//	Byte5:		HART Data 5
//		//	Byte6:		HART Data 6
//		//	Byte7:		HART Data 7
//		
//		__watchdog_reset();
//		HART_CAN_RX_Buffer.Pointer += dataLen;
//		if( HART_CAN_RX_Buffer.ByteCount == (HART_CAN_RX_Buffer.Pointer) )
//		{
//			// FOR DEBUG ONLY: to check out how many bytes received so far!
//			// printf_P("\r {%d} Bytes Received  \n", HART_CAN_RX_Buffer.Pointer);
//			CAN_TxMsg.Command = FCP_CMD_INITIATE_TRANSFER;
//			CAN_TxMsg.DataLen = 3;
//			CAN_TxMsg.Data[0] = 1;  // 1: Download  0: UPload
//			CAN_TxMsg.Data[1] = 0;
//			CAN_TxMsg.Data[2] = 0;
//			HART_CAN_RX_Buffer.Pointer =0;
//			HART_CAN_RX_Buffer.DataReadyToSend = true;
//			CAN_Send();
//			__watchdog_reset();
//			
//			AssetMngReq[HART_CAN_RX_Buffer.Channel].cmd = HART_CAN_RX_Buffer.Byte[0];  // HART Command in Byte[0] 
//			AssetMngReq[HART_CAN_RX_Buffer.Channel].DataLength = HART_CAN_RX_Buffer.ByteCount - 1;
//			if ( AssetMngReq[HART_CAN_RX_Buffer.Channel].DataLength != 0 )
//			{
//				for ( UChar i = 0 ; i < AssetMngReq[HART_CAN_RX_Buffer.Channel].DataLength ; i++ )
//				{
//					AssetMngReq[HART_CAN_RX_Buffer.Channel].Data[i] = HART_CAN_RX_Buffer.Byte[i+1];
//					__watchdog_reset();
//				}
//			}
//			
//			// FOR DEBUG ONLY: some check-up to be sure about data
//			//printf_P("\r Frame = [ ");
//			//for ( i = 0 ; i < AssetMngReq[HART_CAN_RX_Buffer.Channel].DataLength ; i++ )
//			//printf_P(" %X ", HART_CAN_RX_Buffer.Byte[i+1]);
//			//printf_P(" ]\n");
//			
//			ReqBuff[HART_CAN_RX_Buffer.Channel][0] = true;
//		}
//		else if ( HART_CAN_RX_Buffer.ByteCount < HART_CAN_RX_Buffer.Pointer )
//		{
//			// FOR DEBUG ONLY: Error Ocurred during Data Download
//			// printf_P("\r Error Ocurred during Data Download \n");
//			
//			__watchdog_reset();
//			
//			// FOR DEBUG ONLY: to check out received bytes HART RX
//			// printf_P("\r {%d} Bytes Received  \n", HART_CAN_RX_Buffer.Pointer);				
//		}
//		break ;
//#endif
//		
//	  case FCP_CMD_SET_GET_DATA:
//	  case FCP_CMD_DO_CALIBRATION:
//		break ;
//		
//	  default:
//		return 0 ;
//    }
//	
//    return ProcessMessage( command, dataLen ) ;
}

/***********************************************************************************
*  Function   : ProcessPairResponse()
*----------------------------------------------------------------------------------
*  Object     : This function is called during the Hot Redundancy routine and interprets
*				the pair response to take the action in case of redundancy switch.
*
*  Return     : None.
*			
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  PairState	   |  the state of pair switch
*------------------+-----------------------------------------------------------------
***********************************************************************************/
void ProcessPairResponse( UChar PairState )
{
	Card.HotRedundancyRxTimer   = 0;
	Card.HotRedundancyFailCounter = 0;
}

/***********************************************************************************
*  Function   : EncodeSID_H ()
*----------------------------------------------------------------------------------
*  Object     : To combine command and address into SIDH 
*
*  Return     : 0
*			
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  command		   |  the desired command 
*------------------+-----------------------------------------------------------------
*  address		   |  the desired address
*------------------+-----------------------------------------------------------------
***********************************************************************************/
UChar EncodeSID_H( UInt command, UInt address )
{
    UChar SIDH;
	
	SIDH = ( (address & 0x3F)<< 2 ) | ( (command & 0x0F) >> 2);

	return SIDH;
}

/***********************************************************************************
*  Function   : EncodeSID_L ()
*----------------------------------------------------------------------------------
*  Object     : To combine group and command into SIDL
*
*  Return     : 0
*			
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  group		   |  group input/output
*------------------+-----------------------------------------------------------------
*  command		   |  the desired command 
*------------------+-----------------------------------------------------------------
***********************************************************************************/
UChar EncodeSID_L( UInt group, UInt command )
{
    UChar SIDL;
	
	SIDL  = ( (command & 0x0F)<< 6 ) | ( group << 5 ) ;
	SIDL &= 0xE0 ;
	
	return SIDL;
}

/***********************************************************************************
*  Function   : DecodeSID ()
*----------------------------------------------------------------------------------
*  Object     : To extract group, command and address from SID
*
*  Return     : 0
*			
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  SID_H		   |  High Byte of SID
*------------------+-----------------------------------------------------------------
*  SID_L			   |  Low Byte of SID
*------------------+-----------------------------------------------------------------
***********************************************************************************/
UInt DecodeSID( UChar SID_H, UChar SID_L )
{
	CAN_RxMsg.Command = ((( SID_L & 0x03 ) << 2 ) | (( SID_H & 0xC0 ) >> 6 )) & 0x0F;
	CAN_RxMsg.Address = (( SID_L & 0xFC ) >> 2 ) & 0x3F;
	CAN_RxMsg.Group   = (( SID_H & 0x20 ) >> 5 ) & 0x01;
	
	return 0;
}

/***********************************************************************************
*  Function   : ProcessRxQueue ()
*----------------------------------------------------------------------------------
*  Object     : if GetRxMessage returns 0 (RET_SUCCESS or FAIL because of overflow)
*				one message is read and RxQueue.Tail/RxQueue.Head are initilized in
*				order to receive next message!
*
*  Return     : none
*			
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
***********************************************************************************/
void ProcessRxQueue( void )
{
    while( GetRxMessage() == 0 )
    {
	    if (watchdogEnable == WDT_ENABLE)
		    feedWD(deviceHandle);
		
		DecodeSID( CAN_RxMsg.ID[1], CAN_RxMsg.ID[0] );
        CAN_RxMsg.DataLen &= 0x0F;
		
		//	after extracting Command, DataLen, Address and Group, Call PreProcessMessage
        Card.RxReady = PreProcessMessage( CAN_RxMsg.Command, CAN_RxMsg.DataLen );
        
    }
	
	//	Head and Tail will be incremented before use, so they should be initiated by -1
    RxQueue.Tail = -1 ;
    RxQueue.Head = -1 ;
}

/***********************************************************************************
*  Function   : CAN_Send ()
*----------------------------------------------------------------------------------
*  Object     : it sends a message to CAN interface.
*
*  Return     : the return value of CAN_SendOn
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  None			   |  ---
*------------------+-----------------------------------------------------------------
***********************************************************************************/
UChar CAN_Send( void )
{
    return CAN_SendOn( CAN_RxMsg.CS, 0, 0 ) ;
}

/***********************************************************************************
*  Function   : CAN_SendOn()
*----------------------------------------------------------------------------------
*  Object     : it sends a message to CAN interface.
*
*  Return     : RET_SUCCESS : successful
*				otherwise :   failed
*
*--------------------------------------------------------------------------------------
*  Input Arguments   |  Description
*--------------------+-----------------------------------------------------------------
*  cs			     |  CAN_CHIP0-2 =>MCP2515 ChipSelect pins in uController
*--------------------+-----------------------------------------------------------------
*  useTxQueue	     |  if TxBuffers are full, use TX Queue
*--------------------+-----------------------------------------------------------------
*  useSpecificAddress|  LoopBack or Calibrator is different from normal mode
*--------------------+-----------------------------------------------------------------
***********************************************************************************/
UChar CAN_SendOn( UChar cs, UChar useTxQueue, UChar useSpecificAddress )
{
    UInt SID_Value , SIDH_Value, SIDL_Value = 0;
    UChar id = 0;
	
    if( CAN_Mode == MODE_LISTENONLY )
        return 0 ;
	
    if( cs == CAN_CHIP0 )
        id = 0 ;
    else if( cs == CAN_CHIP1 )
        id = 1 ;
    else
        id = 2 ;
	
    if( useTxQueue == 1 )
    {
		// if you are going to send messages from queue, send from Head at first!
        CAN[id].TxQueue.Head++;
        CAN_TxMsg = CAN[id].TxQueue.Msg[ CAN[id].TxQueue.Head ] ;
        CAN[id].TxQueue.Length-- ;
    }
    else
    {
        if( CAN[id].IsFullTxBuff0 && CAN[id].IsFullTxBuff1 && CAN[id].IsFullTxBuff2 )
        {
            //if all TxBuff0,1,2 are full, Queue message for next transmition
            CAN[id].TxQueue.Tail++;
            if( CAN[id].TxQueue.Tail == MAX_QUEUE_SIZE )
            {
//                MCP_RTS( cs, CAN_RTS_TXBuff0 );
	            
	            if (watchdogEnable == WDT_ENABLE)
		            feedWD(deviceHandle);
//                MCP_RTS( cs, CAN_RTS_TXBuff1 );
	            
	            if (watchdogEnable == WDT_ENABLE)
		            feedWD(deviceHandle);
//                MCP_RTS( cs, CAN_RTS_TXBuff2 );
	            
	            if (watchdogEnable == WDT_ENABLE)
		            feedWD(deviceHandle);
				
                printf("%s", errCANTXFull );
	            if (watchdogEnable == WDT_ENABLE)
		            feedWD(deviceHandle);
                return 1;
            }
			
            CAN[id].TxQueue.Msg[ CAN[id].TxQueue.Tail ] = CAN_TxMsg ;
            CAN[id].TxQueue.Length++ ;
            return RET_SUCCESS;
        }
        else
        {
            CAN[id].TxQueue.Head = -1 ;
            CAN[id].TxQueue.Tail = -1 ;
            CAN[id].TxQueue.Length = 0 ;
        }
    }
	
    //Construct the arbitration field 
    if( useSpecificAddress == 0 )
    {
		SIDH_Value = EncodeSID_H( CAN_TxMsg.Command, Card.BPAddress );
        if(Card.Group!=INOUT_GROUP)
			SIDL_Value = EncodeSID_L( Card.Group, CAN_TxMsg.Command );
        else
			SIDL_Value = EncodeSID_L( 0, CAN_TxMsg.Command ); 
    }
    else   // For Loopback Test
    {
		SIDH_Value = EncodeSID_H( CAN_TxMsg.Command, CAN_TxMsg.Address );
		SIDL_Value = EncodeSID_L( CAN_TxMsg.Group, CAN_TxMsg.Command );
    }
	
//    MCP_CS_ENABLE;
	SID_Value = ((((SIDH_Value << 8)&(0xFF)) | (SIDL_Value)) >> 5)&0x7FF;
	sprintf(temp, "%x", SID_Value);
	if (SID_Value < 0x100)
	{
		strncat(buff, &zero, 1);
		strncat(buff, &zero, 1);
		strncat(buff, temp, 1);
	}
	else if (SID_Value<0x10)
	{
		strncat(buff, &zero, 1);
		strncat(buff, temp, 2);
	}
	else
	{
		strncat(buff, temp, 3);
	}
	strncat(buff, &sharp, 1);
	sprintf(data,"%x", CAN_TxMsg.Data[0]);
	if (CAN_TxMsg.Data[0] < 0x10)
	{
		strncat(buff, &zero, 1);
		strncat(buff, data, 1);
	}
	else
	{
		strncat(buff, data, 2);
	}
	
	if (parse_canframe(buff, &frame)) 
	{
		fprintf(stderr, "\nWrong CAN-frame format!\n\n");
		fprintf(stderr, "Try: <can_id>#{R|data}\n");
		fprintf(stderr, "can_id can have 3 (SFF) or 8 (EFF) hex chars\n");
		fprintf(stderr, "data has 0 to 8 hex-values that can (optionally)");
		fprintf(stderr, " be seperated by '.'\n\n");
		fprintf(stderr, "e.g. 5A1#11.2233.44556677.88 / 123#DEADBEEF / ");
		fprintf(stderr, "5AA# /\n     1F334455#1122334455667788 / 123#R ");
		fprintf(stderr, "for remote transmission request.\n\n");
		return 1;
	}
    if( !CAN[id].IsFullTxBuff0 )
    {
//        MCP_REG_Write( cs, TXB0SIDH , SIDH_Value );
//        MCP_REG_Write( cs, TXB0SIDL , SIDL_Value );
//        MCP_REG_Write( cs, TXB0DLC  , CAN_TxMsg.DataLen );
//        MCP_SPI_WriteBuffer( cs, CAN_TXBuffData0, CAN_TxMsg );
		
	    if((nbytes = write(s, &frame, sizeof(frame))) != sizeof(frame)) 
	    {
		    perror("write");
	    }
        CAN[id].IsFullTxBuff0 = 1;
//        MCP_RTS( cs, CAN_RTS_TXBuff0 );
    }
    else if( !CAN[id].IsFullTxBuff1 )
    {
//        MCP_REG_Write( cs, TXB1SIDH , SIDH_Value );
//        MCP_REG_Write( cs, TXB1SIDL , SIDL_Value );
//        MCP_REG_Write( cs, TXB1DLC  , CAN_TxMsg.DataLen );
//        MCP_SPI_WriteBuffer( cs, CAN_TXBuffData1, CAN_TxMsg );
	    if((nbytes = write(s, &frame, sizeof(frame))) != sizeof(frame)) 
	    {
		    perror("write");
	    }
        CAN[id].IsFullTxBuff1 = 1;
//        MCP_RTS( cs, CAN_RTS_TXBuff1 );
    }
    else
    {
//        MCP_REG_Write( cs, TXB2SIDH , SIDH_Value );
//        MCP_REG_Write( cs, TXB2SIDL , SIDL_Value );
//        MCP_REG_Write( cs, TXB2DLC , CAN_TxMsg.DataLen );
//        MCP_SPI_WriteBuffer( cs, CAN_TXBuffData2, CAN_TxMsg );
	    
		if((nbytes = write(s, &frame, sizeof(frame))) != sizeof(frame)) 
	    {
		    perror("write");
	    }
        CAN[id].IsFullTxBuff2 = 1;
//        MCP_RTS( cs, CAN_RTS_TXBuff2 );
    }
	
//    MCP_CS_DISABLE ;
	
    return RET_SUCCESS;
}

/***********************************************************************************
*  Function   : CAN_Init ()
*----------------------------------------------------------------------------------
*  Object     : it initialize CAN variables, buffers and SPI-related registers
*
*  Return     : RET_SUCCESS : successful
*				otherwise :   failed 
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  mode			   |  LoopBack or Normal
*------------------+-----------------------------------------------------------------
***********************************************************************************/
UChar CAN_Init( UChar mode )
{
	
    CAN_Mode = mode;
	
    CAN_ExpectedReceiveCount = 0;
    UnwantedPacketReceived 	 = 0;
	
    for( int id = 0; id < MAX_NUM_CONTROLLERS; id++ )
    {
        CAN[id].Mode = mode ;
        CAN[id].IsFullTxBuff0  = 0;
        CAN[id].IsFullTxBuff1  = 0;
        CAN[id].IsFullTxBuff2  = 0;
		
		CAN[id].TxQueue.Length = 0 ;
        CAN[id].TxQueue.Tail   = -1;
        CAN[id].TxQueue.Head   = -1;
    }
	
	//	Head and Tail will be incremented before use, so they should be initiated by -1
    RxQueue.Head   = -1 ;
    RxQueue.Tail   = -1 ;
    RxQueue.Length = 0 ;
	
    // Initialization of the SPI
//    MCP_SPI_Init();
	
    if( MCP_Init( CAN_CHIP0, mode ) != 0 )
        return 1;
    if( MCP_Init( CAN_CHIP1, mode ) != 0 )
        return 2;	
    
    //printf_P("Salam");
    if(Card.HotRedundancyActive)
    {
	    //printf_P("S2");
	    //     if( MCP_Init( CAN_CHIP2, mode ) != 0 )
	    //     {
	    //       //printf_P("S3");
	    //        return 3;
	    //        
	    //     }
	    printf(FGRED);
		printf("HotRedundancyActiveError!!");
	    printf(ATTRESET);
    }
	
#ifdef HART_ENABLED
	
//  NOTE-------------NOTE--------------NOTE: deprecated	
/*    if( Card.LCPCWarning.Bit.ExternalRAM == ExternalRAMisOK )
    {
        printf_P( msgTransportInit);
		
		// Initializes the Block Transfer Protocol's variables
		timer0_init();  // Timer for Transport Layer
        HARTInit();     
    }
    else
        printf_P(msgTransportNOTInit);
*/
		printf_P( msgTransportInit);
		timer0_init();  // Timer for Transport Layer
        HARTInit();     
		
#endif
	
    return RET_SUCCESS ;
}

/***********************************************************************************
*  Function   : CAN_SendHeartBeat ()
*----------------------------------------------------------------------------------
*  Object     : it sends a hot redundant msg to CAN interface.
*
*  Return     : RET_SUCCESS : successful
*				 otherwise :   failed
*
*  Note(s)    : This function at first wait for sending the last message, if after TxBuffFlagTimeout ms the message
*               had not been sent, will do the request to send. If latest message is sent successfuly,
*               the data registers and command message are written and the TXREQ bit is set to request
*               the IC to send the mesage.
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  cs			   |  CAN_CHIP0-2 =>MCP2515 ChipSelect pins in uController
*------------------+-----------------------------------------------------------------
***********************************************************************************/
UChar CAN_SendHeartBeat(UChar cs)
{
    CAN_TxMsg.Command   		= FCP_CMD_HOTREDUNDANCY;
    CAN_TxMsg.DataLen   		= 1;
    CAN_TxMsg.Data[0]   		= Card.State;
    CAN_TxMsg.Address   		= Setting.Bit.Generic.PairCardAddress;
    if(Card.Group!=INOUT_GROUP)
     CAN_TxMsg.Group     		= Card.Group;
    else
     CAN_TxMsg.Group     		= 0; 
    Card.CAN_TX_Timeout_Timer[2]= 0;
	
    return CAN_SendOn( cs, 0 , 1 ) ;    
}

/***********************************************************************************
*  Function   : IsCANBusAvailabe ()
*----------------------------------------------------------------------------------
*  Object     : it checks if CAN bus is available or not, firstly it sends a hot
*			    redundant msg to CAN interface(CAN_SendHeartBeat), then waits for
*               msg to transmit, if after TxBuffFlagTimeout ms the message
*               had not been sent yet, would request to send(RTS). If message
*               is sent successfuly, data registers and command message are written
*				and the TXREQ bit is set to request the IC to send the message.
*
*  Return     : RET_SUCCESS : successful
*				otherwise :   failed 
*
*------------------------------------------------------------------------------------
*  Input Arguments |  Description
*------------------+-----------------------------------------------------------------
*  cs			   |  CAN_CHIP0-2 =>MCP2515 ChipSelect pins in uController
*------------------+-----------------------------------------------------------------
***********************************************************************************/
UChar IsCANBusAvailabe( UChar cs )
{
    UChar CANErorr;
    UChar id = 0;
	
//    if( cs == CAN_CHIP0 )
//        id = 0 ;
//    else if( cs == CAN_CHIP1 )
//        id = 1 ;
//    else
//        id = 2 ;
//	
//    CANErorr = Card.LCPCError.D8 & ( 1 << id );
//
//	//	Check if it was succesfull to send heart beat or not
//    if ( CAN_SendHeartBeat(cs) !=  RET_SUCCESS )
//    {
//        printf_P(errCANnotAvailable, id+1);
//        Card.LCPCError.D8 |= ( 1 << id ) ;
//        return 1;
//    }
//    else
//    {
//        printf_P(msgWaitingACKonCAN, id+1, Card.CAN_TX_Timeout_Timer[2]);
//        while ( ( Card.CAN_TX_Timeout_Timer[2]  < CAN3_TIMEOUT ) && ( CAN[id].TXDONE == false ) && ( CANErorr == 0 ) )
//        {
//            CANErorr = Card.LCPCError.D8 & ( 1 << id );
//			
//			// FOR DEBUG ONLY: to print out CAN ERROR register 
//			// printf_P("\r CANErorr = %x  \n", CANErorr);
//            __delay_cycles(800);
//            __watchdog_reset();
//        };
//		
//		
//        CANErorr = Card.LCPCError.D8 & ( 1 << id );
//        if ( (Card.CAN_TX_Timeout_Timer[2]  >= CAN3_TIMEOUT) ||  ( CANErorr != 0) )
//        {
//            printf_P( msgCANErrTimeOut, Card.CAN_TX_Timeout_Timer[2], CANErorr);
//            return 2;
//        }
//        else 
//        {
//            printf_P(msgHearBeatSent);
//            return RET_SUCCESS;
//        }
//    }
	
}




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
void MCP_SoftReset(UChar cs)
{
	//    MCP_CS_ENABLE;
	//	
	//    MCP_SPI_Write( CAN_CMD_RESET );
	//	
	//    MCP_CS_DISABLE;
	
	switch(cs)
	{
		case CAN_CHIP0:		
			system("sudo ip link set can0 down");
			perror("can0 down");
			system("sudo ip link set can0 up");
			perror("can0 up");
			system("sudo ifconfig can0 txqueuelen 1000");
			perror("set can0 txqueuelen");
		
			break;
		case CAN_CHIP1:
			system("sudo ip link set can1 down");
			perror("can1 down");
			system("sudo ip link set can1 up");
			perror("can1 up");
			system("sudo ifconfig can0 txqueuelen 1000");
			perror("set can1 txqueuelen");
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
UChar MCP_Init(UChar cs, UChar mode)
{
	// At first, disable appropriate CAN interrupt
	switch(cs)
	{
	case CAN_CHIP0:	
		strcpy(ifr.ifr_name, "can0");
//		MCP_CS0_INTERRUPT_DISABLE;
		break;
	case CAN_CHIP1:
		strcpy(ifr.ifr_name, "can1");
//		MCP_CS1_INTERRUPT_DISABLE;
		break;
//	case CAN_CHIP2:
//		MCP_CS2_INTERRUPT_DISABLE;
//		break;
	}
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) 
	{
		perror("Socket");
	}
	// FOR DEBUG ONLY: to check out backplane address
    // printf_P( "\r Card.BPAddress=0x%X\n", Card.BPAddress ) ;
	
	// reset requested CAN ip by software
//    MCP_SoftReset(cs);										//reset MCP2515 isn't possible in SocketCAN
	// Check CANCTRL Register due to its initial value!   
	// MODE_CONFIG | MCP_CANCTRL_CLKEN_S = 0x84, but MODE_MASK is 0xE0
	// then 0x04 to enable clock out is useless!!!!
//    MCP_REG_BitModify(cs, CANCTRL, MODE_CONFIG | MCP_CANCTRL_CLKEN_S, MODE_MASK);          //config
	
	ioctl(s, SIOCGIFINDEX, &ifr);
	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

//	if ((MCP_REG_Read(cs, CANCTRL) & MODE_MASK) != MODE_CONFIG)									//if system was not in config(error)
		
	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) 	
	{
		// Check the previous value
        if(cs == CAN_CHIP0)
		{
			perror("Bind");
			Card.LCPCError.Bit.Link1 = 1;       // Set CAN0 link error
		}
		else if(cs == CAN_CHIP1)
		{
			perror("Bind");
			Card.LCPCError.Bit.Link2 = 1;       // Set CAN1 link error
		}
//        else if(cs == CAN_CHIP2)
//            Card.LCPCError.Bit.Link3 = 1;      // Set CAN2 link error
		
//        MCP_CS_DISABLE ;
        MCP_INTERRUPT_ENABLE;
		return 2;
	}
	
//	// Synchronization Jump With (SJW) ==> 0 ==>1 TQ
//	// Baud Rate Prescaler (BRP) = 0   ==> TQ = 2 x (BRP + 1)/FOSC ==> TQ = 2 T_OSL           
//	MCP_REG_Write(cs, CNF1, CNF1_SJW | CNF1_BRP);      // 0x00												  //timing
	// The SAM bit controls how many times RXCAN pin is sampled: SAM = 1 ==> 3times
	// The BTLMODE bit controls how the length of PS2 is determined.
	//     BTLMODE = 1 the length of PS2 is determined by PHSEG2<2:0> bits of CNF3
	// The PRSEG<2:0> bits set length of propagation segment ==> 4 TQ
    // The PHSEG1<2:0> bits set length of PS1 ==> PS1 Length =(PHSEG1 + 1)*TQ = 3 TQ	
//	MCP_REG_Write(cs, CNF2, MCP_CNF2_SAM_S | MCP_CNF2_BTLMODE_S | (MCP_CNF2_PRSEG1_S | MCP_CNF2_PRSEG0_S) | MCP_CNF2_PHSEG11_S);     // 0xD3			//timing
    // The PHSEG2<2:0> bits set length in TQ’s of PS2 ==> 4 TQ
//	MCP_REG_Write(cs, CNF3, MCP_CNF3_PHSEG21_S | MCP_CNF3_PHSEG20_S);      // 0x03																		//timing
	//  //1TQ, 3TQ, 4TQ, 4TQ
	//	MCP_REG_Write( cs, CNF1, CNF1_SJW | CNF1_BRP );
	//	MCP_REG_Write( cs, CNF2, MCP_CNF2_SAM_S | MCP_CNF2_BTLMODE_S | ( MCP_CNF2_PRSEG1_S ) | MCP_CNF2_PHSEG11_S | MCP_CNF2_PHSEG10_S );
	//	MCP_REG_Write( cs, CNF3, MCP_CNF3_PHSEG21_S | MCP_CNF3_PHSEG20_S );
	
/*********************************************************************************************/
//	MCP_REG_Write(cs, BFPCTRL, BFPCTRL_Default);				//enable rx interrupt pins
//	MCP_REG_Write(cs, TXRTSCTRL, TXRTSCTRL_Default);			//enable tx interrupt pins
	
/********************************************************************************************/
	if (mode != MODE_LISTENONLY)
	{
      
//		MCP_REG_Write(cs, RXB0CTRL, REC_ONLY_VALID_STD | MCP_RXB0CTRL_BUKT_S);				//???
//		MCP_REG_Write(cs, RXB1CTRL, REC_ONLY_VALID_STD);									//???
//		
//		// MASK setting M0        
//		MCP_REG_Write(cs, RXM0SIDH, 0xFC);                      //set SIDH mask 1111 1100
//		if (Card.Group != INOUT_GROUP)
//			MCP_REG_Write(cs, RXM0SIDL, 0x20);					//set SIDL mask 0010 0000			
//		else
//			MCP_REG_Write(cs, RXM0SIDL, 0x00);					//set SIDL mask 0000 0000
//        
//		MCP_REG_Write(cs, RXM0EID8, 0x00);						//set EXTENDED IDENTIFIER REGISTER HIGH
//		MCP_REG_Write(cs, RXM0EID0, 0x00);						//set EXTENDED IDENTIFIER REGISTER LOW
//		// MASK setting M1
//		MCP_REG_Write(cs, RXM1SIDH, 0xFC);						//set SIDH mask 1111 1100
//		if (Card.Group != INOUT_GROUP)	
//			MCP_REG_Write(cs, RXM1SIDL, 0x20);					//set SIDL mask 0010 0000
//		else
//			MCP_REG_Write(cs, RXM1SIDL, 0x00);					//set SIDL mask 0000 0000
//        
//		MCP_REG_Write(cs, RXM1EID8, 0x00);						//set mask EXTENDED IDENTIFIER REGISTER HIGH
//		MCP_REG_Write(cs, RXM1EID0, 0x00);						//set mask EXTENDED IDENTIFIER REGISTER LOW
		
		if(Card.Group != INOUT_GROUP)
			rfilter[0].can_mask = ((0xFC00)|(0x20))>>5;
		else
			rfilter[0].can_mask = ((0xFC00)|(0x00))>>5;
		
		if (Card.Group != INOUT_GROUP)
			rfilter[1].can_mask = ((0xFC00)|(0x20))>>5;
		else
			rfilter[1].can_mask = ((0xFC00)|(0x00))>>5;
        
		if (Card.Group != INOUT_GROUP)
		{
//			// Filter 0
//			MCP_REG_Write(cs, RXF0SIDH, 0);	                     //Standard Identifier Filter bits
//			MCP_REG_Write(cs, RXF0SIDL, (Card.Group << 5));		 //
//			MCP_REG_Write(cs, RXF0EID8, 0);	
//			MCP_REG_Write(cs, RXF0EID0, 0);	
//			// Filter 1
//			MCP_REG_Write(cs, RXF1SIDH, (Card.BPAddress << 2));
//			MCP_REG_Write(cs, RXF1SIDL, (Card.Group << 5));	
//			MCP_REG_Write(cs, RXF1EID8, 0);	
//			MCP_REG_Write(cs, RXF1EID0, 0);	
//			// Filter 2
//			MCP_REG_Write(cs, RXF2SIDH, 0);	
//			MCP_REG_Write(cs, RXF2SIDL, (Card.Group << 5));	
//			MCP_REG_Write(cs, RXF2EID8, 0);	
//			MCP_REG_Write(cs, RXF2EID0, 0);	
//			// Filter 3
//			MCP_REG_Write(cs, RXF3SIDH, (Card.BPAddress << 2));	
//			MCP_REG_Write(cs, RXF3SIDL, (Card.Group << 5));
//			MCP_REG_Write(cs, RXF3EID8, 0);	
//			MCP_REG_Write(cs, RXF3EID0, 0);	
//			// Filter 4
//			MCP_REG_Write(cs, RXF4SIDH, 0);	
//			MCP_REG_Write(cs, RXF4SIDL, (Card.Group << 5));	
//			MCP_REG_Write(cs, RXF4EID8, 0);	
//			MCP_REG_Write(cs, RXF4EID0, 0);	
//			// Filter 5
//			MCP_REG_Write(cs, RXF5SIDH, (Card.BPAddress << 2));	
//			MCP_REG_Write(cs, RXF5SIDL, (Card.Group << 5));
//			MCP_REG_Write(cs, RXF5EID8, 0);	
//			MCP_REG_Write(cs, RXF5EID0, 0);	
			
			//set filter 1&2
			rfilter[0].can_id   = ((0x0000)&(Card.Group << 5))>>5;

			rfilter[1].can_id   = ((((Card.BPAddress << 2) & 0x00FF)<<8)|(Card.Group << 5)) >> 5;

		}
		else
		{
//			// Filter 0
//			MCP_REG_Write(cs, RXF0SIDH, 0);	
//			MCP_REG_Write(cs, RXF0SIDL, 0);
//			MCP_REG_Write(cs, RXF0EID8, 0);	
//			MCP_REG_Write(cs, RXF0EID0, 0);	
//			// Filter 1
//			MCP_REG_Write(cs, RXF1SIDH, (Card.BPAddress << 2));
//			MCP_REG_Write(cs, RXF1SIDL, 0);	
//			MCP_REG_Write(cs, RXF1EID8, 0);	
//			MCP_REG_Write(cs, RXF1EID0, 0);	
//			// Filter 2
//			MCP_REG_Write(cs, RXF2SIDH, 0);	
//			MCP_REG_Write(cs, RXF2SIDL, 0);	
//			MCP_REG_Write(cs, RXF2EID8, 0);	
//			MCP_REG_Write(cs, RXF2EID0, 0);	
//			// Filter 3
//			MCP_REG_Write(cs, RXF3SIDH, (Card.BPAddress << 2));	
//			MCP_REG_Write(cs, RXF3SIDL, 0);
//			MCP_REG_Write(cs, RXF3EID8, 0);	
//			MCP_REG_Write(cs, RXF3EID0, 0);	
//			// Filter 4
//			MCP_REG_Write(cs, RXF4SIDH, 0);	
//			MCP_REG_Write(cs, RXF4SIDL, 0);	
//			MCP_REG_Write(cs, RXF4EID8, 0);	
//			MCP_REG_Write(cs, RXF4EID0, 0);	
//			// Filter 5
//			MCP_REG_Write(cs, RXF5SIDH, (Card.BPAddress << 2));	
//			MCP_REG_Write(cs, RXF5SIDL, 0);
//			MCP_REG_Write(cs, RXF5EID8, 0);	
//			MCP_REG_Write(cs, RXF5EID0, 0);	 
			
			//set filter 1&2
			rfilter[0].can_id   = (0x000);

			rfilter[1].can_id   = (((Card.BPAddress << 2) & 0x00FF) << 8) >> 5;
			
		}
	}
	else							//LISTENONLY mode
	{
//		MCP_REG_Write(cs, RXB0CTRL, REC_ALL_VALID | MCP_RXB0CTRL_BUKT_S);
//		MCP_REG_Write(cs, RXB1CTRL, REC_ALL_VALID);
//		
//		// MASK setting M0
//		MCP_REG_Write(cs, RXM0SIDH, 0x00);
//		MCP_REG_Write(cs, RXM0SIDL, 0x00);
//		MCP_REG_Write(cs, RXM0EID8, 0x00);
//		MCP_REG_Write(cs, RXM0EID0, 0x00);
//		// MASK setting M1
//		MCP_REG_Write(cs, RXM1SIDH, 0x00);	
//		MCP_REG_Write(cs, RXM1SIDL, 0x00);	
//		MCP_REG_Write(cs, RXM1EID8, 0x00);
//		MCP_REG_Write(cs, RXM1EID0, 0x00);
		
		//disable filter and mask in socketcan
		setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

	}
	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
//	MCP_REG_Write(cs, TXB0CTRL, TXB0PRIORITY);        // 0x03		//send priority
//	MCP_REG_Write(cs, TXB1CTRL, TXB1PRIORITY);        // 0x02
//	MCP_REG_Write(cs, TXB2CTRL, TXB2PRIORITY);        // 0x01
	
//	MCP_REG_Write(cs, CANINTE, MCP_CANINTE_TX0IE_S | MCP_CANINTE_TX1IE_S | MCP_CANINTE_TX2IE_S | MCP_CANINTE_RX1IE_S | MCP_CANINTE_RX0IE_S);   //0x1F   enable interrupts 0x1F                	
	
	//========= NOTE : The IOCard's CAN should not work in One Shot Mode ===============
	//    if(cs == CAN_Chip2)
	//        MCP_REG_BitModify( cs, CANCTRL, mode | MCP_CANCTRL_CLKEN_S | MCP_CANCTRL_OSM_S, MODE_MASK | MCP_CANCTRL_OSM_M );
	//    else
//	MCP_REG_BitModify(cs, CANCTRL, mode | MCP_CANCTRL_CLKEN_S, MODE_MASK);    //CAN CONTROL REGISTER 
	
	// Enable appropriate CAN interrupt
	switch(cs)
	{
	case CAN_CHIP0:		
		if (mode != MODE_LOOPBACK)
		{
			CAN0_LOOPBACK_DISABLE;
		}
		else
		{
			loopBackSetting();
		}
		MCP_CS0_INTERRUPT_ENABLE;
		break;
	case CAN_CHIP1:
		if (mode != MODE_LOOPBACK)
		{
			CAN1_LOOPBACK_DISABLE;
		}
		else
		{
			loopBackSetting();
		}
		MCP_CS1_INTERRUPT_ENABLE;
		break;
//	case CAN_CHIP2:
//		MCP_CS2_INTERRUPT_ENABLE;
//		if (mode != MODE_LOOPBACK)
//			CAN2_LOOPBACK_DISABLE;
//		break;
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
	
	    ULong ResponseWaite = 0;
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
UChar MCP_REG_Read(UChar cs, UChar address)
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
void MCP_SPI_Write(UChar data)
{
//	    MCP_SPI_DATA = data;
	    ULong ResponseWaite=0;

	    //while( !( MCP_SPI_FLAG_CHECK ));// SPI interrupt flag check
	    
	   
	    //	Set the SPI data
	    
//	    while( !( MCP_SPI_FLAG_CHECK ))//&&(ResponseWaite<100000))
//	    {
//	      		//__watchdog_reset();
//			//__delay_cycles(1);
//	                ResponseWaite++;
//	    }
	    
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
void MCP_REG_Write(UChar cs, UChar address, UChar data)
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
void MCP_REG_BitModify(UChar cs, UChar address, UChar data, UChar mask)
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
void MCP_SPI_WriteBuffer(UChar cs, UChar txRegister, tCANMsg msg)
{
//	MCP_CS_ENABLE;
		
	//  Putting the MCP's TX buffer 0 in writing mode
	MCP_SPI_Write( CAN_CMD_LoadTXBuffer | txRegister );
		
	//  Write the data register
	for( UChar i = 0; i < msg.DataLen; i++ )
	    MCP_SPI_Write( msg.Data[i] );
		
//	MCP_CS_DISABLE;
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
void MCP_RTS( UChar cs, UChar txRegister )
{
    //MCP_Interrupt_Disable;
//    MCP_CS_ENABLE;
	
    MCP_SPI_Write( CAN_CMD_RTS | txRegister );
	
//    MCP_CS_DISABLE;
    //MCP_Interrupt_Enable;
}



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
UChar MCP_ReadStatus(UChar cs)
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
UChar MCP_ReadRxStatus(UChar cs)
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

// ------------------------ BSW functions --------------------------//
int canInit(const char* can_iface_name)
{
	int s; /* can raw socket */ 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct timeval tv;
	
	//close(IOCAN_Socket);
	//close(CPUCAN_Socket);
	
	/* open socket */
	if((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket");
		printf("Open Socket Error!\n");
		return -1;
	}
	
	// Resolve the iface index
	(void)memset(&ifr, 0, sizeof(ifr));
	(void)strncpy(ifr.ifr_name, can_iface_name, IFNAMSIZ);

	if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
		perror("SIOCGIFINDEX");
		printf("Resolving iface index error!\n");
		return -1;
	}
	
	// Assign the iface
	(void)memset(&addr, 0, sizeof(addr));
	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	/* disable default receive filter on this RAW socket */
	/* This is obsolete as we do not read from the socket at all, but for */
	/* this reason we can remove the receive list in the Kernel to save a */
	/* little (really a very little!) CPU usage.                          */
	//setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

tv.tv_sec = 0;
	tv.tv_usec = 50000;  // 50ms
	//setsockopt(s, SOL_CAN_RAW, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		printf("binding Socket Error!\n");
		return -1;
	}
	
	printf("(can_Init) Interface ""%s"" initiated at index:  %d\n", ifr.ifr_name, addr.can_ifindex);

	return addr.can_ifindex;   // Refers to the initiated socket
}

///////////////////////////////////////////////////////
int can0_init()
{
	int s; /* can raw socket */ 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct timeval tv;
	
	/* open socket */
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket");
		printf("Open IO Socket Error!\n");
		return -1;
	}
	
	// Resolve the iface index
	(void)memset(&ifr, 0, sizeof(ifr));
	(void)strncpy(ifr.ifr_name, "can0", IFNAMSIZ);

	if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
		perror("SIOCGIFINDEX");
		printf("Resolving iface index error!\n");
		return -1;
	}
	
	// Assign the iface
	(void)memset(&addr, 0, sizeof(addr));
	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	/* disable default receive filter on this RAW socket */
	/* This is obsolete as we do not read from the socket at all, but for */
	/* this reason we can remove the receive list in the Kernel to save a */
	/* little (really a very little!) CPU usage.                          */
	//setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

tv.tv_sec = 0;
	//===== time out =========//
	tv.tv_usec = 50000;  // 50ms
	//setsockopt(s, SOL_CAN_RAW, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		printf("binding Socket Error!\n");
		return -1;
	}
	
	printf(" - Interface ""%s"" initiated with index: %d \r\n", ifr.ifr_name, addr.can_ifindex);

	return s;   // Refers to the initiated socke
}

int can1_init()
{
	int s; /* can raw socket */ 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct timeval tv;
	
	/* open socket */
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket");
		printf("Open CPU Socket Error!\n");
		return -1;
	}
	
	// Resolve the iface index
	(void)memset(&ifr, 0, sizeof(ifr));
	(void)strncpy(ifr.ifr_name, "can1", IFNAMSIZ);

	if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
		perror("SIOCGIFINDEX");
		printf("Resolving iface index error!\n");
		return -1;
	}
	
	// Assign the iface
	(void)memset(&addr, 0, sizeof(addr));
	addr.can_family  = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	/* disable default receive filter on this RAW socket */
	/* This is obsolete as we do not read from the socket at all, but for */
	/* this reason we can remove the receive list in the Kernel to save a */
	/* little (really a very little!) CPU usage.                          */
	//setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

tv.tv_sec = 0;
	tv.tv_usec = 500000;  // 500ms
	//setsockopt(s, SOL_CAN_RAW, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		printf("binding Socket Error!\n");
		return -1;
	}
	
	printf(" - Interface ""%s"" initiated with index: %d\n", ifr.ifr_name, addr.can_ifindex);

	return s;   // Refers to the initiated socke
}

/*struct timeval timeStamp(int CANSocket)
{
	struct timeval tv;
	ioctl(CANSocket, SIOCGSTAMP, &tv);
	return tv
}*/

/*int sendto_can0(int CANSocket, struct can_frame cf)
{
	struct ifreq ifr;
	struct sockaddr_can addr;
	int nbytes;

	strcpy(ifr.ifr_name, "can0");
	ioctl(CANSocket, SIOCGIFINDEX, &ifr);
    addr.can_ifindex = ifr.ifr_ifindex;
    addr.can_family  = AF_CAN;

	nbytes = sendto(CANSocket, &cf, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, sizeof(addr));
}

int sendto_can1(int CANSocket, struct can_frame cf)
{
	struct ifreq ifr;
	struct sockaddr_can addr;
	int nbytes;
	strcpy(ifr.ifr_name, "can1");
	ioctl(CANSocket, SIOCGIFINDEX, &ifr);
    addr.can_ifindex = ifr.ifr_ifindex;
    addr.can_family  = AF_CAN;

	nbytes = sendto(CANSocket, &cf, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, sizeof(addr));
}*/

int sendFrame(int CANSocket, int canID, unsigned char dLen, unsigned char canData[])  // send can packet to the can0 device
{
	struct can_frame cf;
	//struct sockaddr_can addr;

	cf.can_id = canID;
	cf.can_dlc = dLen;

	for (int i = 0; i < dLen; i++)
	{
		cf.data[i] = canData[i];
	}

	// new form
	/*addr.can_ifindex = CANSocket;
	addr.can_family  = AF_CAN;

	nbytes = sendto(CANSocket, &cf, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, sizeof(addr));
	if (nbytes != sizeof(cf)) {
		printf(" CAN Write Error.\n");
		return ERROR;
	}*/
	///

	if((write(CANSocket, &cf, sizeof(cf))) != sizeof(cf)) {
		printf(" CAN Write Error.\n");
		return ERROR;
	}
	
	return SUCCESS;
}

int receiveFrame(int CANSocket, struct can_frame *cf)  // receive can packet from can_Socket device
{
	int nbytes;

	nbytes = read(CANSocket, cf, sizeof(struct can_frame));
	//nbytes = recvmsg(CANSocket, cf, 0);

	// new form
	/*struct sockaddr_can addr;
	socklen_t len = sizeof(addr);
	nbytes = recvfrom(CANSocket, cf, sizeof(struct can_frame), 0, (struct sockaddr*)&addr, &len);*/
	//

	if(nbytes < 0) {
		errCnt++;
		//perror("read error");
		printf("%d - CAN read error.\n", errCnt);
		return ERROR;
	}

	//paranoid check
	if(nbytes < sizeof(struct can_frame)) {
		//fprintf(stderr, "read: incomplete CAN frame \n");
		printf("read: incomplete CAN frame. \n");
		return ERROR;
	}

	//int i = 0;
	//printf("can0:  %03X   [%d]", cf->can_id, cf->can_dlc);
	//for (i = 0; i < cf->can_dlc; i++)
	//	printf(" %02X", cf->data[i]);
	//printf("\n");

	return SUCCESS;
}

void printRcvFrame(struct can_frame rcvFrame)  // for debug usage
{
	char c;
	
	printf("%03X [%d] ", rcvFrame.can_id, rcvFrame.can_dlc);
	for (c = 0; c < rcvFrame.can_dlc; c++)
		printf(" %02X", rcvFrame.data[c]);
	printf("\n");
}

void can0LoopbackEnable(void)
{
	int status = system("sudo ip link set can0 down");
	perror("can0 down");
	status = system("sudo ip link set can0 type can loopback on");
	perror("can0 loopback on");
	status = system("sudo ip link set can0 up type can bitrate 1000000");
	perror("can0 set baudrate");
	status = system("sudo ifconfig can0 txqueuelen 1000");
	perror("set txqueuelen");

}
void can0LoopbackDisable(void)
{
	int status = system("sudo ip link set can0 down");
	perror("can0 down");
	status = system("sudo ip link set can0 type can loopback off");
	perror("can0 loopback off");
	status = system("sudo ip link set can0 up type can bitrate 1000000");
	perror("can0 set baudrate");
	status = system("sudo ifconfig can0 txqueuelen 1000");
	perror("set txqueuelen");
}
void can1LoopbackEnable(void)
{
	int status = system("sudo ip link set can1 down");
	perror("can1 down");
	status = system("sudo ip link set can1 type can loopback on");
	perror("can1 loopback on");
	status = system("sudo ip link set can1 up type can bitrate 1000000");
	perror("can1 set baudrate");
	status = system("sudo ifconfig can1 txqueuelen 1000");
	perror("set txqueuelen");

}
void can1LoopbackDisable(void)
{
	int status = system("sudo ip link set can1 down");
	perror("can1 down");
	status = system("sudo ip link set can1 type can loopback off");
	perror("can1 loopback off");
	status = system("sudo ip link set can1 up type can bitrate 1000000");
	perror("can1 set baudrate");
	status = system("sudo ifconfig can1 txqueuelen 1000");
	perror("set txqueuelen");
}

void loopBackSetting(void)
{
	int loopback = 1; /* 0 = disabled, 1 = enabled (default) */

	setsockopt(s, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback));
	
	int recv_own_msgs = 1; /* 0 = disabled (default), 1 = enabled */

	setsockopt(s, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &recv_own_msgs, sizeof(recv_own_msgs));
	
}
//***********************************************************************************
// No More
//***********************************************************************************


