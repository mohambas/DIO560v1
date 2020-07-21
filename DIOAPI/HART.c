/***********************************************************************************
*  File      : HART.C
*----------------------------------------------------------------------------------
*  Object    : it includes required functions for Initializing and handling HART
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

#ifdef 	  HART_ENABLED

/***********************************************************************************
*  Specific Header files for HART enabled-IOCards
***********************************************************************************/
#include "Common.h"
#include "HART.h"

/***********************************************************************************
*  Local variables
***********************************************************************************/
bool	ReqBuff[8][3];

bool	isAckRcvd;
bool	isDataDetected;

UChar 	PollAddr   = 0;     // it is used in HART_RCV_ERROR
UChar 	RetryCount = 0;     // it is used in HART_RCV_ERROR
UChar	RetryLimit = 3;	    // RetryLimit for RetryCount

UChar 	pream_L = 5;        // FrameFormation arguments
UChar 	*Addr;              // FrameFormation arguments

// it is defined in CAN.c, and now we define it externally here, because CAN.c is a common file
// for all IOCards, but HART.c only used in two of them!
extern  UChar	Command;    // FrameFormation arguments 

UChar 	SendData[40];       // FrameFormation arguments
UChar 	ByteCount;          // FrameFormation arguments (Byte Count 0-255)
bool 	AddrType;           // FrameFormation arguments (Polling =0/ Unique =1)
bool 	PrimaryMaster = 1;  // if it is Primary =1 or Secondary Master =0
UChar 	*FirstSendAddrByte; // pointer to the first address UChar
Int 	FrameLenght;        // made in FrameFormation and used in Send_DATA_UART()
UChar 	SendFrame[50];      // made in FrameFormation and used in Send_DATA_UART()

UInt	data_byte;          // a variable used in DataStorage()
UChar	Channel = 0;        // a variable used in DataStorage()

UChar HARTState = HART_IDLE;// HART State in HART State Machine
UChar HART_Error;           // to save Errors in HART State Machine
UInt  RepeatTimer = 0;

tHARTMSG HART_RxMsg;

tUARTBuffer UART_TX_Buffer;
tUARTBuffer UART_RX_Buffer;

tDataBuffer HART_CAN_TX_Buffer;
tDataBuffer HART_CAN_RX_Buffer;

struct HART_data HARTData[8];
struct HARTError HARTErrorIndicator[8];
struct ReqDatas  AssetMngReq[8];
struct DataBase  Device[8];
struct HARTError PreviousHARTError[8];

__farflash Char errHARTRetryLimitReached[]	= "\r ERROR: Retry Limit Reached\n";
__farflash Char msgAllSegmentsUploaded[]	= "\r INFO: All Segments Uploaded successfully \n";
__farflash Char msgHARTInitDone[]			= "\r INFO: HART INIT Done. \n";

/***********************************************************************************
*  Local Definitions
***********************************************************************************/
#define HART_SLAVE_RESPONSE_TIMEOUT 208   // 208ms: 28 character Length = 28* 1000*9/1200
#define HART_RESPONSE_TIMEOUT  		3000  // ms
#define HART_GAP_TIMEOUT       		100   // ms  (1 character length)

#define HART_SLAVE_TIMEOUT_ERROR    1
#define HART_RESPONSE_TIMEOUT_ERROR 2
#define HART_GAP_TIMEOUT_ERROR      3

/***********************************************************************************
*  Function   : HART_MainStateMachine ()
*----------------------------------------------------------------------------------
*  Object     : Call this function to run HART State Machine.
*
*  Return     : None
*
***********************************************************************************/
void HART_MainStateMachine( void )
{
    UChar i = 0,j = 0;
	
#if( CARD_TYPE == IOTYPE_AI623 )
    if( IOCard.IsConversionDone == false )
        return;
#endif
	
    switch( HARTState )
    {
	  case HART_IDLE:
#ifdef HART_TEST
		if ( RepeatTimer >= 10000 )    //for Test
		{
			ReqBuff[0][0] = true;
			if( Device[0].fill == true )
				AssetMngReq[0].cmd = 3;
			else
				AssetMngReq[0].cmd = 0;
			
			AssetMngReq[0].DataLength=0;
		}
#endif
		for ( i = 0; i < 8; i++ )
		{
			if( ReqBuff[i][0] == true )
			{
				ReqBuff[i][0] = false;
				Command   = AssetMngReq[i].cmd;
				ByteCount = AssetMngReq[i].DataLength;
				memcpy(SendData, AssetMngReq[i].Data, ByteCount);
				if (Command != 0)
					LongAddr(i);
				else
					ShortAddr(i);
				
				SwitchToChannel(i);
				Channel   = i;
				HARTState = TRANSMIT_HART_COMMAND;
				if( Command == 0 )
				{
					for (j = 0; j < 5;  j++)
						Device[i].LongAddr[j]=0;
					
					for (j = 0; j < 20; j++)
						Device[i].cmd0[j]=0;
					
					for (j = 0; j < 35; j++)
						Device[i].LongTAG[j]=0;
				}
				i=8;
			}
		}
        break;
		
	  case HART_RCV_ERROR:
		if( HART_Error == HART_SLAVE_TIMEOUT_ERROR )
		{
			RetryCount++;
			if ( RetryCount < RetryLimit)
			{
				HARTState = TRANSMIT_HART_COMMAND;
			}
			else
			{
				printf_P(errHARTRetryLimitReached);
				RetryCount  = 0;
				if( Command == 0 )
				{
					PollAddr++;
					if (PollAddr>16)
					{
						HARTState = HART_IDLE;
						PollAddr  = 0;
						Device[i].ShortAddr=0;
					}
					else
					{
						Device[i].ShortAddr = PollAddr;
						ShortAddr(i);
						HARTState = TRANSMIT_HART_COMMAND;
					}
				}
				else
					HARTState = HART_IDLE;
			}
		}
		else
			HARTState = HART_IDLE;
        break;
		
	  case TRANSMIT_HART_COMMAND:
		FrameFormation(pream_L, Addr, Command, SendData, ByteCount, AddrType);
		__delay_cycles(8000);
		
#ifdef DEBUG_HART
                UChar  TX;
		printf_P("\r\nTX=[ ");
		for( i = 0 ; i < FrameLenght ; i++ )
		{
			TX = *( SendFrame + i);
			printf_P("%d ", TX);
		}
		printf_P(" ]");
#endif
		UART_RX_Buffer.pointer = 0;
		Send_Data_UART( SendFrame, FrameLenght );
		isDataDetected	= false;
		isAckRcvd		= false;
		Card.UART3_RX_Timer = 0;
		HARTState = WAITING_FOR_RESPONSE;
        break;
		
	  case WAITING_FOR_RESPONSE:
		if ( Card.UART3_RX_Timer > HART_SLAVE_RESPONSE_TIMEOUT )
		{
			HART_Error = HART_SLAVE_TIMEOUT_ERROR;
			HARTState  = HART_RCV_ERROR;
			
#ifdef DEBUG_HART
			printf_P("\r\n HART Error: STO");
#endif
			RepeatTimer=0;
		}
		if ( isDataDetected && (Card.UART3_RX_Timer > HART_GAP_TIMEOUT) )
		{
			isDataDetected	= false;
			HARTState 		= HART_RCV_ERROR;
			HART_Error 		= HART_GAP_TIMEOUT_ERROR;
			Card.UART3_RX_Timer = 0;
#ifdef DEBUG_HART
			printf_P("\r\n HART Error: GTO");
#endif
			RepeatTimer=0;
		}
        break;
		
	  case RECEIVING_HART_PACKET:
		if ( Card.UART3_RX_Timer > HART_RESPONSE_TIMEOUT )
		{
			HART_Error = HART_RESPONSE_TIMEOUT_ERROR;
			HARTState  = HART_RCV_ERROR;
			Card.UART3_RX_Timer=0;
#ifdef DEBUG_HART
			printf_P("\r\n HART Error: RTO");
#endif
			RepeatTimer=0;
		}
		if( isAckRcvd )
		{
			isAckRcvd = false;
			
			// FOR DEBUG ONLY: to check out UART RX Buffer in HART
			//printf_P("\r\nUART-RX=[ ");
			//for ( i = 0 ; i < UART_RX_Buffer.pointer ; i++ )
			//    printf_P("%d ", UART_RX_Buffer.Byte[i]);
			//printf_P(" ]");
			if( Decode_HART_Message() == 0 )
				DataStorage();
			
			HARTState = HART_IDLE;
			RepeatTimer = 0;
		}
        break;
	}
}

/***********************************************************************************
*  Function   : SendBlock
*----------------------------------------------------------------------------------
*  Object     : To send HART Blocks by CAN controller to MPU
*
*  Return     : 0 : default
*				
*----------------------------------------------------------------------------------
*  Variable Name     |IN |OUT|GLB|  Utilization
*--------------------+---+---+---+-------------------------------------------------
*  ByteCount         | X |   |   |
*--------------------+---+---+---+-------------------------------------------------
***********************************************************************************/
UChar SendBlock( UChar ByteCount )
{
    UChar segmentSize = 8;
	
    UChar DataSize, numOfSegments, lastSegmentLen;
    UChar Counter = 0;
	
    DataSize       = ByteCount;
    numOfSegments  = DataSize / segmentSize ;
	lastSegmentLen = DataSize - ( numOfSegments * segmentSize );
	
    CAN_TxMsg.Command = FCP_CMD_UPLOAD_DATA;
	
    if( numOfSegments == 0 )
    {
        Counter = HART_CAN_TX_Buffer.Pointer;
        for( UChar i = 0 ; i < DataSize; i++)
            CAN_TxMsg.Data[i] = HART_CAN_TX_Buffer.Byte[i + Counter];
		
        CAN_TxMsg.DataLen = DataSize;
        CAN_Send();
        __watchdog_reset();
		
        lastSegmentLen = 0 ;
        HART_CAN_TX_Buffer.Pointer += DataSize;
    }
    else
    {
        for( UChar j = 0 ; j < numOfSegments; j++)
        {
            Counter = HART_CAN_TX_Buffer.Pointer;
            for( UChar i = 0 ; i < segmentSize; i++)
            {
                CAN_TxMsg.Data[i] = HART_CAN_TX_Buffer.Byte[i + Counter];
                __watchdog_reset();
            }
			
            CAN_TxMsg.DataLen = segmentSize;
            CAN_Send();
            HART_CAN_TX_Buffer.Pointer += segmentSize;
            __watchdog_reset();
        }
		
        if( lastSegmentLen != 0 )
        {
            Counter = HART_CAN_TX_Buffer.Pointer;
            for( UChar i = 0 ; i < lastSegmentLen; i++)
            {
                CAN_TxMsg.Data[i] = HART_CAN_TX_Buffer.Byte[i + Counter];
                __watchdog_reset();
            }
            CAN_TxMsg.DataLen = lastSegmentLen;
            CAN_Send();
            HART_CAN_TX_Buffer.Pointer += lastSegmentLen;
            __watchdog_reset();
        }
    }
	
    if( HART_CAN_TX_Buffer.ByteCount == (HART_CAN_TX_Buffer.Pointer) )
    {
        HART_CAN_TX_Buffer.Pointer         = 0;
        HART_CAN_TX_Buffer.DataReadyToSend = false;
        printf_P(msgAllSegmentsUploaded);
    }
    else if ( HART_CAN_TX_Buffer.ByteCount < HART_CAN_TX_Buffer.Pointer )
    {
		// FOR DEBUG ONLY: an Error ocurred during upload
		// printf_P("\r Error Ocurred during Data Upload \n");
        //TODO : Make a decision
    }
    __watchdog_reset();
    return 0;
}

/***********************************************************************************
*  Function   : FrameFormation
*----------------------------------------------------------------------------------
*  Object     : To generate frame in HART format
*
*  Return     : None
*				
*----------------------------------------------------------------------------------
*  Variable Name     |IN |OUT|GLB|  Utilization
*--------------------+---+---+---+-------------------------------------------------
*  preamble_length   | X |   |   |
*--------------------+---+---+---+-------------------------------------------------
*  addr              | X |   |   |
*--------------------+---+---+---+-------------------------------------------------
*  cmd               | X |   |   |
*--------------------+---+---+---+-------------------------------------------------
*  c_data            | X |   |   |
*--------------------+---+---+---+-------------------------------------------------
*  c_numb            | X |   |   |
*--------------------+---+---+---+-------------------------------------------------
*  address_type      | X |   |   |
*--------------------+---+---+---+-------------------------------------------------
***********************************************************************************/
void FrameFormation(Int preamble_lenght, UChar *addr, UChar cmd, UChar *c_data, UChar c_numb, bool address_type)
{
	UChar *c_d;
	UChar *a;
	Int j;
	Int i;
	
	// Transmit Preamble
	UChar dataB = 0xff;
	for (i = 0; i < preamble_lenght; i++){ // Data transmit function with UART3
		j = i;
		SendFrame[i] = dataB;
	}
	
	// Transmit Start Character
	UChar Delimiter = 0x00;
	if (address_type)
		Delimiter = (Delimiter | 0x80);
	
	// Master to field device
	Delimiter = (Delimiter | 0x02);
	SendFrame[++j] = Delimiter;
	
	// Transmit Address
	a = addr;
	UChar tFirstAddressByte = 0;
	if (PrimaryMaster)
		tFirstAddressByte = (tFirstAddressByte | 0x80);
	else
		tFirstAddressByte = (tFirstAddressByte & 0x7f);
	
	//(10111111)master devcie should set this bit to "0" in all request to slave
	tFirstAddressByte = (tFirstAddressByte & 0xbf); 			
	if (address_type == 1) 					//Unique addresses && Broadcast addresses
	{	
		SendFrame[++j] = (tFirstAddressByte | ((*a++) & 0x3f));	// 1st address UChar.
		FirstSendAddrByte=&SendFrame[j];	// Pointer to the first address UChar
		for (i = 1; i <= 4; ++i)
			SendFrame[++j] = *a++;			// Next 4 addr bytes.
	}
	else if (address_type == 0)				// Polling addresses
	{
		SendFrame[++j] = (tFirstAddressByte | *a++);
		FirstSendAddrByte = &SendFrame[j];	// Pointer to the first address UChar
	}
	
	// Transmit command
	SendFrame[++j] = cmd;
	
    // Transmit  Byte Count
	SendFrame[++j] = c_numb;      								
	
	// Transmit  Data
	for (i = 1, c_d = c_data; i <= c_numb; i++)
		SendFrame[++j] = *c_d++;	
	
	// Transmit Checksum
	Int temp = j + 1 - preamble_lenght;
	SendFrame[++j] = Checksum((SendFrame+preamble_lenght), temp);	
	FrameLenght = j+1;
	
	return;
}

/***********************************************************************************
*  Function   : Checksum
*----------------------------------------------------------------------------------
*  Object     : To generate checksum in HART frame
*
*  Return     : None
*				
*----------------------------------------------------------------------------------
*  Variable Name     |IN |OUT|GLB|  Utilization
*--------------------+---+---+---+-------------------------------------------------
*  s                 | X |   |   |  Pointer to array of bytes
*--------------------+---+---+---+-------------------------------------------------
*  nbytes            | X |   |   |  number of bytes
*--------------------+---+---+---+-------------------------------------------------
***********************************************************************************/
UChar Checksum(UChar *s, UChar nbytes)
{
    UChar i, check;
    UChar *p;
	
    for( i = 1, check = 0, p = s; i <= nbytes; ++i )
		check ^= *p++;	
    return(check);
}

/***********************************************************************************
*  Function   : LongAddr
*----------------------------------------------------------------------------------
*  Object     : To find long address(5bytes) of device connected to the CH_num
*
*  Return     : None
*				
*----------------------------------------------------------------------------------
*  Variable Name     |IN |OUT|GLB|  Utilization
*--------------------+---+---+---+-------------------------------------------------
*  CH_num            | X |   |   |  related Channel Number
*--------------------+---+---+---+-------------------------------------------------
***********************************************************************************/
void LongAddr(UChar CH_num)
{
	Addr = Device[CH_num].LongAddr;
	AddrType = 1;
}

/***********************************************************************************
*  Function   : ShortAddr
*----------------------------------------------------------------------------------
*  Object     : To find Short address(1 UChar) of device connected to the CH_num
*
*  Return     : None
*				
*----------------------------------------------------------------------------------
*  Variable Name     |IN |OUT|GLB|  Utilization
*--------------------+---+---+---+-------------------------------------------------
*  CH_num            | X |   |   |  related Channel Number
*--------------------+---+---+---+-------------------------------------------------
***********************************************************************************/
void ShortAddr(UChar CH_num)
{
	Addr = &Device[CH_num].ShortAddr;
	AddrType = 0;
}

/***********************************************************************************
*  Function   : SwitchToChannel
*----------------------------------------------------------------------------------
*  Object     : To switch to desired HART channel
*
*  Return     : None
*				
*----------------------------------------------------------------------------------
*  Variable Name     |IN |OUT|GLB|  Utilization
*--------------------+---+---+---+-------------------------------------------------
*  channel           | X |   |   |
*--------------------+---+---+---+-------------------------------------------------
***********************************************************************************/
void SwitchToChannel(UChar channel)
{
	HART_ADDR0_DEF;
	HART_ADDR1_DEF;
	HART_ADDR2_DEF;
	
	if ((channel & 0x01))
		HART_ADDR0_HIGH;	// SET HART ADDR0 HIGH
	else
		HART_ADDR0_LOW;		// SET HART ADDR0 LOW
	
	if ((channel & 0x02))
		HART_ADDR1_HIGH;	// SET HART ADDR1 HIGH
	else
		HART_ADDR1_LOW;		// SET HART ADDR1 LOW
	
	if ((channel & 0x04))
		HART_ADDR2_HIGH;	// SET HART ADDR2 HIGH
	else
		HART_ADDR2_LOW;		// SET HART ADDR2 LOW
}

/***********************************************************************************
*  Function   : HARTInit
*----------------------------------------------------------------------------------
*  Object     : To initialize timers,interrupts,external RAM
*
*  Return     : None
*
***********************************************************************************/
void HARTInit(void)
{	
	UART3_init();
    for (UChar i = 0; i < 8; i++)
	{
		ReqBuff[i][0]  = false;
		ReqBuff[i][1]  = false;
		ReqBuff[i][2]  = false;
		Device[i].fill = false;
		for(UChar j = 0; j < 5 ; j++)
			Device[i].LongAddr[j]=0;
		
		for(UChar j = 0; j < 20; j++)
			Device[i].cmd0[j]=0;
		
		for(UChar j = 0; j < 35; j++)
			Device[i].LongTAG[j]=0;
	}
	
	OCD_DEF;			//  Initialize Carrier Detect signal
	INRTS_DEF;			//  Set Modem as Reciever, INRTS = 1
	INRTS_HIGH;
	
	// Initialize HART Channels:
	HART_ADDR0_DEF;
	HART_ADDR1_DEF;
	HART_ADDR2_DEF;
	
	// Set Multiplexer to HART Channel 0:
	HART_ADDR0_LOW;		// SET HART ADDR0 LOW
	HART_ADDR1_LOW;		// SET HART ADDR1 LOW
	HART_ADDR2_LOW;		// SET HART ADDR2 LOW	
	printf_P(msgHARTInitDone);
}

/***********************************************************************************
*  Function   : DataStorage
*----------------------------------------------------------------------------------
*  Object     : To save HART Data
*
*  Return     : None
*
***********************************************************************************/
void DataStorage(void)
{
    if( Command == 0 )
    {
        Device[Channel].fill	= true;
        Device[Channel].cmd0[0] = data_byte;
        Device[Channel].Status 	= HART_RxMsg.DATA[1];
        memcpy(Device[Channel].cmd0 + 1, HART_RxMsg.DATA, data_byte);
		
        Device[Channel].ShortAddr   = (*Addr) & 0x3f;
        Device[Channel].LongAddr[0] = Device[Channel].cmd0[4];
        Device[Channel].LongAddr[1] = Device[Channel].cmd0[5];
        memcpy(Device[Channel].LongAddr + 2, Device[Channel].cmd0 + 12, 3);
		
        pream_L = Device[Channel].cmd0[6];
    }
	
    HART_CAN_TX_Buffer.Channel   = Channel;
	for ( UChar i = 0 ; i < data_byte ; i++ )
        HART_CAN_TX_Buffer.Byte[i] = HART_RxMsg.DATA[i];
	
    HART_CAN_TX_Buffer.ByteCount = data_byte;
    Card.HARTState = Channel+1;
	
#ifdef DEBUG_HART
    printf_P("\r\n%d Byte Received",data_byte);
    printf_P("\r\nRX=[ ");
	
    for ( UChar i = 0 ; i < data_byte ; i++ )
    {
        HART_CAN_TX_Buffer.Byte[i] = HART_RxMsg.DATA[i];
        printf_P("%d ", HART_RxMsg.DATA[i]);
    }
	printf_P(" ]\r\n");
	
#endif
	
}

#endif

//***********************************************************************************
// No More
//***********************************************************************************
