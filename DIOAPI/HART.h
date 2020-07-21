/***********************************************************************************
*  File      : HART.H
*----------------------------------------------------------------------------------
*  Object    : it includes required functions for Initializing and handling HART
*
*  Contents  : variables, structures, definitions ...
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

#ifndef HART_H
#define HART_H

/*********************************************************************************************************
* HART Commands
*********************************************************************************************************/
#define WAITING_FOR_RESPONSE    48
#define HART_RCV_ERROR          49
#define HART_IDLE               0
#define TRANSMIT_HART_COMMAND   1
#define RECEIVING_HART_PACKET   2

/***********************************************************************************
 * HART Structures
 ***********************************************************************************/
// Database structure
// For each Command:
// 		First Byte: response data length
// 		Second & third Byte: device status bytes
// 		The rest part: response data bytes
struct DataBase{
	bool fill;
	UChar ShortAddr;
	UChar LongAddr[5];
	UChar Status;
	UChar cmd0[20];
    UChar LongTAG[35];          // Long TAG
	//UChar AstMngCmd[255];		// Asset Management Command Response
};

// For Asset Management Command:
// 		First & second Byte: response data length
// 		Third & forth Byte: device status bytes
// 		The rest part: response data bytes
struct ReqDatas{
	//bool fill;
	int  DataLength;
	UChar CardAddress[4];     	
	UChar ServiceType;
	UChar cmd;                	//Index[0]
	UChar ChannelNum;			//Index[1]
	UChar DeviceAddr;			//SubIndex
	UChar TLDU_Handle;
	UChar Status;
	UChar Data[255];// Changed from 40
};

struct HART_data{
	UChar Status;
	union HARTDataError{
		struct HARTDataErrorBit{
			UChar PrimVarFill:1;
			UChar ScndVarFill:1;
			UChar TrdVarFill:1;
			UChar QuatVarFill:1;
			UChar Res1:1;
			UChar Res2:1;
			UChar Res3:1;
			UChar Res4:1;
		}Bit;
		UChar D8;
	}HARTError;
	UChar PrimVar[5];
	UChar ScndVar[5];
	UChar TrdVar[5];
	UChar QuatVar[5];
    UChar WriteDeviceVar[7];
};


struct HARTError{
	union HARTCmdError{
		struct HARTCmdErrorBit{
			UChar StatusChanged:1;
			UChar Cmd3ByteCountError:1;
			UChar Cmd48ByteCountError:1;
			UChar Cmd3CommunicationError:1;
			UChar Cmd48CommunicationError:1;
			UChar Cmd3CmdError:1;
			UChar Cmd48CmdError:1;
			UChar DevNotResp:1;
		}Bit;
		UChar D8;
	} HARTError;
	UChar RespCode;
	UChar StatusLength;
	UChar Status[25];
	UChar ErrorOccurred:1;
};

// A NEW STructure for a HART Message
typedef struct
{
    UChar Preamble[20];
    UChar PELength;
    UChar FrameType;
    UChar PhysicalLayerType;
    UChar Delimiter;
    UChar Address[5];
    bool  AddressType;          // 0 : Polling Address 1 : Unique 5byte Address
    UChar NumberOfExpansionByte;
    UChar ExpansionByte[10];
    UChar Command;
    UChar ByteCount;
    UChar DATA[40];
    UChar CheckByte;
}tHARTMSG;

typedef struct  // A NEW STructure for a HART Message
{
   UChar Byte[100];
   UChar pointer;
}tUARTBuffer;

typedef struct  // A NEW STructure for a HART Data to be transmitted on CAN
{
   UChar Byte[256];
   UChar Pointer;
   UChar ByteCount;
   bool DataReadyToSend;
   UChar Channel ;
}tDataBuffer;

// External function prototypes:
extern tUARTBuffer UART_TX_Buffer;
/***********************************************************************************
 *  Subroutines Declarations
 ***********************************************************************************/
void  HART_MainStateMachine(void);
UChar SendBlock(UChar ByteCount);
void  FrameFormation(Int preamble_lenght, UChar *addr, UChar cmd, UChar *c_data, UChar c_numb, bool address_type);
UChar Checksum(UChar *s, UChar nbytes);
void  LongAddr(UChar Channel);
void  ShortAddr(UChar Channel);
void  HARTInit(void);
void  SwitchToChannel(UChar channel);
void  DataStorage(void);
void  UART3_init(void);
void  timer0_init(void);

/***********************************************************************************
 *  HART_PHY.c subroutines
 ***********************************************************************************/
UChar Send_Data_UART( UChar *Data, UChar DataLen );
UChar Decode_HART_Message( void );
UChar Fill_UART_Buffer( void );

#endif

//***********************************************************************************
// No More
//***********************************************************************************
