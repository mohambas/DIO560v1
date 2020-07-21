/***********************************************************************************
*  File     : MCP2515.h
*----------------------------------------------------------------------------------
*  Object   : it defines all MCP2515-related constants
*
*  Contents : MCP2515 related constants, function declarations, ...
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

#ifndef __MCP2515_H
#define __MCP2515_H

/*************************************************************************************
*       Defines MCP2515 register addresses
**************************************************************************************/
#define         RXF0SIDH	0x00
#define		    RXF0SIDL	0x01
#define 		RXF0EID8	0x02
#define 		RXF0EID0	0x03
#define	    	RXF1SIDH	0x04
#define 		RXF1SIDL	0x05
#define		    RXF1EID8	0x06
#define	    	RXF1EID0	0x07
#define		    RXF2SIDH	0x08
#define		    RXF2SIDL	0x09
#define		    RXF2EID8	0x0A
#define		    RXF2EID0	0x0B
#define		    BFPCTRL		0x0C
#define		    TXRTSCTRL	0x0D
#define		    CANSTAT		0x0E
#define		    CANCTRL		0x0F
#define		    RXF3SIDH	0x10
#define		    RXF3SIDL	0x11
#define		    RXF3EID8	0x12
#define		    RXF3EID0	0x13
#define		    RXF4SIDH	0x14
#define		    RXF4SIDL	0x15
#define		    RXF4EID8	0x16
#define		    RXF4EID0	0x17
#define		    RXF5SIDH	0x18
#define		    RXF5SIDL	0x19
#define		    RXF5EID8	0x1A
#define		    RXF5EID0	0x1B
#define		    TEC		    0x1C
#define		    REC		    0x1D
#define		    RXM0SIDH	0x20
#define		    RXM0SIDL	0x21
#define		    RXM0EID8	0x22
#define		    RXM0EID0	0x23
#define		    RXM1SIDH	0x24
#define		    RXM1SIDL	0x25
#define		    RXM1EID8	0x26
#define		    RXM1EID0	0x27
#define		    CNF3		0x28
#define		    CNF2		0x29
#define		    CNF1		0x2A
#define		    CANINTE		0x2B
#define		    CANINTF		0x2C
#define		    EFLG		0x2D
#define		    TXB0CTRL	0x30
#define		    TXB0SIDH	0x31
#define		    TXB0SIDL	0x32
#define		    TXB0EID8	0x33
#define		    TXB0EID0	0x34
#define		    TXB0DLC		0x35
#define		    TXB0D0		0x36
#define		    TXB0D1		0x37
#define		    TXB0D2		0x38
#define		    TXB0D3		0x39
#define		    TXB0D4		0x3A
#define		    TXB0D5		0x3B
#define		    TXB0D6		0x3C
#define		    TXB0D7		0x3D
#define		    TXB1CTRL	0x40
#define		    TXB1SIDH	0x41
#define		    TXB1SIDL	0x42
#define		    TXB1EID8	0x43
#define		    TXB1EID0	0x44
#define		    TXB1DLC		0x45
#define		    TXB1D0		0x46
#define		    TXB1D1		0x47
#define		    TXB1D2		0x48
#define		    TXB1D3		0x49
#define		    TXB1D4		0x4A
#define		    TXB1D5		0x4B
#define		    TXB1D6		0x4C
#define		    TXB1D7		0x4D
#define		    TXB2CTRL	0x50
#define		    TXB2SIDH	0x51
#define		    TXB2SIDL	0x52
#define		    TXB2EID8	0x53
#define		    TXB2EID0	0x54
#define		    TXB2DLC		0x55
#define		    TXB2D0		0x56
#define		    TXB2D1		0x57
#define		    TXB2D2		0x58
#define		    TXB2D3		0x59
#define		    TXB2D4		0x5A
#define		    TXB2D5		0x5B
#define		    TXB2D6		0x5C
#define		    TXB2D7		0x5D
#define		    RXB0CTRL	0x60
#define		    RXB0SIDH	0x61
#define		    RXB0SIDL	0x62
#define		    RXB0EID8	0x63
#define		    RXB0EID0	0x64
#define		    RXB0DLC		0x65
#define		    RXB0D0		0x66
#define		    RXB0D1		0x67
#define		    RXB0D2		0x68
#define		    RXB0D3		0x69
#define		    RXB0D4		0x6A
#define		    RXB0D5		0x6B
#define		    RXB0D6		0x6C
#define		    RXB0D7		0x6D
#define		    RXB1CTRL	0x70
#define		    RXB1SIDH	0x71
#define		    RXB1SIDL	0x72
#define		    RXB1EID8	0x73
#define		    RXB1EID0	0x74
#define		    RXB1DLC		0x75
#define		    RXB1D0		0x76
#define		    RXB1D1		0x77
#define		    RXB1D2		0x78
#define		    RXB1D3		0x79
#define		    RXB1D4		0x7A
#define		    RXB1D5		0x7B
#define		    RXB1D6		0x7C
#define		    RXB1D7		0x7D

/************************************************************************************
*       MCP2515 TXBnCTRL register
*************************************************************************************/
//Mask
#define         MCP_TXBnCTRL_ABTF_M  0x40
#define         MCP_TXBnCTRL_MLOA_M  0x20
#define         MCP_TXBnCTRL_TXERR_M 0x10
#define         MCP_TXBnCTRL_TXREQ_M 0x08
#define         MCP_TXBnCTRL_TXP10_M 0x03

//Set
#define         MCP_TXBnCTRL_ABTF_S  0x40
#define         MCP_TXBnCTRL_MLOA_S  0x20
#define         MCP_TXBnCTRL_TXERR_S 0x10
#define         MCP_TXBnCTRL_TXREQ_S 0x08
#define         MCP_TXBnCTRL_TXP1_S  0x02
#define         MCP_TXBnCTRL_TXP0_S  0x01

//Location
#define         MCP_TXBnCTRL_ABTF_L  0x06
#define         MCP_TXBnCTRL_MLOA_L  0x05
#define         MCP_TXBnCTRL_TXERR_L 0x04
#define         MCP_TXBnCTRL_TXREQ_L 0x03
#define         MCP_TXBnCTRL_TXP1_L  0x01
#define         MCP_TXBnCTRL_TXP0_L  0x00

//Reset
#define         MCP_TXBnCTRL_ABTF_R  0xBF
#define         MCP_TXBnCTRL_MLOA_R  0xDF
#define         MCP_TXBnCTRL_TXERR_R 0xEF
#define         MCP_TXBnCTRL_TXREQ_R 0xF7
#define         MCP_TXBnCTRL_TXP10_R 0xFC

//Misc.
#define         HIGHEST_MESS_PRIORITY		(MCP_TXBnCTRL_TXP1_S | MCP_TXBnCTRL_TXP0_S)
#define         HIGH_INTERM_MESS_PRIORITY	(MCP_TXBnCTRL_TXP1_S)
#define         LOW_INTERM_MESS_PRIORITY	(MCP_TXBnCTRL_TXP0_S)
#define         LOWEST_MESS_PRIORITY	    (0)

#define         TXB0PRIORITY                HIGHEST_MESS_PRIORITY
#define         TXB1PRIORITY                HIGH_INTERM_MESS_PRIORITY
#define         TXB2PRIORITY                LOW_INTERM_MESS_PRIORITY

/***********************************************************************************
*       MCP2515 TXRTSCTRL register 
************************************************************************************/
//Mask
#define         MCP_TXRTSCTRL_B2RTS_M       0x20
#define         MCP_TXRTSCTRL_B1RTS_M       0x10
#define         MCP_TXRTSCTRL_B0RTS_M       0x08
#define         MCP_TXRTSCTRL_B2RTSM_M      0x04
#define         MCP_TXRTSCTRL_B1RTSM_M      0x02
#define         MCP_TXRTSCTRL_B0RTSM_M      0x01

//Set
#define         MCP_TXRTSCTRL_B2RTS_S       0x20
#define         MCP_TXRTSCTRL_B1RTS_S       0x10
#define         MCP_TXRTSCTRL_B0RTS_S       0x08
#define         MCP_TXRTSCTRL_B2RTSM_S      0x04
#define         MCP_TXRTSCTRL_B1RTSM_S      0x02
#define         MCP_TXRTSCTRL_B0RTSM_S      0x01

//Location
#define         MCP_TXRTSCTRL_B2RTS_L       0x05
#define         MCP_TXRTSCTRL_B1RTS_L       0x04
#define         MCP_TXRTSCTRL_B0RTS_L       0x03
#define         MCP_TXRTSCTRL_B2RTSM_L      0x02
#define         MCP_TXRTSCTRL_B1RTSM_L      0x01
#define         MCP_TXRTSCTRL_B0RTSM_L      0x00

//Reset
#define         MCP_TXRTSCTRL_B2RTS_R       0xDF
#define         MCP_TXRTSCTRL_B1RTS_R       0xEF
#define         MCP_TXRTSCTRL_B0RTS_R       0xF7
#define         MCP_TXRTSCTRL_B2RTSM_R      0xFB
#define         MCP_TXRTSCTRL_B1RTSM_R      0xFD
#define         MCP_TXRTSCTRL_B0RTSM_R      0xFE

//Misc.
#define         TXRTSCTRL_Default 0x00

/***************************************************************************************
*       TXBnSIDL Register 
***************************************************************************************/
//Mask
#define         MCP_TXBnSIDL_EXIDE_M        0x08
#define         MCP_TXBnSIDL_SID2_0_M       0xE0
#define         MCP_TXBnSIDL_EID17_16_M     0x03

//Set
#define         MCP_TXBnSIDL_EXIDE_S        0x08
#define         MCP_TXBnSIDL_SID2_0_S       0xE0
#define         MCP_TXBnSIDL_EID17_16_S     0x03

//Location
#define         MCP_TXBnSIDL_EXIDE_L        0x03

//Reset
#define         MCP_TXBnSIDL_EXIDE_R        0xF7
#define         MCP_TXBnSIDL_SID2_0_R       0x1F
#define         MCP_TXBnSIDL_EID17_16_R     0xFC

//Misc.
#define         TXB_SIDVAL                  0 

/***************************************************************************************
*       TXBnEID Register 
***************************************************************************************/
// The fixed data during sending packets via CAN. Except when it comes to hot redundancy
#define         TXB_EIDVAL  (0xC000 | (((ULong) Card.Group & 0x7) << 11) | (((Card.Type >> 3) & 0x1F ) << 6) | Card.BPAddress) 

/****************************************************************************************
*       TXBnDLC Register Bits
****************************************************************************************/
//Mask
#define         MCP_TXBnDLC_RTR_M           0x40    
#define         MCP_TXBnDLC_DLC_M           0x0F

//Set
#define         MCP_TXBnDLC_RTR_S           0x40    
#define         MCP_TXBnDLC_DLC_S           0x0F

//Location
#define         MCP_TXBnDLC_RTR_L           0x06    


//Reset
#define         MCP_TXBnDLC_RTR_R           0xBF    
#define         MCP_TXBnDLC_DLC_R           0xF0    

/**************************************************************************************
*       MCP2515 RXB0CTRL register Mask
***************************************************************************************/
//Mask
#define         MCP_RXB0CTRL_RXM1_M         0x40
#define         MCP_RXB0CTRL_RXM0_M         0x20
#define         MCP_RXB0CTRL_RXRTR_M        0x08
#define         MCP_RXB0CTRL_BUKT_M	        0x04
#define         MCP_RXB0CTRL_BUKT1_M        0x02
#define         MCP_RXB0CTRL_FILHIT0_M      0x01

//Set
#define         MCP_RXB0CTRL_RXM1_S         0x40
#define         MCP_RXB0CTRL_RXM0_S         0x20
#define         MCP_RXB0CTRL_RXRTR_S        0x08
#define         MCP_RXB0CTRL_BUKT_S         0x04
#define         MCP_RXB0CTRL_BUKT1_S        0x02
#define         MCP_RXB0CTRL_FILHIT0_S      0x01

//Location
#define         MCP_RXB0CTRL_RXM1_L         0x06
#define         MCP_RXB0CTRL_RXM0_L         0x05
#define         MCP_RXB0CTRL_RXRTR_L        0x03
#define         MCP_RXB0CTRL_BUKT_L	        0x02
#define         MCP_RXB0CTRL_BUKT1_L        0x01
#define         MCP_RXB0CTRL_FILHIT0_L      0x00

//Reset
#define         MCP_RXB0CTRL_RXM1           0xBF
#define         MCP_RXB0CTRL_RXM0           0xDF
#define         MCP_RXB0CTRL_RXRTR          0xF7
#define         MCP_RXB0CTRL_BUKT	        0xFB
#define         MCP_RXB0CTRL_BUKT1          0xFD
#define         MCP_RXB0CTRL_FILHIT0        0xFE

//Misc.
#define         RXB0_ROLLEOVER	            MCP_RXB0CTRL_BUKT
#define         RXB0_NOT_ROLLEOVER          0

/**************************************************************************************
*       MCP2515 RXB1CTRL register Mask
***************************************************************************************/

//Mask
#define         MCP_RXB1CTRL_FILHIT2_M      0x04
#define         MCP_RXB1CTRL_FILHIT1_M      0x02
#define         MCP_RXB1CTRL_FILHIT0_M      0x01

//Set
#define         MCP_RXB1CTRL_FILHIT2_S      0x04
#define         MCP_RXB1CTRL_FILHIT1_S      0x02
#define         MCP_RXB1CTRL_FILHIT0_S      0x01

//Location
#define         MCP_RXB1CTRL_FILHIT2_L      0x02
#define         MCP_RXB1CTRL_FILHIT1_L      0x01
#define         MCP_RXB1CTRL_FILHIT0_L      0x00

//Reset
#define         MCP_RXB1CTRL_FILHIT2        0xFB
#define         MCP_RXB1CTRL_FILHIT1        0xFD
#define         MCP_RXB1CTRL_FILHIT0        0xFE

//Misc.
#define         RXM_MASK                    (MCP_RXB0CTRL_RXM1_M | MCP_RXB0CTRL_RXM0_M)
#define         REC_ALL                     (MCP_RXB0CTRL_RXM1_M | MCP_RXB0CTRL_RXM0_M)
#define         REC_ONLY_VALID_EXT          (MCP_RXB0CTRL_RXM1_M)
#define         REC_ONLY_VALID_STD          (MCP_RXB0CTRL_RXM0_M)
#define         REC_ALL_VALID	            (0)

#define         RECBUFF0_OPERATING_MODE     REC_ONLY_VALID_EXT
#define         RECBUFF1_OPERATING_MODE     REC_ONLY_VALID_EXT

#define         RXB1_FILLHIT_MASK           (MCP_RXB1CTRL_FILHIT2_S | MCP_RXB1CTRL_FILHIT1_S | MCP_RXB1CTRL_FILHIT0_S)
#define         FILTERHIT_RXF0		        0x0
#define         FILTERHIT_RXF1		        0x1
#define         FILTERHIT_RXF2		        0x2
#define         FILTERHIT_RXF3		        0x3
#define         FILTERHIT_RXF4		        0x4
#define         FILTERHIT_RXF5		        0x5

/**************************************************************************************
*       MCP2515 BFPCTRL register
***************************************************************************************/

//Mask

#define         MCP_BFPCTRL_B1BFS_M     0x20
#define         MCP_BFPCTRL_B0BFS_M     0x10
#define         MCP_BFPCTRL_B1BFE_M     0x08
#define         MCP_BFPCTRL_B0BFE_M     0x04
#define         MCP_BFPCTRL_B1BFM_M     0x02
#define         MCP_BFPCTRL_B0BFM_M     0x01

//Set
#define         MCP_BFPCTRL_B1BFS_S     0x20
#define         MCP_BFPCTRL_B0BFS_S     0x10
#define         MCP_BFPCTRL_B1BFE_S     0x08
#define         MCP_BFPCTRL_B0BFE_S     0x04
#define         MCP_BFPCTRL_B1BFM_S     0x02
#define         MCP_BFPCTRL_B0BFM_S     0x01

//Location
#define         MCP_BFPCTRL_B1BFS_L     0x05
#define         MCP_BFPCTRL_B0BFS_L     0x04
#define         MCP_BFPCTRL_B1BFE_L     0x03
#define         MCP_BFPCTRL_B0BFE_L     0x02
#define         MCP_BFPCTRL_B1BFM_L     0x01
#define         MCP_BFPCTRL_B0BFM_L     0x00

//Reset
#define         MCP_BFPCTRL_B1BFS_R     0xDF
#define         MCP_BFPCTRL_B0BFS_R     0xEF
#define         MCP_BFPCTRL_B1BFE_R     0xF7
#define         MCP_BFPCTRL_B0BFE_R     0xFB
#define         MCP_BFPCTRL_B1BFM_R     0xFD
#define         MCP_BFPCTRL_B0BFM_R     0xFE

#define         BFPCTRL_Default         0x00

/**************************************************************************************
*       MCP2515 RXBnSIDL register
***************************************************************************************/
//Mask
#define         MCP_RXBnSIDL_SRR_M          0x10
#define         MCP_RXBnSIDL_IDE_M          0x08
#define         MCP_RXBnSIDL_SID2_0_M       0xE0
#define         MCP_RXBnSIDL_EID17_16_M     0x03

//Set
#define         MCP_RXBnSIDL_SRR_S          0x10
#define         MCP_RXBnSIDL_IDE_S          0x08
#define         MCP_RXBnSIDL_SID2_0_S       0xE0
#define         MCP_RXBnSIDL_EID17_16_S     0x03

//Location
#define         MCP_RXBnSIDL_SRR_L          0x04
#define         MCP_RXBnSIDL_IDE_L          0x03

//Reset
#define         MCP_RXBnSIDL_SRR_R          0xEF
#define         MCP_RXBnSIDL_IDE_R          0xF7
#define         MCP_RXBnSIDL_SID2_0_R       0x1F
#define         MCP_RXBnSIDL_EID17_16_R     0xFC

/**************************************************************************************
*       MCP2515 RXBnDLC register Mask
***************************************************************************************/
//Mask
#define         MCP_RXBnDLC_RTR_M           0x40    
#define         MCP_RXBnDLC_DLC_M           0x0F

//Set
#define         MCP_RXBnDLC_RTR_S           0x40    
#define         MCP_RXBnDLC_DLC_S           0x0F

//Location
#define         MCP_RXBnDLC_RTR_L           0x06    

//Reset
#define         MCP_RXBnDLC_RTR_R           0xBF    
#define         MCP_RXBnDLC_DLC_R           0xF0 

/**************************************************************************************
*       MCP2515 RXFnSID register Mask
***************************************************************************************/
//Mask
#define         MCP_RXFnSIDL_EXIDE_M        0x08
#define         MCP_RXFnSIDL_SID2_0_M       0xE0
#define         MCP_RXFnSIDL_EID17_16_M     0x03

//Set
#define         MCP_RXFnSIDL_EXIDE_S        0x08
#define         MCP_RXFnSIDL_SID2_0_S       0xE0
#define         MCP_RXFnSIDL_EID17_16_S     0x03

//Location
#define         MCP_RXFnSIDL_EXIDE_L        0x03

//Reset
#define         MCP_RXFnSIDL_EXIDE_R        0xF7
#define         MCP_RXFnSIDL_SID2_0_R       0x1F
#define         MCP_RXFnSIDL_EID17_16_R     0xFC

// The card type and card address matching during receiving packets from CAN
// should be taken to consdireation. RF0 is for specific packets.
#define         RF0_SIDVAL_NORMAL           0x0000
#define         RF0_EIDVAL_NORMAL           ((((((ULong) ((Card.Type >> 3) & 0x1F))) << 6) & 0x07C0 ) | (Card.BPAddress & 0x3F)) 

// The card type and card address matching during receiving packets from CAN
// should be taken to consdireation. RF1 is for general packets.
#define         RF1_SIDVAL_NORMAL           0x0000
#define         RF1_EIDVAL_NORMAL           0x0000

// The card type and card address matching during receiving packets from CAN
// should be taken to consdireation. RF2 is for specific packets.
#define         RF2_SIDVAL_NORMAL           0x0000
#define         RF2_EIDVAL_NORMAL           ((((((ULong) ((Card.Type >> 3) & 0x1F))) << 6) & 0x07C0 ) | (Card.BPAddress & 0x3F))  

// The card type and card address matching during receiving packets from CAN
// should be taken to consdireation. RF3 is for general packets.
#define         RF3_SIDVAL_NORMAL           0x0000
#define         RF3_EIDVAL_NORMAL           0x0000

// The card type and card address matching during receiving packets from CAN
// should be taken to consdireation. RF4 is for general packets.
#define         RF4_SIDVAL_NORMAL           0x0000
#define         RF4_EIDVAL_NORMAL           0x0000

// The card type and card address matching during receiving packets from CAN
// should be taken to consdireation. RF5 is for general packets.
#define         RF5_SIDVAL_NORMAL           0x0000
#define         RF5_EIDVAL_NORMAL           0x0000

/*
#define         RF0_SIDVAL_HOTREDUNDANCY    0x0100
#define         RF0_EIDVAL_HOTREDUNDANCY    0x0100

#define         RF1_SIDVAL_HOTREDUNDANCY    0x0100
#define         RF1_EIDVAL_HOTREDUNDANCY    0x0100

#define         RF2_SIDVAL_HOTREDUNDANCY    0x0100
#define         RF2_EIDVAL_HOTREDUNDANCY    0x0100  

#define         RF3_SIDVAL_HOTREDUNDANCY    0x0100
#define         RF3_EIDVAL_HOTREDUNDANCY    0x0100

#define         RF4_SIDVAL_HOTREDUNDANCY    0x0100
#define         RF4_EIDVAL_HOTREDUNDANCY    0x0100

#define         RF5_SIDVAL_HOTREDUNDANCY    0x0100
#define         RF5_EIDVAL_HOTREDUNDANCY    0x0100
*/

// The card type and card address matching during receiving packets from CAN in hot redundancy mode
// should be taken to consdireation. RF0, RF1, RF2, RF3, RF4 and RF5 are for specific packets.
#define         RF0_SIDVAL_HOTREDUNDANCY  0x0100
#define         RF0_EIDVAL_HOTREDUNDANCY  ((((((ULong) ((Card.Type >> 3) & 0x1F))) << 6) & 0x07C0 ) | (Card.BPAddress & 0x3F)) 

#define         RF1_SIDVAL_HOTREDUNDANCY  0x0100
#define         RF1_EIDVAL_HOTREDUNDANCY  ((((((ULong) ((Card.Type >> 3) & 0x1F))) << 6) & 0x07C0 ) | (Card.BPAddress & 0x3F)) 

#define         RF2_SIDVAL_HOTREDUNDANCY  0x0100
#define         RF2_EIDVAL_HOTREDUNDANCY  ((((((ULong) ((Card.Type >> 3) & 0x1F))) << 6) & 0x07C0 ) | (Card.BPAddress & 0x3F))   

#define         RF3_SIDVAL_HOTREDUNDANCY  0x0100
#define         RF3_EIDVAL_HOTREDUNDANCY  ((((((ULong) ((Card.Type >> 3) & 0x1F))) << 6) & 0x07C0 ) | (Card.BPAddress & 0x3F)) 

#define         RF4_SIDVAL_HOTREDUNDANCY  0x0100
#define         RF4_EIDVAL_HOTREDUNDANCY  ((((((ULong) ((Card.Type >> 3) & 0x1F))) << 6) & 0x07C0 ) | (Card.BPAddress & 0x3F)) 

#define         RF5_SIDVAL_HOTREDUNDANCY  0x0100
#define         RF5_EIDVAL_HOTREDUNDANCY  ((((((ULong) ((Card.Type >> 3) & 0x1F))) << 6) & 0x07C0 ) | (Card.BPAddress & 0x3F)) 

/**************************************************************************************
*       MCP2515 RXMnSIDL register Mask
***************************************************************************************/
//Mask
#define         MCP_RXMnSIDL_SID2_0_M       0xE0
#define         MCP_RXMnSIDL_EID17_16_M     0x03

//Set
#define         MCP_RXMnSIDL_SID2_0_S       0xE0
#define         MCP_RXMnSIDL_EID17_16_S     0x03

//Reset
#define         MCP_RXMnSIDL_SID2_0_R       0x1F
#define         MCP_RXMnSIDL_EID17_16_R     0xFC

// The card type and card address matching during receiving packets from CAN
// should be taken to consdireation. 
#define         RM0_SIDVAL_NORMAL           0x0180 
#define         RM0_EIDVAL_NORMAL           ((((((ULong) ((Card.Type >> 3) & 0x1F))) << 6) & 0x07C0 ) | (Card.BPAddress & 0x3F)) 

#define         RM1_SIDVAL_NORMAL           0x0180 
#define         RM1_EIDVAL_NORMAL           ((((((ULong) ((Card.Type >> 3) & 0x1F))) << 6) & 0x07C0 ) | (Card.BPAddress & 0x3F)) 

/*
#define         RM0_SIDVAL_HOTREDUNDANCY    0x0100 
#define         RM0_EIDVAL_HOTREDUNDANCY    ((((~((ULong) ((Card.Type >> 3) & 0x1F))) << 6) & 0x07C0 ) | (~Card.BPAddress & 0x3F)) 

#define         RM1_SIDVAL_HOTREDUNDANCY    0x0100 
#define         RM1_EIDVAL_HOTREDUNDANCY    ((((~((ULong) ((Card.Type >> 3) & 0x1F))) << 6) & 0x07C0 ) | (~Card.BPAddress & 0x3F)) 
*/

// The card type and card address matching during receiving packets from CAN in hot redundancy mode
// should be taken to consdireation.
#define         RM0_SIDVAL_HOTREDUNDANCY    0x0100 
#define         RM0_EIDVAL_HOTREDUNDANCY    ((((((ULong) ((Card.Type >> 3) & 0x1F))) << 6) & 0x07C0 ) | (Card.BPAddress & 0x3F)) 

#define         RM1_SIDVAL_HOTREDUNDANCY    0x0100 
#define         RM1_EIDVAL_HOTREDUNDANCY    ((((((ULong) ((Card.Type >> 3) & 0x1F))) << 6) & 0x07C0 ) | (Card.BPAddress & 0x3F)) 

/**************************************************************************************
*       MCP2515 CNF1 register Mask
***************************************************************************************/
//Mask
#define         MCP_CNF1_SJW1_M    0x80 
#define         MCP_CNF1_SJW0_M    0x40
#define         MCP_CNF1_BRP5_M    0x20
#define         MCP_CNF1_BRP4_M    0x10
#define         MCP_CNF1_BRP3_M    0x08
#define         MCP_CNF1_BRP2_M    0x04
#define         MCP_CNF1_BRP1_M    0x02
#define         MCP_CNF1_BRP0_M    0x01

//Set
#define         MCP_CNF1_SJW1_S    0x80 
#define         MCP_CNF1_SJW0_S    0x40
#define         MCP_CNF1_BRP5_S    0x20
#define         MCP_CNF1_BRP4_S    0x10
#define         MCP_CNF1_BRP3_S    0x08
#define         MCP_CNF1_BRP2_S    0x04
#define         MCP_CNF1_BRP1_S    0x02
#define         MCP_CNF1_BRP0_S    0x01

//Location
#define         MCP_CNF1_SJW1_L    0x07 
#define         MCP_CNF1_SJW0_L    0x06
#define         MCP_CNF1_BRP5_L    0x05
#define         MCP_CNF1_BRP4_L    0x04
#define         MCP_CNF1_BRP3_L    0x03
#define         MCP_CNF1_BRP2_L    0x02
#define         MCP_CNF1_BRP1_L    0x01
#define         MCP_CNF1_BRP0_L    0x00

//Reset
#define         MCP_CNF1_SJW1_R    0x7F 
#define         MCP_CNF1_SJW0_R    0xBF
#define         MCP_CNF1_BRP5_R    0xDF
#define         MCP_CNF1_BRP4_R    0xEF
#define         MCP_CNF1_BRP3_R    0xF7
#define         MCP_CNF1_BRP2_R    0xFB
#define         MCP_CNF1_BRP1_R    0xFD
#define         MCP_CNF1_BRP0_R    0xFE

//Misc.
#define         CNF1_SJW_MASK      0xC0
#define         SJW_4TQ		       (MCP_CNF1_SJW1_S1 | MCP_CNF1_SJW0_S)
#define         SJW_3TQ		       (MCP_CNF1_SJW1_S)
#define         SJW_2TQ		       (MCP_CNF1_SJW0_S)
#define         SJW_1TQ		        (0)
#define         CNF1_BRP_MASK	    0x3F
#define         CNF1_BRP	        0x0
#define         CNF1_SJW	        SJW_1TQ

/**************************************************************************************
*       MCP2515 CNF2 register Mask
***************************************************************************************/
//Mask
#define         MCP_CNF2_BTLMODE_M     0x80
#define         MCP_CNF2_SAM_M         0x40
#define         MCP_CNF2_PHSEG12_M     0x20
#define         MCP_CNF2_PHSEG11_M     0x10
#define         MCP_CNF2_PHSEG10_M     0x08
#define         MCP_CNF2_PRSEG2_M      0x04
#define         MCP_CNF2_PRSEG1_M      0x02
#define         MCP_CNF2_PRSEG0_M      0x01

//Set
#define         MCP_CNF2_BTLMODE_S     0x80
#define         MCP_CNF2_SAM_S         0x40
#define         MCP_CNF2_PHSEG12_S     0x20
#define         MCP_CNF2_PHSEG11_S     0x10
#define         MCP_CNF2_PHSEG10_S     0x08
#define         MCP_CNF2_PRSEG2_S      0x04
#define         MCP_CNF2_PRSEG1_S      0x02
#define         MCP_CNF2_PRSEG0_S      0x01

//Location
#define         MCP_CNF2_BTLMODE_L      0x07
#define         MCP_CNF2_SAM_L          0x06
#define         MCP_CNF2_PHSEG12_L      0x05
#define         MCP_CNF2_PHSEG11_L      0x04
#define         MCP_CNF2_PHSEG10_L      0x03
#define         MCP_CNF2_PRSEG2_L       0x02
#define         MCP_CNF2_PRSEG1_L       0x01
#define         MCP_CNF2_PRSEG0_L       0x00

//Reset
#define         MCP_CNF2_BTLMODE_R      0x7F
#define         MCP_CNF2_SAM_R          0xBF
#define         MCP_CNF2_PHSEG12_R      0xDF
#define         MCP_CNF2_PHSEG11_R      0xEF
#define         MCP_CNF2_PHSEG10_R      0xF7
#define         MCP_CNF2_PRSEG2_R       0xFB
#define         MCP_CNF2_PRSEG1_R       0xFD
#define         MCP_CNF2_PRSEG0_R       0xFE

//Misc.
#define         PHSEG1_MASK             0x38
#define         PRSEG_MASK	            0x07

/*************************************************************************************
*       MCP2515 CNF3 register Mask
**************************************************************************************/
//Mask
#define         MCP_CNF3_SOF_M     0x80
#define         MCP_CNF3_WAKFIL_M  0x40
#define         MCP_CNF3_PHSEG22_M 0x04
#define         MCP_CNF3_PHSEG21_M 0x02
#define         MCP_CNF3_PHSEG20_M 0x01

//Set
#define         MCP_CNF3_SOF_S     0x80
#define         MCP_CNF3_WAKFIL_S  0x40
#define         MCP_CNF3_PHSEG22_S 0x04
#define         MCP_CNF3_PHSEG21_S 0x02
#define         MCP_CNF3_PHSEG20_S 0x01

//Location
#define         MCP_CNF3_SOF_L     0x07
#define         MCP_CNF3_WAKFIL_L  0x06
#define         MCP_CNF3_PHSEG22_L 0x02
#define         MCP_CNF3_PHSEG21_L 0x01
#define         MCP_CNF3_PHSEG20_L 0x00

//Reset
#define         MCP_CNF3_SOF_R     0x7F
#define         MCP_CNF3_WAKFIL_R  0xBF
#define         MCP_CNF3_PHSEG22_R 0xFB
#define         MCP_CNF3_PHSEG21_R 0xFD
#define         MCP_CNF3_PHSEG20_R 0xFE

//Misc.
#define         CNF3_PHSEG2_MASK    0x07


/**************************************************************************************
*       MCP2515 EFLG register Mask
***************************************************************************************/
//Mask
#define         MCP_EFLG_RX1OVR_M 0x80
#define         MCP_EFLG_RX0OVR_M 0x40
#define         MCP_EFLG_TXBO_M   0x20
#define         MCP_EFLG_TXEP_M   0x10
#define         MCP_EFLG_RXEP_M   0x08
#define         MCP_EFLG_TXWAR_M  0x04
#define         MCP_EFLG_RXWAR_M  0x02
#define         MCP_EFLG_EWARN_M  0x01

//Set
#define         MCP_EFLG_RX1OVR_S 0x80
#define         MCP_EFLG_RX0OVR_S 0x40
#define         MCP_EFLG_TXBO_S   0x20
#define         MCP_EFLG_TXEP_S   0x10
#define         MCP_EFLG_RXEP_S   0x08
#define         MCP_EFLG_TXWAR_S  0x04
#define         MCP_EFLG_RXWAR_S  0x02
#define         MCP_EFLG_EWARN_S  0x01

//Location
#define         MCP_EFLG_RX1OVR_L 0x07
#define         MCP_EFLG_RX0OVR_L 0x06
#define         MCP_EFLG_TXBO_L   0x05
#define         MCP_EFLG_TXEP_L   0x04
#define         MCP_EFLG_RXEP_L   0x03
#define         MCP_EFLG_TXWAR_L  0x02
#define         MCP_EFLG_RXWAR_L  0x01
#define         MCP_EFLG_EWARN_L  0x00

//Reset
#define         MCP_EFLG_RX1OVR_R 0x7F
#define         MCP_EFLG_RX0OVR_R 0xBF
#define         MCP_EFLG_TXBO_R   0xDF
#define         MCP_EFLG_TXEP_R   0xEF
#define         MCP_EFLG_RXEP_R   0xF7
#define         MCP_EFLG_TXWAR_R  0xFB
#define         MCP_EFLG_RXWAR_R  0xFD
#define         MCP_EFLG_EWARN_R  0xFE

/**************************************************************************************
*   MCP2515 CANINTE register Mask
***************************************************************************************/
//Mask
#define         MCP_CANINTE_MERRE_M   0x80
#define         MCP_CANINTE_WAKIE_M   0x40
#define         MCP_CANINTE_ERRIE_M   0x20
#define         MCP_CANINTE_TX2IE_M   0x10
#define         MCP_CANINTE_TX1IE_M   0x08
#define         MCP_CANINTE_TX0IE_M   0x04
#define         MCP_CANINTE_RX1IE_M   0x02
#define         MCP_CANINTE_RX0IE_M   0x01

//Set
#define         MCP_CANINTE_MERRE_S   0x80
#define         MCP_CANINTE_WAKIE_S   0x40
#define         MCP_CANINTE_ERRIE_S   0x20
#define         MCP_CANINTE_TX2IE_S   0x10
#define         MCP_CANINTE_TX1IE_S   0x08
#define         MCP_CANINTE_TX0IE_S   0x04
#define         MCP_CANINTE_RX1IE_S   0x02
#define         MCP_CANINTE_RX0IE_S   0x01

//Location
#define         MCP_CANINTE_MERRE_L   0x07
#define         MCP_CANINTE_WAKIE_L   0x06
#define         MCP_CANINTE_ERRIE_L   0x05
#define         MCP_CANINTE_TX2IE_L   0x04
#define         MCP_CANINTE_TX1IE_L   0x03
#define         MCP_CANINTE_TX0IE_L   0x02
#define         MCP_CANINTE_RX1IE_L   0x01
#define         MCP_CANINTE_RX0IE_L   0x00

//Reset
#define         MCP_CANINTE_MERRE_R   0x7F
#define         MCP_CANINTE_WAKIE_R   0xBF
#define         MCP_CANINTE_ERRIE_R   0xDF
#define         MCP_CANINTE_TX2IE_R   0xEF
#define         MCP_CANINTE_TX1IE_R   0xF7
#define         MCP_CANINTE_TX0IE_R   0xFB
#define         MCP_CANINTE_RX1IE_R   0xFD
#define         MCP_CANINTE_RX0IE_R   0xFE

/**************************************************************************************
*   MCP2515 CANINTF register Mask
***************************************************************************************/
//Mask
#define         MCP_CANINTF_MERRF_M           0x80
#define         MCP_CANINTF_WAKIF_M           0x40
#define         MCP_CANINTF_ERRIF_M           0x20
#define         MCP_CANINTF_TX2IF_M           0x10
#define         MCP_CANINTF_TX1IF_M           0x08
#define         MCP_CANINTF_TX0IF_M           0x04
#define         MCP_CANINTF_RX1IF_M           0x02
#define         MCP_CANINTF_RX0IF_M           0x01

//Set
#define         MCP_CANINTF_MERRF_S           0x80
#define         MCP_CANINTF_WAKIF_S           0x40
#define         MCP_CANINTF_ERRIF_S           0x20
#define         MCP_CANINTF_TX2IF_S           0x10
#define         MCP_CANINTF_TX1IF_S           0x08
#define         MCP_CANINTF_TX0IF_S           0x04
#define         MCP_CANINTF_RX1IF_S           0x02
#define         MCP_CANINTF_RX0IF_S           0x01

//Location
#define         MCP_CANINTF_MERRF_L           0x07
#define         MCP_CANINTF_WAKIF_L           0x06
#define         MCP_CANINTF_ERRIF_L           0x05
#define         MCP_CANINTF_TX2IF_L           0x04
#define         MCP_CANINTF_TX1IF_L           0x03
#define         MCP_CANINTF_TX0IF_L           0x02
#define         MCP_CANINTF_RX1IF_L           0x01
#define         MCP_CANINTF_RX0IF_L           0x00

//Reset
#define         MCP_CANINTF_MERRF_R           0x7F
#define         MCP_CANINTF_WAKIF_R           0xBF
#define         MCP_CANINTF_ERRIF_R           0xDF
#define         MCP_CANINTF_TX2IF_R           0xEF
#define         MCP_CANINTF_TX1IF_R           0xF7
#define         MCP_CANINTF_TX0IF_R           0xFB
#define         MCP_CANINTF_RX1IF_R           0xFD
#define         MCP_CANINTF_RX0IF_R           0xFE

/**************************************************************************************
*   MCP2515 CANCTRL register Mask
***************************************************************************************/
//Mask
#define         MCP_CANCTRL_REQOP2_M          0x80
#define         MCP_CANCTRL_REQOP1_M          0x40
#define         MCP_CANCTRL_REQOP0_M          0x20
#define         MCP_CANCTRL_ABAT_M            0x10
#define         MCP_CANCTRL_OSM_M             0x08
#define         MCP_CANCTRL_CLKEN_M           0x04
#define         MCP_CANCTRL_CLKPRE1_M         0x02
#define         MCP_CANCTRL_CLKPRE0_M         0x01

//Set
#define         MCP_CANCTRL_REQOP2_S          0x80
#define         MCP_CANCTRL_REQOP1_S          0x40
#define         MCP_CANCTRL_REQOP0_S          0x20
#define         MCP_CANCTRL_ABAT_S            0x10
#define         MCP_CANCTRL_OSM_S             0x08
#define         MCP_CANCTRL_CLKEN_S           0x04
#define         MCP_CANCTRL_CLKPRE1_S         0x02
#define         MCP_CANCTRL_CLKPRE0_S         0x01

//Location
#define         MCP_CANCTRL_REQOP2_L          0x07
#define         MCP_CANCTRL_REQOP1_L          0x06
#define         MCP_CANCTRL_REQOP0_L          0x05
#define         MCP_CANCTRL_ABAT_L            0x04
#define         MCP_CANCTRL_OSM_L             0x03
#define         MCP_CANCTRL_CLKEN_L           0x02
#define         MCP_CANCTRL_CLKPRE1_L         0x01
#define         MCP_CANCTRL_CLKPRE0_L         0x00

//Reset
#define         MCP_CANCTRL_REQOP2            0x7F
#define         MCP_CANCTRL_REQOP1            0xBF
#define         MCP_CANCTRL_REQOP0            0xDF
#define         MCP_CANCTRL_ABAT              0xEF
#define         MCP_CANCTRL_OSM               0xF7
#define         MCP_CANCTRL_CLKEN             0xFB
#define         MCP_CANCTRL_CLKPRE1           0xFD
#define         MCP_CANCTRL_CLKPRE0           0xFE

//Misc.
#define         MODE_NORMAL                   0x00
#define         MODE_SLEEP                    0x20
#define         MODE_LOOPBACK                 0x40
#define         MODE_LISTENONLY               0x60
#define         MODE_CONFIG                   0x80
#define         MODE_POWERUP	              0xE0
#define         MODE_MASK	                  0xE0

#define         CLKOUT_PS1	                  0x00
#define         CLKOUT_PS2	                  0x01
#define         CLKOUT_PS4	                  0x02
#define         CLKOUT_PS8	                  0x03

/**************************************************************************************
*       MCP2515 CANSTAT register Mask
***************************************************************************************/
//Mask
#define         MCP_CANSTAT_OPMOD2_M 0x80
#define         MCP_CANSTAT_OPMOD1_M 0x40
#define         MCP_CANSTAT_OPMOD0_M 0x20
#define         MCP_CANSTAT_ICOD2_M  0x08
#define         MCP_CANSTAT_ICOD1_M  0x04
#define         MCP_CANSTAT_ICOD0_M  0x02

//Set
#define         MCP_CANSTAT_OPMOD2_S 0x80
#define         MCP_CANSTAT_OPMOD1_S 0x40
#define         MCP_CANSTAT_OPMOD0_S 0x20
#define         MCP_CANSTAT_ICOD2_S  0x08
#define         MCP_CANSTAT_ICOD1_S  0x04
#define         MCP_CANSTAT_ICOD0_S  0x02

//Location
#define         MCP_CANSTAT_OPMOD2_L 0x07
#define         MCP_CANSTAT_OPMOD1_L 0x06
#define         MCP_CANSTAT_OPMOD0_L 0x05
#define         MCP_CANSTAT_ICOD2_L  0x03
#define         MCP_CANSTAT_ICOD1_L  0x02
#define         MCP_CANSTAT_ICOD0_L  0x01

//Reset
#define         MCP_CANSTAT_OPMOD2_R 0x7F
#define         MCP_CANSTAT_OPMOD1_R 0xBF
#define         MCP_CANSTAT_OPMOD0_R 0xDF
#define         MCP_CANSTAT_ICOD2_R  0xF7
#define         MCP_CANSTAT_ICOD1_R  0xFB
#define         MCP_CANSTAT_ICOD0_R  0xFD

//Misc.
#define         CANSTAT_INTCODE     (MCP_CANSTAT_ICOD2_S | MCP_CANSTAT_ICOD1_S | MCP_CANSTAT_ICOD0_S)
#define         CANSTAT_NOINT	    (0)
#define         CANSTAT_ERRINT	    (1 << 1)
#define         CANSTAT_WAKEUPINT   (2 << 1)
#define         CANSTAT_TXB0INT     (3 << 1)
#define         CANSTAT_TXB1INT     (4 << 1)
#define         CANSTAT_TXB2INT     (5 << 1)
#define         CANSTAT_RXB0INT     (6 << 1)
#define         CANSTAT_RXB1INT     (7 << 1)

/**************************************************************************************
       MCP2515 SPI Instruction Set
***************************************************************************************/
#define		    CAN_CMD_RESET		    0xC0
#define		    CAN_CMD_READ		    0x03
#define		    CAN_CMD_ReadRXBuffer	0x90
#define		    CAN_CMD_WRITE		    0x02
#define		    CAN_CMD_LoadTXBuffer	0x40
#define		    CAN_CMD_RTS		        0x80
#define		    CAN_CMD_ReadStatus	    0xA0
#define		    CAN_CMD_RXStatus	    0xB0
#define		    CAN_CMD_BitModify	    0x05

#define         CAN_RST                 0x03
#define         CAN_Buffer0_SIDH        0x00  
#define         CAN_Buffer0_D0	        0x02
#define         CAN_Buffer1_SIDH        0x04  
#define         CAN_Buffer1_D0	        0x06
#define         CAN_TXBuffID            0x00  

#define         CAN_TXBuffData0         0x01  
#define         CAN_TXBuffData1         0x03  
#define         CAN_TXBuffData2         0x05  

#define         CAN_RTS_TXBuff0         0x01  
#define         CAN_RTS_TXBuff1         0x02 
#define         CAN_RTS_TXBuff2         0x04 

#endif //__MCP2515_H 

//***********************************************************************************
// No More
//***********************************************************************************