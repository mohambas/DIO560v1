/***********************************************************************************
*  File      : Hardware.h
*----------------------------------------------------------------------------------
*  Object    : it defines all hardware-related constants
*
*  Contents  : required PORTS Configurations and Definitions (I/O) for LCPC, 
*			   watchdog timer,	external RAM, microcontroller definitions for 
*			   different cards (AO, AI, AIH, AOH)
*
* ---------------------------------------------------------------------------------
*  Date     |	Description
* ----------+----------------------------------------------------------------------
*  93.07.09 | 	Hardware Phase 3, final revision 
* ----------+----------------------------------------------------------------------
*  93.06.29 | 	comments updaQ_BufferQ_Bufferte 
* ----------+----------------------------------------------------------------------
*  93.06.04 | 	Review comments incorporated 
* ----------+----------------------------------------------------------------------
***********************************************************************************/
#ifndef __HARDWARE_H_
#define __HARDWARE_H_

/***********************************************************************************
 *  Header files
 ***********************************************************************************/
//#include <ioavr.h>
#include    "Definitions.h"
#include    <wiringPi.h>

/***********************************************************************************
 *  Hardware definitions for LCPC Cards
 ***********************************************************************************/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  LEDs and BackPlane Address
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define LED1_OUTPUT_DEF  pinMode(PIN_Base_560 + RUN_LED_560     , OUTPUT)     //GP1.0 is output(LED1)
#define LED2_OUTPUT_DEF  pinMode(PIN_Base_560 + STBY_LED_560    , OUTPUT)     //GP1.1 is output(LED2)
#define LED3_OUTPUT_DEF  pinMode(PIN_Base_560 + STATUS_LED_560  , OUTPUT)     //GP1.2 is output(LED3)
#define LED4_OUTPUT_DEF  pinMode(PIN_Base_560 + FLT_LED_560     , OUTPUT)     //GP1.3 is output(LED4)





// LCPC (Local Processor) Board Revision LEDs
//#define READ_LCPC_VERSION   ( (PINH & 0x7C) >> 2 )

#define SET_LED1_LOW    digitalWrite(PIN_Base_560 + RUN_LED_560, HIGH)     //SET LED1 HIGH
#define SET_LED2_LOW    digitalWrite(PIN_Base_560 + STBY_LED_560, HIGH)    //SET LED2 HIGH
#define SET_LED3_LOW    digitalWrite(PIN_Base_560 + STATUS_LED_560, HIGH)  //SET LED3 HIGH
#define SET_LED4_LOW    digitalWrite(PIN_Base_560 + FLT_LED_560, HIGH)     //SET LED4 HIGH


#define SET_LED1_HIGH     digitalWrite(PIN_Base_560 + RUN_LED_560, LOW)      //SET LED1 LOW
#define SET_LED2_HIGH     digitalWrite(PIN_Base_560 + STBY_LED_560, LOW)     //SET LED2 LOW
#define SET_LED3_HIGH     digitalWrite(PIN_Base_560 + STATUS_LED_560,LOW)    //SET LED3 LOW
#define SET_LED4_HIGH     digitalWrite(PIN_Base_560 + FLT_LED_560, LOW)      //SET LED4 LOW


#define BP_ADDR0_DEF     pinMode(PIN_Base_560 + BP_ADDR0     , INPUT)     //GP0.2 is input(BP_ADDR0)
#define BP_ADDR1_DEF     pinMode(PIN_Base_560 + BP_ADDR1     , INPUT)     //GP0.3 is input(BP_ADDR1)
#define BP_ADDR2_DEF     pinMode(PIN_Base_560 + BP_ADDR2     , INPUT)     //GP0.4 is input(BP_ADDR2)
#define BP_ADDR3_DEF     pinMode(PIN_Base_560 + BP_ADDR3     , INPUT)     //GP0.5 is input(BP_ADDR3)
#define BP_ADDR4_DEF     pinMode(PIN_Base_560 + BP_ADDR4     , INPUT)     //GP0.6 is input(BP_ADDR4)
#define BP_ADDR5_DEF     pinMode(PIN_Base_560 + BP_ADDR5     , INPUT)     //GP0.7 is input(BP_ADDR5)

#define BP_ADDR0_READ    digitalRead(PIN_Base_560 + BP_ADDR0)	 //Read BackPlaneAddress 0
#define BP_ADDR1_READ    digitalRead(PIN_Base_560 + BP_ADDR1)	 //Read BackPlaneAddress 1
#define BP_ADDR2_READ    digitalRead(PIN_Base_560 + BP_ADDR2)  //Read BackPlaneAddress 2
#define BP_ADDR3_READ    digitalRead(PIN_Base_560 + BP_ADDR3)  //Read BackPlaneAddress 3
#define BP_ADDR4_READ    digitalRead(PIN_Base_560 + BP_ADDR4)  //Read BackPlaneAddress 4
#define BP_ADDR5_READ    digitalRead(PIN_Base_560 + BP_ADDR5)  //Read BackPlaneAddress 5

//// GeneralLED is a byte for 6 LEDs on board (Run, Standby, Fault, Status1 (CAN0), Status2 (CAN1)), 
//// Although CAN_RxMsg.Cs is exactly the hardware CS pin of CAN Controllers, it is 
//// to set an equal bit in GeneralLED 0 or 1, based on CS of CAN (which can be 0 or l)
//// set   CAN LED(LED4) blinking
#define SET_CAN_LED_BLINK    Card.GeneralLED &= ~(CAN_RxMsg.CS >> 1); 

// reset CAN LED(LED4)
#define	RESET_CAN_LED_BLINK  Card.GeneralLED |=  (CAN_RxMsg.CS >> 1);  
//
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
////  E2PROM Definitions
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//// Some other definitions for Internal E2PROM
//#define WAIT_FOR_INTERNAL_E2PROM_WRITE_COMPLETION   EECR & (1 << EEPE)
//#define INTERNAL_E2PROM_ADDRESS_REGISTER  			EEAR                   
//#define INTERNAL_E2PROM_DATA_REGISTER  				EEDR
//
//// Start eeprom write by setting EEMPE and EEPE
//#define INTERNAL_E2PROM_WRITE_COMMAND         EECR |= (1 << EEMPE); EECR |= (1 << EEPE) 
//
//// Start eeprom read by setting EERE 
//#define INTERNAL_E2PROM_READ_COMMAND          EECR |= (1 << EERE); 
//
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
////  Interrupt Vectors Definitions
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//#define	INTERRUPT_VECTOR_ENABLE    	MCUCR = (1 << IVCE) // Write the Interrupt Vector Change Enable (IVCE) bit to one.      
//#define INTERRUPT_SETTING_NORMAL   	MCUCR &= ~((1 << IVSEL) | (1 << IVCE))// Within four cycles, write the desired value to IVSEL while writing a zero to IVCE.			
//
////  NOTE-------------NOTE--------------NOTE: deprecated	
////#define INTERRUPT_SETTING_BOOTLOAD  	MCUCR = (1 << IVSEL)
//
//#define	LCPC_INTERRUPT_ENABLE		SREG |= 0x80
//#define LCPC_INTERRUPT_DISABLE		SREG &= 0x7F
//
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
////  Watchdog configurations
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//// Set the watchdog reset interval (Find the interval values in general definitions)
//#define	WD_125MS		    WDTCSR = ((1 << WDE)|(1 << WDP1)|(1 << WDP0))// 125ms
//#define	WD_250MS		    WDTCSR = ((1 << WDE)|(1 << WDP2))			 // 250ms
//#define	WD_500MS		    WDTCSR = ((1 << WDE)|(1 << WDP2)|(1 << WDP0))// 500ms
//#define	WD_1000MS		    WDTCSR = ((1 << WDE)|(1 << WDP2)|(1 << WDP1))// 1000ms
//																	     // 2000ms
//#define	WD_2000MS		    WDTCSR = ((1 << WDE)|(1 << WDP2)|(1 << WDP1)|(1 << WDP0))   

// Check if system has encountered a watchdog error or not
// The MCU is reset when the Watchdog Timer period expires and Watchdog is enabled
#define SYSTEM_RESET_VIA_WATCHDOG  	1                      

// Check if Power on reset detected or not
// The MCU is reset when the supply voltage is below the Power-on Reset threshold
#define SYSTEM_POWER_ON_RESET		0         
//
//// Clear WDRF in MCUSR
//#define	WD_CLEARFLG			MCUSR &= ~(1<<WDRF)
//// Enable watchdog timer by setting WDCE and WDE
//#define	WD_ENABLE	   		WDTCSR |= (1<<WDCE) | (1<<WDE)
//// Turn off WDT 
//#define	WD_TURNOFF			WDTCSR = 0x00
//// Start timed equence
//#define	WD_START	   		WDTCSR |= (1<<WDCE) | (1<<WDE)
//// Set new prescaler(time-out) value = 64K cycles (~0.5 s) 
//#define	WD_PRESCALER_SET	WD_500MS 
//
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
////  USART configurations
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//#define	USART0_DISABLE		UCSR0B = 0x00		// Disable the transmitter/receiver while setting baud rate
//#define USART0_SETTING_A	UCSR0A = (1 << U2X0)// double the speed of communication
//#define	USART0_SETTING_C    UCSR0C = 0x06		// 1 stop bit, 8 bit, no parity
//#define USART0_BAUDRATE_L	UBRR0L = 0x08 		// Set baud rate lower byte (Baud rate is 115200 bps)
//#define USART0_BAUDRATE_H   UBRR0H = 0x00		// Set baud rate high byte
//#define USART0_ENABLE		UCSR0B = 0x98       // transmitter/receiver are enabled 
//#define USART0_DATA			UDR0				// data register of USART0
//#define USART0_SETTX_FLG	UCSR0A |= 0x40		// set transmit complete flag
//#define USART0_WAITTX_FLG	(UCSR0A & 0x40)		// wait for transmit completion flag 
//
//#define USART2_SETTING_A    UCSR2A = 0x00	// Setting A register of USART2
//#define USART2_BAUDRATE_H	UBRR2H = 0x01	// Set baud rate high bye (1200)
//#define USART2_BAUDRATE_L   UBRR2L = 0xA1	// Set baud rate low byte (1200)
//#define	USART2_SETTING_C    UCSR2C = 0x36  	// Set frame format: 8data 1stop bit odd parity
//#define USART2_ENABLE		UCSR2B = 0x98  	// Rx/Tx enable, receive interrupt enable and Rx/Tx complete flag
//#define USART2_DATA			UDR2			// data register of USART2
//#define USART2_SETTX_FLG	UCSR2A |= 0x40	// set transmit complete flag
//#define USART2_WAITTX_FLG	(UCSR2A & 0x40)	// wait for transmit completion flag

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  External RAM (deprecated)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/*
// Modify these lines according to external RAM address map
#define	EXTRAM_STARTADD		0x8000	 // External SRAM addresses start from here
#define	EXTRAM_ENDADD		0xB000	 // External SRAM addresses end here

// Number of memory bytes that would be checked in RAM check function
#define	CHECKMARGIN			10		 

// Address offsets
// actually this Internal RAM offset address is not used anywhere!
#define	INT_SRAM_OFFSET		0x0100	 // Internal SRAM addresses can start from here

#define IO_OFFSET           0x3000	 // IO addresses can start from here

#define LCPC_EnExternalMem	XMCRA |= (1 << SRE)	// Enable external memory
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  Timer0 Definitions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//// timer0 CLK = CLK_io / 256(prescaler)=8MHz/256=> 32us [CS02 CS01 CS00]=[1 0 0]= 0x04
//#define   SET_TIMER0_MODE             TCCR0B = 0x04
//
//// timer0 == 1ms, starts from (255-32 = 233) and counts up to 255:: 32*32us= 1ms
//#define   SET_TIMER0_VALUE            TCNT0  = 0XFF - 0x20
//
//#define    ENABLE_TIMER0_INTERRUPT    TIMSK0 |=  (1 << TOIE0)	// enable timer 0
//#define   DISABLE_TIMER0_INTERRUPT    TIMSK0 &= ~(1 << TOIE0)	// disable timer 0
//
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
////  Timer2 Definitions
////~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//// timer2 CLK = CLK_io / 64(prescaler)=8MHz/64= 125KHz (8uSec) [CS22 CS21 CS20]=[1 0 0]= 0x04
//#define SET_TIMER2B_MODE          TCCR2B |= (1 << CS22)
//
//// 1msec ; Up counting; OVF is set when counting from BOTTOM to MAX 
#define SET_TIMER2_VALUE            timer_useconds(1000) 

#define ENABLE_TIMER2_INTERRUPT     signal(SIGALRM,Timer2OVHandler)  // Enable timer 2 
#define DISABLE_TIMER2_INTERRUPT    signal(SIGALRM, SIG_IGN) // Disable timer 2

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//  CARD Type, version, revision, ... Definitions
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// tmpBuf1 == Board Address2:: Moduly Type, Card Type1 and 1 bit of Card Type2
// Bit(0):MSB of Card Type2, Bit(1-2):Card Type1, Bit(3-5):Rsv, Bit(6-7):Module Type
#define CARD_TYPE1     	 (  (tmpBuf1 & 0x06) >> 1 )
#define CARD_TYPE2     	 ( ((tmpBuf1 & 0x01) << 2) | ((tmpBuf2 & 0xC0) >> 6) ) 
#define CARD_MODULE_TYPE (  (tmpBuf1 & 0xC0) >> 6 )

// tmpBuf2 == Board Address1:: Card Revision, Version and 2 bits of Card Type2
// Bit(0-2):Revision, Bit(3-5):Version, Bit(6-7): LSBs of Card Type2
#define CARD_VERSION  	 ( (tmpBuf2 & 0x38) >> 3)
#define CARD_REVISION  	 (  tmpBuf2 & 0x07)
#define HARDWARE_VERSION ( (CARD_VERSION << 0x03) | CARD_REVISION)	

#define BOARD_ADRRESS_1_DEF     pinMode(ADE1 , OUTPUT) // ADE1 Board Address Enable1
#define BOARD_ADRRESS_2_DEF     pinMode(ADE2  , OUTPUT) // ADE2 Board Address Enable2

#define	BOARD_ADRRESS_1_HIGH    digitalWrite(ADE1, HIGH)    // ADE1 = 1
#define	BOARD_ADRRESS_1_LOW     digitalWrite(ADE1, LOW)     // ADE1 = 0
#define	BOARD_ADRRESS_2_HIGH    digitalWrite(ADE2, HIGH)    // ADE2 = 1
#define	BOARD_ADRRESS_2_LOW     digitalWrite(ADE1, LOW)     // ADE2 = 0

#define	INPUT_DATA_BYTE_DEF     DefINData()                 // D0..D7 is Input
#define	OUTPUT_DATA_BYTE_DEF    DefOUTData()                // D0..D7 is Output
#define	READ_INPUT_DATA_BYTE    ReadINData()               // Input  D0-D7 == PL0-PL7
//#define	SET_OUTPUT_DATA_BYTE    SetOUTData()                // Output D0-D7 == PL0-PL7

/**********************************************************************************
 *  Hardware definitions for DI530 Cards (DI530)
 **********************************************************************************/
#if  CARD_CODE == DI530_CODE
//// timer3 CLK = CLK_io / 8(prescaler)=8MHz/8= 1MHz (1uSec) [CS32 CS31 CS30]=[0 1 0]= 0x02
//	#define SET_TIMER3A_MODE          TCCR3A = 0x00
//	#define SET_TIMER3B_MODE          TCCR3B = (1 << CS31)
//
////  10 usec (10 * 1usec = 10 usec)
//	#define SET_TIMER3H_VALUE         TCNT3H  = 0XFF
//	#define SET_TIMER3L_VALUE         TCNT3L  = 0XFF - 0x7C
//
////  125 usec (125 * 1usec = 125 usec)
//	#define SET_TIMER3H_INIT          TCNT3H  = 0XFF
//	#define SET_TIMER3L_INIT          TCNT3L  = 0xFF - 0x09
//
//    #define  ENABLE_TIMER3_INTERRUPT  TIMSK3 |=  (1 << TOIE3) // timer3 overflow interrupt enable
//    #define DISABLE_TIMER3_INTERRUPT  TIMSK3 &= ~(1 << TOIE3) // timer3 overflow interrupt disable   
//
//	#define SET_TIMER4A_MODE          TCCR4A = 0x00
//	#define SET_TIMER4B_MODE          TCCR4B = (1 << CS31)
//
////  250 usec (250 * 1usec = 250 usec)
//	#define SET_TIMER4H_VALUE         TCNT4H =0xFF//0XFF-1
//	#define SET_TIMER4L_VALUE         TCNT4L =0xFF-0xF9//0XFF-0xF3
//
////  10 usec (10 * 1usec = 10 usec)
//	#define SET_TIMER4H_INIT          TCNT4H  = 0XFF
//	#define SET_TIMER4L_INIT          TCNT4L  = 0xFF - 0x09
//
//    #define  ENABLE_TIMER4_INTERRUPT  TIMSK4 |=  (1 << TOIE4) // timer4 overflow interrupt enable
//    #define DISABLE_TIMER4_INTERRUPT  TIMSK4 &= ~(1 << TOIE4) // timer4 overflow interrupt disable

//    #define	GROUP1_SELECT_DEF   	DDRJ |= (1 << PORTJ2)   // D0-D7,   OE of Latch
//    #define	GROUP2_SELECT_DEF   	DDRJ |= (1 << PORTJ3)   // D8-D15,  OE of Latch
//    #define	GROUP3_SELECT_DEF   	DDRJ |= (1 << PORTJ4)   // D16-D23, OE of Latch
//    #define	GROUP4_SELECT_DEF   	DDRJ |= (1 << PORTJ5)   // D24-D31, OE of Latch
//    #define ALL_SELECT_HIGH         PORTJ|= 0x3C            //DS4..1=1  
//    #define	GROUP1_SELECT_HIGH  	PORTJ |= (1 << PORTJ2)  // D0-D7   DS1 = 1
//    #define	GROUP1_SELECT_LOW   	PORTJ &= ~(1 << PORTJ2) // D0-D7   DS1 = 0
//	#define	GROUP2_SELECT_HIGH  	PORTJ |= (1 << PORTJ3)  // D8-D15  DS2 = 1
//    #define	GROUP2_SELECT_LOW   	PORTJ &= ~(1 << PORTJ3) // D8-D15  DS2 = 0
//	#define	GROUP3_SELECT_HIGH  	PORTJ |= (1 << PORTJ4)  // D16-D23 DS3 = 1
//    #define	GROUP3_SELECT_LOW   	PORTJ &= ~(1 << PORTJ4) // D16-D23 DS3 = 0
//	#define	GROUP4_SELECT_HIGH  	PORTJ |= (1 << PORTJ5)  // D24-D31 DS4 = 1
//    #define	GROUP4_SELECT_LOW   	PORTJ &= ~(1 << PORTJ5) // D24-D31 DS4 = 0
    
//    #define	HIGH_SIDE_SWITCH1_DEF   DDRF |= (1 << DDF3)     // High Side Switch Group1 
//    #define	HIGH_SIDE_SWITCH2_DEF   DDRG |= (1 << DDG3)     // High Side Switch Group2
//    #define	HIGH_SIDE_SWITCH3_DEF   DDRG |= (1 << DDG4)     // High Side Switch Group3
//    #define	HIGH_SIDE_SWITCH4_DEF   DDRE |= (1 << DDE4)     // High Side Switch Group4
//    
//    #define	HIGH_SIDE_SWITCH1_HIGH  PORTF |= (1 << PORTF3)  // HS1 = 1
//    #define	HIGH_SIDE_SWITCH2_HIGH  PORTG |= (1 << PORTG3)  // HS2 = 1
//    #define	HIGH_SIDE_SWITCH3_HIGH  PORTG |= (1 << PORTG4)  // HS3 = 1
//    #define	HIGH_SIDE_SWITCH4_HIGH  PORTE |= (1 << PORTE4)  // HS4 = 1
//    
//    #define	HIGH_SIDE_SWITCH1_LOW   PORTF &= ~(1 << PORTF3) // HS1 = 0
//    #define	HIGH_SIDE_SWITCH2_LOW   PORTG &= ~(1 << PORTG3) // HS2 = 0
//    #define	HIGH_SIDE_SWITCH3_LOW   PORTG &= ~(1 << PORTG4) // HS3 = 0
//    #define	HIGH_SIDE_SWITCH4_LOW   PORTE &= ~(1 << PORTE4) // HS4 = 0
//    
//    #define	LOW_SIDE_SWITCH1_DEF    DDRJ  |= (1 << PORTJ6)  // low Side Switch Group1
//    #define	LOW_SIDE_SWITCH2_DEF    DDRJ  |= (1 << PORTJ7)  // low Side Switch Group2
//    #define	LOW_SIDE_SWITCH3_DEF    DDRE  |= (1 << PORTE7)  // low Side Switch Group3
//    #define	LOW_SIDE_SWITCH4_DEF    DDRE  |= (1 << PORTE5)  // low Side Switch Group4
//    
//    #define	LOW_SIDE_SWITCH1_HIGH   PORTJ |= (1 << PORTJ6)  // LS1 = 1
//    #define	LOW_SIDE_SWITCH2_HIGH   PORTJ |= (1 << PORTJ7)  // LS2 = 1
//    #define	LOW_SIDE_SWITCH3_HIGH   PORTE |= (1 << PORTE7)  // LS3 = 1
//    #define	LOW_SIDE_SWITCH4_HIGH   PORTE |= (1 << PORTE5)  // LS4 = 1
//    
//    #define	LOW_SIDE_SWITCH1_LOW    PORTJ &= ~(1 << PORTJ6) // LS1 = 0
//    #define	LOW_SIDE_SWITCH2_LOW    PORTJ &= ~(1 << PORTJ7) // LS2 = 0
//    #define	LOW_SIDE_SWITCH3_LOW    PORTE &= ~(1 << PORTE7) // LS3 = 0
//    #define	LOW_SIDE_SWITCH4_LOW    PORTE &= ~(1 << PORTE5) // LS4 = 0
//    
//    #define	FIELD_POWER1_DEF        DDRH &= ~(1 << DDH0)    // Field Power Group1 Input
//    #define	FIELD_POWER2_DEF        DDRH &= ~(1 << DDH1)    // Field Power Group2 Input
//    #define	FIELD_POWER3_DEF        DDRG &= ~(1 << DDG5)    // Field Power Group3 Input
//    #define	FIELD_POWER4_DEF        DDRH &= ~(1 << DDH7)    // Field Power Group4 Input
//    
//    #define	FIELD_POWER1_READ      (PINH & (1 << PORTH0))   // Read FPOW1 
//    #define	FIELD_POWER2_READ      (PINH & (1 << PORTH1))   // Read FPOW2 
//    #define	FIELD_POWER3_READ      (PING & (1 << PORTG5))   // Read FPOW3 
//    #define	FIELD_POWER4_READ      (PINH & (1 << PORTH7))   // Read FPOW4 
    
//  NOTE-------------NOTE--------------NOTE: deprecated	
//    #define EXTERNALRAM_BASE    	0x8000
//    #define EXTERNALRAM_END     	0xC000
/**********************************************************************************
 *  Hardware definitions for DI531 Cards (DI531) 
 after changing DI531_CODE you can uncomment this section
 **********************************************************************************/
/*#elif  CARD_CODE == DI531_CODE
// timer3 CLK = CLK_io / 8(prescaler)=8MHz/8= 1MHz (1uSec) [CS32 CS31 CS30]=[0 1 0]= 0x02
	#define SET_TIMER3A_MODE          TCCR3A = 0x00
	#define SET_TIMER3B_MODE          TCCR3B = (1 << CS31)

//  10 usec (10 * 1usec = 10 usec)
	#define SET_TIMER3H_VALUE         TCNT3H  = 0XFF
	#define SET_TIMER3L_VALUE         TCNT3L  = 0XFF - 0x7C

//  125 usec (125 * 1usec = 125 usec)
	#define SET_TIMER3H_INIT          TCNT3H  = 0XFF
	#define SET_TIMER3L_INIT          TCNT3L  = 0xFF - 0x09

    #define  ENABLE_TIMER3_INTERRUPT  TIMSK3 |=  (1 << TOIE3) // timer3 overflow interrupt enable
    #define DISABLE_TIMER3_INTERRUPT  TIMSK3 &= ~(1 << TOIE3) // timer3 overflow interrupt disable   

	#define SET_TIMER4A_MODE          TCCR4A = 0x00
	#define SET_TIMER4B_MODE          TCCR4B = (1 << CS31)

//  250 usec (250 * 1usec = 250 usec)
	#define SET_TIMER4H_VALUE         TCNT4H =0xFF//0XFF-1
	#define SET_TIMER4L_VALUE         TCNT4L =0xFF-0xF9//0XFF-0xF3

//  10 usec (10 * 1usec = 10 usec)
	#define SET_TIMER4H_INIT          TCNT4H  = 0XFF
	#define SET_TIMER4L_INIT          TCNT4L  = 0xFF - 0x09

    #define  ENABLE_TIMER4_INTERRUPT  TIMSK4 |=  (1 << TOIE4) // timer4 overflow interrupt enable
    #define DISABLE_TIMER4_INTERRUPT  TIMSK4 &= ~(1 << TOIE4) // timer4 overflow interrupt disable 
	
    #define	GROUP1_SELECT_DEF   	DDRJ |= (1 << PORTJ2)   // D0-D7,   OE of Latch
    #define	GROUP2_SELECT_DEF   	DDRJ |= (1 << PORTJ3)   // D8-D15,  OE of Latch
    #define	GROUP3_SELECT_DEF   	DDRJ |= (1 << PORTJ4)   // D16-D23, OE of Latch
    #define	GROUP4_SELECT_DEF   	DDRJ |= (1 << PORTJ5)   // D24-D31, OE of Latch
    #define     ALL_SELECT_HIGH         PORTJ|= 0x3C //DS4..1=1  
    #define	GROUP1_SELECT_HIGH  	PORTJ |= (1 << PORTJ2)  // D0-D7   DS1 = 1
    #define	GROUP1_SELECT_LOW   	PORTJ &= ~(1 << PORTJ2) // D0-D7   DS1 = 0
	#define	GROUP2_SELECT_HIGH  	PORTJ |= (1 << PORTJ3)  // D8-D15  DS2 = 1
    #define	GROUP2_SELECT_LOW   	PORTJ &= ~(1 << PORTJ3) // D8-D15  DS2 = 0
	#define	GROUP3_SELECT_HIGH  	PORTJ |= (1 << PORTJ4)  // D16-D23 DS3 = 1
    #define	GROUP3_SELECT_LOW   	PORTJ &= ~(1 << PORTJ4) // D16-D23 DS3 = 0
	#define	GROUP4_SELECT_HIGH  	PORTJ |= (1 << PORTJ5)  // D24-D31 DS4 = 1
    #define	GROUP4_SELECT_LOW   	PORTJ &= ~(1 << PORTJ5) // D24-D31 DS4 = 0
    
    #define	HIGH_SIDE_SWITCH1_DEF   DDRF |= (1 << DDF3) // High Side Switch Group1 
    #define	HIGH_SIDE_SWITCH2_DEF   DDRG |= (1 << DDG3) // High Side Switch Group2
    #define	HIGH_SIDE_SWITCH3_DEF   DDRG |= (1 << DDG4) // High Side Switch Group3
    #define	HIGH_SIDE_SWITCH4_DEF   DDRE |= (1 << DDE4) // High Side Switch Group4
    
    #define	HIGH_SIDE_SWITCH1_HIGH  PORTF |= (1 << PORTF3) // HS1 = 1
    #define	HIGH_SIDE_SWITCH2_HIGH  PORTG |= (1 << PORTG3) // HS2 = 1
    #define	HIGH_SIDE_SWITCH3_HIGH  PORTG |= (1 << PORTG4) // HS3 = 1
    #define	HIGH_SIDE_SWITCH4_HIGH  PORTE |= (1 << PORTE4) // HS4 = 1
    
    #define	HIGH_SIDE_SWITCH1_LOW   PORTF &= ~(1 << PORTF3)// HS1 = 0
    #define	HIGH_SIDE_SWITCH2_LOW   PORTG &= ~(1 << PORTG3)// HS2 = 0
    #define	HIGH_SIDE_SWITCH3_LOW   PORTG &= ~(1 << PORTG4)// HS3 = 0
    #define	HIGH_SIDE_SWITCH4_LOW   PORTE &= ~(1 << PORTE4)// HS4 = 0
    
    #define	LOW_SIDE_SWITCH1_DEF    DDRJ  |= (1 << PORTJ6) // low Side Switch Group1
    #define	LOW_SIDE_SWITCH2_DEF    DDRJ  |= (1 << PORTJ7) // low Side Switch Group2
    #define	LOW_SIDE_SWITCH3_DEF    DDRE  |= (1 << PORTE7) // low Side Switch Group3
    #define	LOW_SIDE_SWITCH4_DEF    DDRE  |= (1 << PORTE5) // low Side Switch Group4
    
    #define	LOW_SIDE_SWITCH1_HIGH   PORTJ |= (1 << PORTJ6) // LS1 = 1
    #define	LOW_SIDE_SWITCH2_HIGH   PORTJ |= (1 << PORTJ7) // LS2 = 1
    #define	LOW_SIDE_SWITCH3_HIGH   PORTE |= (1 << PORTE7) // LS3 = 1
    #define	LOW_SIDE_SWITCH4_HIGH   PORTE |= (1 << PORTE5) // LS4 = 1
    
    #define	LOW_SIDE_SWITCH1_LOW    PORTJ &= ~(1 << PORTJ6)// LS1 = 0
    #define	LOW_SIDE_SWITCH2_LOW    PORTJ &= ~(1 << PORTJ7)// LS2 = 0
    #define	LOW_SIDE_SWITCH3_LOW    PORTE &= ~(1 << PORTE7)// LS3 = 0
    #define	LOW_SIDE_SWITCH4_LOW    PORTE &= ~(1 << PORTE5)// LS4 = 0
    
    #define	FIELD_POWER1_DEF        DDRH &= ~(1 << DDH0)  // Field Power Group1 Input
    #define	FIELD_POWER2_DEF        DDRH &= ~(1 << DDH1)  // Field Power Group2 Input
    #define	FIELD_POWER3_DEF        DDRG &= ~(1 << DDG5)  // Field Power Group3 Input
    #define	FIELD_POWER4_DEF        DDRH &= ~(1 << DDH7)  // Field Power Group4 Input
    
    #define	FIELD_POWER1_READ      (PINH & (1 << PORTH0)) // Read FPOW1 
    #define	FIELD_POWER2_READ      (PINH & (1 << PORTH1)) // Read FPOW2 
    #define	FIELD_POWER3_READ      (PING & (1 << PORTG5)) // Read FPOW3 
    #define	FIELD_POWER4_READ      (PINH & (1 << PORTH7)) // Read FPOW4 
    
//  NOTE-------------NOTE--------------NOTE: deprecated	
//    #define EXTERNALRAM_BASE    	0x8000
//    #define EXTERNALRAM_END     	0xC000*/

/***********************************************************************************
 *  Hardware definitions for DO580 Cards (DO580)
 ***********************************************************************************/
#elif  CARD_CODE == DO580_CODE

    #define	GROUP1_SELECT_DEF   	DDRJ |= (1 << PORTJ2)   // D0-D7,   LE of Latch
    #define	GROUP2_SELECT_DEF   	DDRJ |= (1 << PORTJ3)   // D8-D15,  LE of Latch
    #define	GROUP3_SELECT_DEF   	DDRJ |= (1 << PORTJ4)   // D16-D23, LE of Latch
    #define	GROUP4_SELECT_DEF   	DDRJ |= (1 << PORTJ5)   // D24-D31, LE of Latch

    #define	GROUP1_SELECT_HIGH  	PORTJ |= (1 << PORTJ2)  // D0-D7  , DS1 = 1
    #define	GROUP1_SELECT_LOW   	PORTJ &= ~(1 << PORTJ2) // D0-D7  , DS1 = 0
	#define	GROUP2_SELECT_HIGH  	PORTJ |= (1 << PORTJ3)  // D8-D15 , DS2 = 1
    #define	GROUP2_SELECT_LOW   	PORTJ &= ~(1 << PORTJ3) // D8-D15 , DS2 = 0
	#define	GROUP3_SELECT_HIGH  	PORTJ |= (1 << PORTJ4)  // D16-D23, DS3 = 1
    #define	GROUP3_SELECT_LOW   	PORTJ &= ~(1 << PORTJ4) // D16-D23, DS3 = 0
	#define	GROUP4_SELECT_HIGH  	PORTJ |= (1 << PORTJ5)  // D24-D31, DS4 = 1
    #define	GROUP4_SELECT_LOW   	PORTJ &= ~(1 << PORTJ5) // D24-D31, DS4 = 0

    #define OUTPUT_ENABLE_DEF		DDRH  |= (1 << PORTH0)	// output, OE of Latch
    #define OUTPUT_ENABLE			PORTH &= ~(1 << PORTH0) // OE = 0
    #define OUTPUT_DISABLE			PORTH |= (1 << PORTH0)  // OE = 1

    #define	FIELD_POWER1_DEF    	DDRE &= ~(1 << DDE7) // Field Power Group1 Input
    #define	FIELD_POWER2_DEF    	DDRE &= ~(1 << DDE5) // Field Power Group2 Input
    #define	FIELD_POWER3_DEF    	DDRF &= ~(1 << DDF3) // Field Power Group3 Input
    #define	FIELD_POWER4_DEF   		DDRG &= ~(1 << DDG5) // Field Power Group4 Input
    
    #define	READ_FIELD_POWER1  	   (PINE & (1 << PORTE7)) // Read FPOW1
    #define	READ_FIELD_POWER2  	   (PINE & (1 << PORTE5)) // Read FPOW2
    #define	READ_FIELD_POWER3  	   (PINF & (1 << PORTF3)) // Read FPOW3
    #define	READ_FIELD_POWER4  	   (PING & (1 << PORTG5)) // Read FPOW4

	#define	STATUS1_DEF         	DDRH &= ~(1 << DDH7)  // Group1 Status: Input
    #define	STATUS2_DEF         	DDRG &= ~(1 << DDG4)  // Group2 Status: Input
    #define	STATUS3_DEF         	DDRE &= ~(1 << DDE4)  // Group3 Status: Input
    #define	STATUS4_DEF        		DDRG &= ~(1 << DDG3)  // Group4 Status: Input

//  Read Group1-4 Status over-temperature, short circuit to GND, and under-voltage 
    #define	READ_STATUS1       	   (PINH & (1 << PORTH7)) // Read ST1
    #define	READ_STATUS2           (PING & (1 << PORTG4)) // Read SR2 
    #define	READ_STATUS3           (PINE & (1 << PORTE4)) // Read ST3
    #define	READ_STATUS4           (PING & (1 << PORTG3)) // Read ST4

//  NOTE-------------NOTE--------------NOTE: deprecated	
//    #define EXTERNALRAM_BASE        0x8000
//    #define EXTERNALRAM_END         0xA027
/***********************************************************************************
 *  Hardware definitions for DO580 Cards (DO580)
 ***********************************************************************************/
#elif  CARD_CODE == DO591_CODE

    #define	GROUP1_SELECT_DEF   	DDRJ |= (1 << PORTJ2)   // D0-D7,   LE of Latch
    #define	GROUP2_SELECT_DEF   	DDRJ |= (1 << PORTJ3)   // D8-D15,  LE of Latch
    #define	GROUP3_SELECT_DEF   	DDRJ |= (1 << PORTJ4)   // D16-D23, LE of Latch
    #define	GROUP4_SELECT_DEF   	DDRJ |= (1 << PORTJ5)   // D24-D31, LE of Latch

    #define	GROUP1_SELECT_HIGH  	PORTJ |= (1 << PORTJ2)  // D0-D7  , DS1 = 1
    #define	GROUP1_SELECT_LOW   	PORTJ &= ~(1 << PORTJ2) // D0-D7  , DS1 = 0
	#define	GROUP2_SELECT_HIGH  	PORTJ |= (1 << PORTJ3)  // D8-D15 , DS2 = 1
    #define	GROUP2_SELECT_LOW   	PORTJ &= ~(1 << PORTJ3) // D8-D15 , DS2 = 0
	#define	GROUP3_SELECT_HIGH  	PORTJ |= (1 << PORTJ4)  // D16-D23, DS3 = 1
    #define	GROUP3_SELECT_LOW   	PORTJ &= ~(1 << PORTJ4) // D16-D23, DS3 = 0
	#define	GROUP4_SELECT_HIGH  	PORTJ |= (1 << PORTJ5)  // D24-D31, DS4 = 1
    #define	GROUP4_SELECT_LOW   	PORTJ &= ~(1 << PORTJ5) // D24-D31, DS4 = 0

    #define OUTPUT_ENABLE_DEF		DDRH  |= (1 << PORTH0)	// output, OE of Latch
    #define OUTPUT_ENABLE			PORTH &= ~(1 << PORTH0) // OE = 0
    #define OUTPUT_DISABLE			PORTH |= (1 << PORTH0)  // OE = 1

    #define	FIELD_POWER1_DEF    	DDRE &= ~(1 << DDE7) // Field Power Group1 Input
    #define	FIELD_POWER2_DEF    	DDRE &= ~(1 << DDE5) // Field Power Group2 Input
    #define	FIELD_POWER3_DEF    	DDRF &= ~(1 << DDF3) // Field Power Group3 Input
    #define	FIELD_POWER4_DEF   		DDRG &= ~(1 << DDG5) // Field Power Group4 Input
    
    #define	READ_FIELD_POWER1  	   (PINE & (1 << PORTE7)) // Read FPOW1
    #define	READ_FIELD_POWER2  	   (PINE & (1 << PORTE5)) // Read FPOW2
    #define	READ_FIELD_POWER3  	   (PINF & (1 << PORTF3)) // Read FPOW3
    #define	READ_FIELD_POWER4  	   (PING & (1 << PORTG5)) // Read FPOW4

    #define	STATUS1_DEF         	DDRH &= ~(1 << DDH7)  // Group1 Status: Input
    #define	STATUS2_DEF         	DDRG &= ~(1 << DDG4)  // Group2 Status: Input
    #define	STATUS3_DEF         	DDRE &= ~(1 << DDE4)  // Group3 Status: Input
    #define	STATUS4_DEF        		DDRG &= ~(1 << DDG3)  // Group4 Status: Input

//  Read Group1-4 Status over-temperature, short circuit to GND, and under-voltage 
    #define	READ_STATUS1       	   (PINH & (1 << PORTH7)) // Read ST1
    #define	READ_STATUS2           (PING & (1 << PORTG4)) // Read SR2 
    #define	READ_STATUS3           (PINE & (1 << PORTE4)) // Read ST3
    #define	READ_STATUS4           (PING & (1 << PORTG3)) // Read ST4

//  NOTE-------------NOTE--------------NOTE: deprecated	
//    #define EXTERNALRAM_BASE        0x8000
//    #define EXTERNALRAM_END         0xA027
#elif  CARD_CODE == DIO550_CODE
//// timer3 CLK = CLK_io / 8(prescaler)=8MHz/8= 1MHz (1uSec) [CS32 CS31 CS30]=[0 1 0]= 0x02
//	#define SET_TIMER3A_MODE          TCCR3A = 0x00
//	#define SET_TIMER3B_MODE          TCCR3B = (1 << CS31)
//
////  10 usec (10 * 1usec = 10 usec)
//	#define SET_TIMER3H_VALUE         TCNT3H  = 0XFF
//	#define SET_TIMER3L_VALUE         TCNT3L  = 0XFF - 0x7C
//
////  125 usec (125 * 1usec = 125 usec)
//	#define SET_TIMER3H_INIT          TCNT3H  = 0XFF
//	#define SET_TIMER3L_INIT          TCNT3L  = 0xFF - 0x09
//
//    #define  ENABLE_TIMER3_INTERRUPT  TIMSK3 |=  (1 << TOIE3) // timer3 overflow interrupt enable
//    #define DISABLE_TIMER3_INTERRUPT  TIMSK3 &= ~(1 << TOIE3) // timer3 overflow interrupt disable   
//
//	#define SET_TIMER4A_MODE          TCCR4A = 0x00
//	#define SET_TIMER4B_MODE          TCCR4B = (1 << CS31)
//
////  250 usec (250 * 1usec = 250 usec)
//	#define SET_TIMER4H_VALUE         TCNT4H =0xFF//0XFF-1
//	#define SET_TIMER4L_VALUE         TCNT4L =0xFF-0xF9//0XFF-0xF3
//
////  10 usec (10 * 1usec = 10 usec)
//	#define SET_TIMER4H_INIT          TCNT4H  = 0XFF
//	#define SET_TIMER4L_INIT          TCNT4L  = 0xFF - 0x09
//
//    #define  ENABLE_TIMER4_INTERRUPT  TIMSK4 |=  (1 << TOIE4) // timer4 overflow interrupt enable
//    #define DISABLE_TIMER4_INTERRUPT  TIMSK4 &= ~(1 << TOIE4) // timer4 overflow interrupt disable 

    #define	GROUP1_SELECT_IN_DEF    pinMode(INPUT_CS_1 , OUTPUT)      // I1-I8,   OE of Latch
    #define	GROUP2_SELECT_IN_DEF    pinMode(INPUT_CS_2 , OUTPUT)      // I9-I16,  OE of Latch
//    #define	GROUP3_SELECT_IN_DEF    DDRE  |= (1 << PORTE5)  // D16-D23, OE of Latch
//    #define	GROUP4_SELECT_IN_DEF    DDRE  |= (1 << PORTE7)  // D24-D31, OE of Latch

    #define	GROUP1_SELECT_IN_HIGH  	digitalWrite(INPUT_CS_1, HIGH)   // I1-D8    INPUT_CS_1 = 1
    #define	GROUP1_SELECT_IN_LOW   	digitalWrite(INPUT_CS_1, LOW)    // I1-D8    INPUT_CS_1 = 0
    #define	GROUP2_SELECT_IN_HIGH  	digitalWrite(INPUT_CS_2, HIGH)   // I9-I16   INPUT_CS_2 = 1
    #define	GROUP2_SELECT_IN_LOW   	digitalWrite(INPUT_CS_2, LOW)    // I9-I16   INPUT_CS_2 = 0
//    #define	GROUP3_SELECT_IN_HIGH  	PORTE |= (1 << PORTE5)  // D16-D23 DS3 = 1
//    #define	GROUP3_SELECT_IN_LOW   	PORTE &= ~(1 << PORTE5) // D16-D23 DS3 = 0
//    #define	GROUP4_SELECT_IN_HIGH  	PORTE |= (1 << PORTE7)  // D24-D31 DS4 = 1
//    #define	GROUP4_SELECT_IN_LOW   	PORTE &= ~(1 << PORTE7) // D24-D31 DS4 = 0    
// 
    #define	GROUP1_SELECT_OUT_DEF   pinMode(OUTPUT_CS , OUTPUT)      // I1-I8,   OE of Latch
//    #define	GROUP2_SELECT_OUT_DEF   DDRJ |= (1 << DDJ7) // D8-D15,  OE of Latch

    #define	GROUP1_SELECT_OUT_HIGH  digitalWrite(OUTPUT_CS, HIGH)    // D0-D7   DS1 = 1
    #define	GROUP1_SELECT_OUT_LOW   digitalWrite(OUTPUT_CS, LOW)     // D0-D7   DS1 = 0
//    #define	GROUP2_SELECT_OUT_HIGH  	PORTJ |= (1 << PORTJ7)  // D8-D15  DS2 = 1
//    #define	GROUP2_SELECT_OUT_LOW   	PORTJ &= ~(1 << PORTJ7) // D8-D15  DS2 = 0

//    #define	SIDE_SWITCH_CS_DEF   pinMode(OUTPUT_CS , OUTPUT) // Side Switch Group Chip Select
//    #define	SIDE_SWITCH_OE_DEF   pinMode(OE , OUTPUT) // Side Switch Group Output Enable
//    
//    #define	SIDE_SWITCH_CS_HIGH  digitalWrite(OUTPUT_CS, HIGH) // Side Switch Group Chip Select = 1
//    #define	SIDE_SWITCH_CS_LOW   digitalWrite(OUTPUT_CS, LOW)// Side Switch Group Chip Select = 0
//    #define	SIDE_SWITCH_OE_HIGH  digitalWrite(OE, HIGH) // Side Switch Group Output Enable = 1
//    #define	SIDE_SWITCH_OE_LOW   digitalWrite(OE, LOW)// Side Switch Group Output Enable = 0

    #define	FIELD_POWER4_DEF     pinMode(PIN_Base_560 + FPOW_OUTPUTS_FLAG_1  , INPUT)    // Field Power Group1 Input
    #define	FIELD_POWER3_DEF     pinMode(PIN_Base_560 + FPOW_OUTPUTS_FLAG_2  , INPUT)    // Field Power Group2 Input
    #define	FIELD_POWER1_DEF     pinMode(PIN_Base_560 + FPOW_INPUTS_FLAG_1   , INPUT)    // Field Power Group1 Output
    #define	FIELD_POWER2_DEF   	 pinMode(PIN_Base_560 + FPOW_INPUTS_FLAG_2   , INPUT)    // Field Power Group2 Output
    
    #define	FIELD_POWER4_READ    digitalRead(PIN_Base_560 + FPOW_OUTPUTS_FLAG_1)      // Field Power Group1 Input 
    #define	FIELD_POWER3_READ    digitalRead(PIN_Base_560 + FPOW_OUTPUTS_FLAG_2)      // Field Power Group2 Input 
    #define	FIELD_POWER1_READ  	 digitalRead(PIN_Base_560 + FPOW_INPUTS_FLAG_1)       // Field Power Group1 Output
    #define	FIELD_POWER2_READ  	 digitalRead(PIN_Base_560 + FPOW_INPUTS_FLAG_2)       // Field Power Group2 Output

    #define OUTPUT_ENABLE_DEF    pinMode(OE , OUTPUT)	    // output, OE of Latch
    #define OUTPUT_ENABLE	     digitalWrite(OE, LOW)     // OE = 0
    #define OUTPUT_DISABLE		 digitalWrite(OE, HIGH)      // OE = 1  

    #define	STATUS1_DEF          pinMode(OUTPUT_STATUS  , INPUT)        // Group1 Status
//    #define	STATUS2_DEF         	DDRH &= ~(1 << DDH7)        // Group2 Status

//  Read Group1-4 Status over-temperature, short circuit to GND, and under-voltage 
    #define	READ_STATUS1         digitalRead(OUTPUT_STATUS)       // Read OUTPUT_STATUS
//    #define	READ_STATUS2            (PINH & (1 << PORTH7))       // Read ST1

//  NOTE-------------NOTE--------------NOTE: deprecated	
//    #define EXTERNALRAM_BASE        0x8000
//    #define EXTERNALRAM_END         0xA027
/***********************************************************************************
 *  Hardware definitions for DO571 Cards (DO571)
 ***********************************************************************************/
#elif  CARD_CODE == DO571_CODE

    #define	GROUP1_SELECT_DEF       DDRJ |= (1 << PORTJ4)   // D0-D7,  LE Data-Latch
    #define	GROUP2_SELECT_DEF       DDRJ |= (1 << PORTJ5)   // D8-D15, LE Data-Latch
    
    #define	GROUP1_SELECT_HIGH      PORTJ |= (1 << PORTJ4)  // D0-D7 ,  LS1 = 1
    #define	GROUP1_SELECT_LOW       PORTJ &= ~(1 << PORTJ4) // D0-D7 ,  LS1 = 0
    #define	GROUP2_SELECT_HIGH      PORTJ |= (1 << PORTJ5)  // D8-D15,  LS2 = 1
    #define	GROUP2_SELECT_LOW       PORTJ &= ~(1 << PORTJ5) // D8-D15,  LS2 = 0

    #define	GROUP1_INPUT_DEF       	DDRJ |= (1 << PORTJ2)   // D0-D7,  OE of Diagnos
	#define	GROUP2_INPUT_DEF       	DDRJ |= (1 << PORTJ3)   // D8-D15, OE of Diagnos
    
    #define	GROUP1_INPUT_HIGH      	PORTJ |= (1 << PORTJ2)  // Diag D0-D7 , DS1 = 1
    #define	GROUP1_INPUT_LOW       	PORTJ &= ~(1 << PORTJ2) // Diag D0-D7 , DS1 = 0
    #define	GROUP2_INPUT_HIGH      	PORTJ |= (1 << PORTJ3)  // Diag D8-D15, DS2 = 1
    #define	GROUP2_INPUT_LOW       	PORTJ &= ~(1 << PORTJ3) // Diag D8-D15, DS2 = 0

    #define OUTPUT_ENABLE_DEF		DDRE  |= (1 << PORTE7)	// output, OE Data-Latch
    #define OUTPUT_ENABLE			PORTE &= ~(1 << PORTE7) // OE = 0
    #define OUTPUT_DISABLE			PORTE |= (1 << PORTE7)  // OE = 1

    #define	FIELD_POWER1_DEF        DDRH &= ~(1 << DDH0)  // Field Power Group1 Input
    #define	FIELD_POWER2_DEF        DDRH &= ~(1 << DDH1)  // Field Power Group2 Input
    
    #define	READ_FIELD_POWER1      (PINH & (1 << PORTH0)) // Read FPOW1
    #define	READ_FIELD_POWER2      (PINH & (1 << PORTH1)) // Read FPOW2

    #define LED_ENABLE_DEF      	DDRF  |= (1 << DDF3)    // OE Latch-Alarm LED
    #define LED_ENABLE_HIGH     	PORTF |= (1 << PORTF3)  // LED_OE = 1
    #define LED_ENABLE_LOW      	PORTF &= ~(1 << PORTF3) // LED_OE = 0

    #define LED_LATCH1_DEF       	DDRJ  |= (1 << DDJ6)    // LE Latch1-Alarm LED
    #define LED_LATCH1_HIGH      	PORTJ |= (1 << PORTJ6)  // LED_LE1 = 1
    #define LED_LATCH1_LOW      	PORTJ &= ~(1 << PORTJ6) // LED_LE1 = 0

    #define LED_LATCH2_DEF       	DDRJ  |= (1 << DDJ7)    // LE Latch2-Alarm LED
    #define LED_LATCH2_HIGH      	PORTJ |= (1 << PORTJ7)  // LED_LE2 = 1
    #define LED_LATCH2_LOW       	PORTJ &= ~(1 << PORTJ7) // LED_LE2 = 0

//  NOTE-------------NOTE--------------NOTE: deprecated	
//    #define EXTERNALRAM_BASE        0x8000
//    #define EXTERNALRAM_END         0xA027
/***********************************************************************************
 *  Hardware definitions for AI624 Cards (AI624-634)
 ***********************************************************************************/
#elif  CARD_CODE == AI624_CODE

    #define LED_ENABLE_DEF      DDRK  |= (1 << DDK0)    // OE Latch-Alarm LED
    #define LED_ENABLE_HIGH     PORTK |= (1 << PORTK0)  // LED_OE = 1
    #define LED_ENABLE_LOW      PORTK &= ~(1 << PORTK0) // LED_OE = 0

    #define LED_LATCH_DEF       DDRK  |= (1 << DDK2)    // LE Latch-Alarm LED
    #define LED_LATCH_HIGH      PORTK |= (1 << PORTK2)  // LED_LE = 1
    #define LED_LATCH_LOW       PORTK &= ~(1 << PORTK2) // LED_LE = 0

    #define SHUNT1_DEF			DDRF  |= (1 << DDF3)	// Solid State Relay Command
    #define SHUNT1_HIGH			PORTF |= (1 << PORTF3)	// AP-OE1 = 1
    #define SHUNT1_LOW			PORTF &= ~(1 << PORTF3)	// AP-OE1 = 0
    
    #define SINGLE_END_DEF  	DDRK  |= (1 << DDK5)	// Single ended is used 
    #define SINGLE_END_HIGH		PORTK |= (1 << PORTK5)	// NC
    #define SINGLE_END_LOW		PORTK &= ~(1 << PORTK5)	// NC 
    
    #define CURRENT_TEST_DEF    DDRK  |= (1 << DDK4)    // Current Test Pin
    #define CURRENT_TEST_LOW    PORTK |= (1 << PORTK4) 	// NC 
    #define CURRENT_TEST_HIGH   PORTK &= ~(1 << PORTK4) // NC 
    
	#define MUXAD0_DEF			DDRJ |= (1 << DDJ2)		// 8-in Channel MUX Address0
    #define MUXAD1_DEF			DDRJ |= (1 << DDJ3)		// 8-in Channel MUX Address1
    #define MUXAD2_DEF			DDRJ |= (1 << DDJ4)		// 8-in Channel MUX Address2
    
    #define MUXAD0_HIGH			PORTJ |= (1 << PORTJ2)	// CH-AD0 = 1
    #define MUXAD0_LOW			PORTJ &= ~(1 << PORTJ2)	// CH-AD0 = 0
    #define MUXAD1_HIGH			PORTJ |= (1 << PORTJ3)	// CH-AD1 = 1
    #define MUXAD1_LOW			PORTJ &= ~(1 << PORTJ3)	// CH-AD1 = 0 
    #define MUXAD2_HIGH			PORTJ |= (1 << PORTJ4)	// CH-AD2 = 1
    #define MUXAD2_LOW			PORTJ &= ~(1 << PORTJ4)	// CH-AD2 = 0 
 
	#define SET_TIMER4A_MODE          TCCR4A = 0x00
	#define SET_TIMER4B_MODE          TCCR4B = (1 << CS31)

//  1250 usec (1250 * 1usec = 1250 usec)
	#define SET_TIMER4H_VALUE         TCNT4H =0xFF-4//0XFF-1
	#define SET_TIMER4L_VALUE         TCNT4L =0xFF-0xE1//0XFF-0xF3

//  10 usec (10 * 1usec = 10 usec)
	#define SET_TIMER4H_INIT          TCNT4H  = 0XFF-4
	#define SET_TIMER4L_INIT          TCNT4L =0xFF-0xE1//0XFF-0xF3//TCNT4L  = 0xFF - 0x09

    #define  ENABLE_TIMER4_INTERRUPT  TIMSK4 |=  (1 << TOIE4) // timer4 overflow interrupt enable
    #define DISABLE_TIMER4_INTERRUPT  TIMSK4 &= ~(1 << TOIE4) // timer4 overflow interrupt disable
	#define ANALOGPOWER_DEF	 	DDRE &= ~(1 << DDE5)  	// Alarm/Rest voltage monitor
    #define ANALOGPOWER_READ   (PINE & (1 << PORTE5)) 	// ARST_IRQ

	#define FIELDPOWER_DEF	 	DDRG &= ~(1 << DDG5)  	// Field Power input
    #define FIELDPOWER_READ	   (PING & (1 << PORTG5)) 	// FPOW 
    
    #define POWER_RST_DEF    	DDRK |= (1 << DDK1)    	// En of DC-DC Converter
    #define POWER_RST_HIGH   	PORTK |= (1 << PORTK1) 	// Power_RST = 1
    #define POWER_RST_LOW    	PORTK &= ~(1 << PORTK1)	// Power_RST = 0
    
    //HART modem Definition
    #define INRTS_DEF   		DDRK |= (1 << DDK3)     // Request To Send of HART
    #define INRTS_HIGH  		PORTK |= (1 << PORTK3)	// INRTS-GPIO = 1
    #define INRTS_LOW   		PORTK &= ~(1 << PORTK3)	// INRTS-GPIO = 0
	
	#define OCD_DEF     		DDRE &= ~(1 << DDE7)    // Carrier Detect input
    #define OCD_RD		 	   (PINE & (1 << PORTE7))	// Read OCD_IRQ
    
    //HART Address channel Definition
    #define HART_ADDR0_DEF   	DDRJ |= (1 << DDJ5)     // 8-in Channel HART MUX Addr0
    #define HART_ADDR1_DEF   	DDRJ |= (1 << DDJ6)     // 8-in Channel HART MUX Addr1
    #define HART_ADDR2_DEF   	DDRJ |= (1 << DDJ7)     // 8-in Channel HART MUX Addr2
    
	#define HART_ADDR0_HIGH   	PORTJ |= (1 << PORTJ5)  // HART_CH-AD0 = 1
    #define HART_ADDR0_LOW    	PORTJ &= ~(1 << PORTJ5) // HART_CH-AD0 = 0
    #define HART_ADDR1_HIGH  	PORTJ |= (1 << PORTJ6)  // HART_CH-AD1 = 1
    #define HART_ADDR1_LOW    	PORTJ &= ~(1 << PORTJ6) // HART_CH-AD1 = 0
    #define HART_ADDR2_HIGH   	PORTJ |= (1 << PORTJ7)  // HART_CH-AD2 = 1
    #define HART_ADDR2_LOW    	PORTJ &= ~(1 << PORTJ7) // HART_CH-AD2 = 0
    
    //UART Interrupt vectors definition
    #define USART2_RXC_INT    	USART2_RX_vect // HART RX UART interrupt vector
    #define USART2_TXC_INT    	USART2_TX_vect // not used

//  NOTE-------------NOTE--------------NOTE: deprecated	
//    #define EXTERNALRAM_BASE  	0x8000
//    #define EXTERNALRAM_END   	0xA027

/***********************************************************************************
 *  Hardware definitions for AI623 Cards (AI622-623)
 ***********************************************************************************/
#elif  CARD_CODE == AI623_CODE

    #define LED_ENABLE_DEF      DDRK  |= (1 << DDK0)    // OE Latch-Alarm LED
    #define LED_ENABLE_HIGH     PORTK |= (1 << PORTK0)  // LED_OE = 1
    #define LED_ENABLE_LOW      PORTK &= ~(1 << PORTK0) // LED_OE = 0

    #define LED_LATCH_DEF       DDRK  |= (1 << DDK2)    // LE Latch-Alarm LED
    #define LED_LATCH_HIGH      PORTK |= (1 << PORTK2)  // LED_LE = 1
    #define LED_LATCH_LOW       PORTK &= ~(1 << PORTK2) // LED_LE = 0

    #define SHUNT1_DEF			DDRF  |= (1 << DDF3)	// Solid State Relay Command
    #define SHUNT1_HIGH			PORTF |= (1 << PORTF3)	// AP-OE1 = 1
    #define SHUNT1_LOW			PORTF &= ~(1 << PORTF3)	// AP-OE1 = 0
    
    #define SINGLE_END_DEF  	DDRK  |= (1 << DDK5)	// Single ended is used 
    #define SINGLE_END_HIGH		PORTK |= (1 << PORTK5)	// NC
    #define SINGLE_END_LOW		PORTK &= ~(1 << PORTK5)	// NC 
    
    #define CURRENT_TEST_DEF    DDRK  |= (1 << DDK4)    // Current Test Pin
    #define CURRENT_TEST_LOW    PORTK |= (1 << PORTK4) 	// NC 
    #define CURRENT_TEST_HIGH   PORTK &= ~(1 << PORTK4) // NC 
    
	#define MUXAD0_DEF			DDRJ |= (1 << DDJ2)		// 8-in Channel MUX Address0
    #define MUXAD1_DEF			DDRJ |= (1 << DDJ3)		// 8-in Channel MUX Address1
    #define MUXAD2_DEF			DDRJ |= (1 << DDJ4)		// 8-in Channel MUX Address2
    
    #define MUXAD0_HIGH			PORTJ |= (1 << PORTJ2)	// CH-AD0 = 1
    #define MUXAD0_LOW			PORTJ &= ~(1 << PORTJ2)	// CH-AD0 = 0
    #define MUXAD1_HIGH			PORTJ |= (1 << PORTJ3)	// CH-AD1 = 1
    #define MUXAD1_LOW			PORTJ &= ~(1 << PORTJ3)	// CH-AD1 = 0 
    #define MUXAD2_HIGH			PORTJ |= (1 << PORTJ4)	// CH-AD2 = 1
    #define MUXAD2_LOW			PORTJ &= ~(1 << PORTJ4)	// CH-AD2 = 0 
 
	#define SET_TIMER4A_MODE          TCCR4A = 0x00
	#define SET_TIMER4B_MODE          TCCR4B = (1 << CS31)

//  1250 usec (1250 * 1usec = 1250 usec)
	#define SET_TIMER4H_VALUE         TCNT4H =0xFF-4//0XFF-1
	#define SET_TIMER4L_VALUE         TCNT4L =0xFF-0xE1//0XFF-0xF3

//  10 usec (10 * 1usec = 10 usec)
	#define SET_TIMER4H_INIT          TCNT4H  = 0XFF-4
	#define SET_TIMER4L_INIT          TCNT4L =0xFF-0xE1//0XFF-0xF3//TCNT4L  = 0xFF - 0x09

    #define  ENABLE_TIMER4_INTERRUPT  TIMSK4 |=  (1 << TOIE4) // timer4 overflow interrupt enable
    #define DISABLE_TIMER4_INTERRUPT  TIMSK4 &= ~(1 << TOIE4) // timer4 overflow interrupt disable
	#define ANALOGPOWER_DEF	 	DDRE &= ~(1 << DDE5)  	// Alarm/Rest voltage monitor
    #define ANALOGPOWER_READ   (PINE & (1 << PORTE5)) 	// ARST_IRQ

	#define FIELDPOWER_DEF	 	DDRG &= ~(1 << DDG5)  	// Field Power input
    #define FIELDPOWER_READ	   (PING & (1 << PORTG5)) 	// FPOW 
    
    #define POWER_RST_DEF    	DDRK |= (1 << DDK1)    	// En of DC-DC Converter
    #define POWER_RST_HIGH   	PORTK |= (1 << PORTK1) 	// Power_RST = 1
    #define POWER_RST_LOW    	PORTK &= ~(1 << PORTK1)	// Power_RST = 0
    
    //HART modem Definition
    #define INRTS_DEF   		DDRK |= (1 << DDK3)     // Request To Send of HART
    #define INRTS_HIGH  		PORTK |= (1 << PORTK3)	// INRTS-GPIO = 1
    #define INRTS_LOW   		PORTK &= ~(1 << PORTK3)	// INRTS-GPIO = 0
	
	#define OCD_DEF     		DDRE &= ~(1 << DDE7)    // Carrier Detect input
    #define OCD_RD		 	   (PINE & (1 << PORTE7))	// Read OCD_IRQ
    
    //HART Address channel Definition
    #define HART_ADDR0_DEF   	DDRJ |= (1 << DDJ5)     // 8-in Channel HART MUX Addr0
    #define HART_ADDR1_DEF   	DDRJ |= (1 << DDJ6)     // 8-in Channel HART MUX Addr1
    #define HART_ADDR2_DEF   	DDRJ |= (1 << DDJ7)     // 8-in Channel HART MUX Addr2
    
	#define HART_ADDR0_HIGH   	PORTJ |= (1 << PORTJ5)  // HART_CH-AD0 = 1
    #define HART_ADDR0_LOW    	PORTJ &= ~(1 << PORTJ5) // HART_CH-AD0 = 0
    #define HART_ADDR1_HIGH  	PORTJ |= (1 << PORTJ6)  // HART_CH-AD1 = 1
    #define HART_ADDR1_LOW    	PORTJ &= ~(1 << PORTJ6) // HART_CH-AD1 = 0
    #define HART_ADDR2_HIGH   	PORTJ |= (1 << PORTJ7)  // HART_CH-AD2 = 1
    #define HART_ADDR2_LOW    	PORTJ &= ~(1 << PORTJ7) // HART_CH-AD2 = 0
    
    //UART Interrupt vectors definition
    #define USART2_RXC_INT    	USART2_RX_vect // HART RX UART interrupt vector
    #define USART2_TXC_INT    	USART2_TX_vect // not used

//  NOTE-------------NOTE--------------NOTE: deprecated	
//    #define EXTERNALRAM_BASE  	0x8000
//    #define EXTERNALRAM_END   	0xA027

/***********************************************************************************
 *  Hardware definitions for AO673 Cards (AO672-673)
 ***********************************************************************************/
#elif  CARD_CODE == AO673_CODE

    #define LED_ENABLE_DEF      DDRK  |= (1 << DDK0)    // OE Latch-Alarm LED
    #define LED_ENABLE_HIGH     PORTK |= (1 << PORTK0)  // LED_OE = 1
    #define LED_ENABLE_LOW      PORTK &= ~(1 << PORTK0) // LED_OE = 0

    #define LED_LATCH_DEF       DDRK  |= (1 << DDK2)    // LE Latch-Alarm LED
    #define LED_LATCH_HIGH      PORTK |= (1 << PORTK2)  // LED_LE = 1
    #define LED_LATCH_LOW       PORTK &= ~(1 << PORTK2) // LED_LE = 0

	#define STATUS_READ_DEF     DDRJ  |= (1 << DDJ2)    // OE Latch-Output Failure
    #define STATUS_READ_HIGH    PORTJ |= (1 << PORTJ2)  // STE = 1
    #define STATUS_READ_LOW     PORTJ &= ~(1 << PORTJ2) // STE = 0
														// voltage2current converter	
	#define OUTPUTEN_DEF		DDRF  |= (1 << DDF3)	// Output Disable (XTR111)
    #define DISABLE_OUTPUTS		PORTF |= (1 << PORTF3)	// AP-OE = 1
    #define ENABLE_OUTPUTS		PORTF &= ~(1 << PORTF3)	// AP-OE = 0
	
	#define ANALOGPOWER_DEF	 	DDRE &= ~(1 << DDE5)  	// Alarm/Rest voltage monitor
    #define ANALOGPOWER_READ   (PINE & (1 << PORTE5)) 	// ARST_IRQ

	#define FIELDPOWER_DEF	 	DDRG &= ~(1 << DDG5)  	// Field Power input
    #define FIELDPOWER_READ	   (PING & (1 << PORTG5)) 	// FPOW  
	
    #define POWER_RST_DEF    	DDRK |= (1 << DDK1)    	// En of DC-DC Converter
    #define POWER_RST_HIGH   	PORTK |= (1 << PORTK1) 	// Power_RST = 1
    #define POWER_RST_LOW    	PORTK &= ~(1 << PORTK1)	// Power_RST = 0
  
	#if ModuleTypeSelection == 0
    
		//HART modem Definition
		#define INRTS_DEF   	DDRK |= (1 << DDK3)     // Request To Send of HART
		#define INRTS_HIGH  	PORTK |= (1 << PORTK3)	// INRTS-GPIO = 1
		#define INRTS_LOW   	PORTK &= ~(1 << PORTK3)	// INRTS-GPIO = 0
		
		#define OCD_DEF     	DDRE &= ~(1 << DDE7)    // Carrier Detect input
		#define OCD_RD		   (PINE & (1 << PORTE7))	// Read OCD_IRQ
		
		//HART Address channel Definition
		#define HART_ADDR0_DEF  DDRJ |= (1 << DDJ5) 	//8-in Channel HART MUX Addr0
		#define HART_ADDR1_DEF  DDRJ |= (1 << DDJ6) 	//8-in Channel HART MUX Addr1
		#define HART_ADDR2_DEF  DDRJ |= (1 << DDJ7) 	//8-in Channel HART MUX Addr2
		
		#define HART_ADDR0_HIGH PORTJ |= (1 << PORTJ5)  // HART_CH-AD0 = 1
		#define HART_ADDR0_LOW  PORTJ &= ~(1 << PORTJ5) // HART_CH-AD0 = 0
		#define HART_ADDR1_HIGH PORTJ |= (1 << PORTJ6)  // HART_CH-AD1 = 1
		#define HART_ADDR1_LOW  PORTJ &= ~(1 << PORTJ6) // HART_CH-AD1 = 0
		#define HART_ADDR2_HIGH PORTJ |= (1 << PORTJ7)  // HART_CH-AD2 = 1
		#define HART_ADDR2_LOW  PORTJ &= ~(1 << PORTJ7) // HART_CH-AD2 = 0
	
		//UART Interrupt vectors definition
		#define USART2_RXC_INT  USART2_RX_vect // HART RX UART interrupt vector
		#define USART2_TXC_INT  USART2_TX_vect // not used
    #endif

//  NOTE-------------NOTE--------------NOTE: deprecated	
//    #define EXTERNALRAM_BASE    0x8000
//    #define EXTERNALRAM_END     0xA027

/***********************************************************************************
 *  Hardware definitions for AI622 Cards
 ***********************************************************************************/
#elif  CARD_CODE == AI622_CODE

    #define LED_ENABLE_DEF       DDRK  |= (1 << DDK0)    // OE Latch-Alarm LED
    #define LED_ENABLE_HIGH      PORTK |= (1 << PORTK0)  // LED_OE = 1
    #define LED_ENABLE_LOW       PORTK &= ~(1 << PORTK0) // LED_OE = 0

    #define LED_LATCH_DEF        DDRK  |= (1 << DDK2)    // LE Latch-Alarm LED
    #define LED_LATCH_HIGH       PORTK |= (1 << PORTK2)  // LED_LE = 1
    #define LED_LATCH_LOW        PORTK &= ~(1 << PORTK2) // LED_LE = 0

    #define SHUNT1_DEF			 DDRF  |= (1 << DDF3)	 // Solid State Relay Command
    #define SHUNT1_HIGH			 PORTF |= (1 << PORTF3)	 // AP-OE1 = 1
    #define SHUNT1_LOW			 PORTF &= ~(1 << PORTF3) // AP-OE1 = 0
    
    #define SINGLE_END_DEF  	 DDRK  |= (1 << DDK5)	 // Single ended is used 
    #define SINGLE_END_HIGH		 PORTK |= (1 << PORTK5)	 // NC 
    #define SINGLE_END_LOW		 PORTK &= ~(1 << PORTK5) // NC
    
    #define CURRENT_TEST_DEF     DDRK  |= (1 << DDK4)    // Current Test Pin 
    #define CURRENT_TEST_LOW     PORTK |= (1 << PORTK4)  // NC 
    #define CURRENT_TEST_HIGH    PORTK &= ~(1 << PORTK4) // NC 
	
    #define MUXAD0_DEF			 DDRJ |= (1 << DDJ2)	// 8-in Channel MUX Address0
    #define MUXAD1_DEF			 DDRJ |= (1 << DDJ3)	// 8-in Channel MUX Address1
    #define MUXAD2_DEF			 DDRJ |= (1 << DDJ4)	// 8-in Channel MUX Address2
    
    #define MUXAD0_HIGH			 PORTJ |= (1 << PORTJ2)	// CH-AD0 = 1
    #define MUXAD0_LOW			 PORTJ &= ~(1 << PORTJ2)// CH-AD0 = 0
    #define MUXAD1_HIGH			 PORTJ |= (1 << PORTJ3)	// CH-AD1 = 1
    #define MUXAD1_LOW			 PORTJ &= ~(1 << PORTJ3)// CH-AD1 = 0 
    #define MUXAD2_HIGH			 PORTJ |= (1 << PORTJ4)	// CH-AD2 = 1
    #define MUXAD2_LOW			 PORTJ &= ~(1 << PORTJ4)// CH-AD2 = 0 

	#define SET_TIMER4A_MODE          TCCR4A = 0x00
	#define SET_TIMER4B_MODE          TCCR4B = (1 << CS31)

//  1250 usec (1250 * 1usec = 1250 usec)
	#define SET_TIMER4H_VALUE         TCNT4H =0xFF-4//0XFF-1
	#define SET_TIMER4L_VALUE         TCNT4L =0xFF-0xE1//0XFF-0xF3

//  10 usec (10 * 1usec = 10 usec)
	#define SET_TIMER4H_INIT          TCNT4H  = 0XFF-4
	#define SET_TIMER4L_INIT          TCNT4L =0xFF-0xE1//0XFF-0xF3//TCNT4L  = 0xFF - 0x09

    #define  ENABLE_TIMER4_INTERRUPT  TIMSK4 |=  (1 << TOIE4) // timer4 overflow interrupt enable
    #define DISABLE_TIMER4_INTERRUPT  TIMSK4 &= ~(1 << TOIE4) // timer4 overflow interrupt disable

	#define ANALOGPOWER_DEF	 	 DDRE &= ~(1 << DDE5)  	// Alarm/Rest voltage monitor
    #define ANALOGPOWER_READ    (PINE & (1 << PORTE5)) 	// ARST_IRQ

	#define FIELDPOWER_DEF	 	 DDRG &= ~(1 << DDG5)  	// Field Power input
    #define FIELDPOWER_READ	    (PING & (1 << PORTG5)) 	// FPOW 
    
    #define POWER_RST_DEF    	 DDRK |= (1 << DDK1)    // En of DC-DC Converter
    #define POWER_RST_HIGH   	 PORTK |= (1 << PORTK1) // Power_RST = 1
    #define POWER_RST_LOW    	 PORTK &= ~(1 << PORTK1)// Power_RST = 0

    #if ModuleTypeSelection == 0

		//HART modem Definition
		#define INRTS_DEF   	 DDRK |= (1 << DDK3)    // Request To Send of HART
		#define INRTS_HIGH  	 PORTK |= (1 << PORTK3)	// INRTS-GPIO = 1
		#define INRTS_LOW   	 PORTK &= ~(1 << PORTK3)// INRTS-GPIO = 0
		
		#define OCD_DEF     	 DDRE &= ~(1 << DDE7)   // Carrier Detect input
		#define OCD_RD		    (PINE & (1 << PORTE7))	// Read OCD_IRQ
		
		//HART Address channel Definition
		#define HART_ADDR0_DEF   DDRJ |= (1 << DDJ5) 	//8-in Channel HART MUX Addr0
		#define HART_ADDR1_DEF   DDRJ |= (1 << DDJ6) 	//8-in Channel HART MUX Addr1
		#define HART_ADDR2_DEF   DDRJ |= (1 << DDJ7) 	//8-in Channel HART MUX Addr2
		
		#define HART_ADDR0_HIGH  PORTJ |= (1 << PORTJ5) // HART_CH-AD0 = 1
		#define HART_ADDR0_LOW   PORTJ &= ~(1 << PORTJ5)// HART_CH-AD0 = 0
		#define HART_ADDR1_HIGH  PORTJ |= (1 << PORTJ6) // HART_CH-AD1 = 1
		#define HART_ADDR1_LOW   PORTJ &= ~(1 << PORTJ6)// HART_CH-AD1 = 0
		#define HART_ADDR2_HIGH  PORTJ |= (1 << PORTJ7) // HART_CH-AD2 = 1
		#define HART_ADDR2_LOW   PORTJ &= ~(1 << PORTJ7)// HART_CH-AD2 = 0
	
		//UART Interrupt vectors definition
		#define USART2_RXC_INT   USART2_RX_vect // HART RX UART interrupt vector
		#define USART2_TXC_INT   USART2_TX_vect // not used
    #endif

//  NOTE-------------NOTE--------------NOTE: deprecated	
//    #define   EXTERNALRAM_BASE   0x8000
//    #define   EXTERNALRAM_END    0xA027

/***********************************************************************************
 *  Hardware definitions for AI621 Cards
 ***********************************************************************************/
#elif  CARD_CODE == AI621_CODE

    #define LED_ENABLE_DEF      DDRK  |= (1 << DDK0)    // OE Latch-Alarm LED
    #define LED_ENABLE_HIGH     PORTK |= (1 << PORTK0)  // LED_OE = 1
    #define LED_ENABLE_LOW      PORTK &= ~(1 << PORTK0) // LED_OE = 0

    #define LED_LATCH_DEF       DDRK  |= (1 << DDK2)    // LE Latch-Alarm LED
    #define LED_LATCH_HIGH      PORTK |= (1 << PORTK2)  // LED_LE = 1
    #define LED_LATCH_LOW       PORTK &= ~(1 << PORTK2) // LED_LE = 0
	
	#define MUXAD0_DEF			DDRJ |= (1 << DDJ2)		// 8-in Channel MUX Address0
    #define MUXAD1_DEF			DDRJ |= (1 << DDJ3)		// 8-in Channel MUX Address1
    #define MUXAD2_DEF			DDRJ |= (1 << DDJ4)		// 8-in Channel MUX Address2
    
    #define MUXAD0_HIGH			PORTJ |= (1 << PORTJ2)	// CH-AD0 = 1
    #define MUXAD0_LOW			PORTJ &= ~(1 << PORTJ2)	// CH-AD0 = 0
    #define MUXAD1_HIGH			PORTJ |= (1 << PORTJ3)	// CH-AD1 = 1
    #define MUXAD1_LOW			PORTJ &= ~(1 << PORTJ3)	// CH-AD1 = 0 
    #define MUXAD2_HIGH			PORTJ |= (1 << PORTJ4)	// CH-AD2 = 1
    #define MUXAD2_LOW			PORTJ &= ~(1 << PORTJ4)	// CH-AD2 = 0 
	
	#define ANALOGPOWER_DEF	 	DDRE &= ~(1 << DDE5)  	// Alarm/Rest voltage monitor
    #define ANALOGPOWER_READ   (PINE & (1 << PORTE5)) 	// ARST_IRQ

	#define FIELDPOWER_DEF	 	DDRG &= ~(1 << DDG5)  	// Field Power input
    #define FIELDPOWER_READ	   (PING & (1 << PORTG5)) 	// FPOW 
	
    #define POWER_RST_DEF    	DDRH |= (1 << DDH0)    	// En of DC-DC Converter
    #define POWER_RST_HIGH   	PORTH |= (1 << PORTH0) 	// Power_RST = 1
    #define POWER_RST_LOW    	PORTH &= ~(1 << PORTH0)	// Power_RST = 0

	#define APIAE_DEF	        DDRK |= (1 << DDK1)	//Enable for Current for groupA
	#define APIBE_DEF			DDRJ |= (1 << DDJ5)	//Enable for Current for groupB
	#define APGE_DEF			DDRK |= (1 << DDK3)	//Enable for GND Connection of CS
	
	#define APIAE_HIGH			PORTK = PORTK | (1 << PORTK1)  // AP-IAE = 1
	#define APIAE_LOW			PORTK = PORTK & ~(1 << PORTK1) // AP-IAE = 0 
	#define APIBE_HIGH			PORTJ = PORTJ | (1 << PORTJ5)  // AP-IBE = 1
	#define APIBE_LOW			PORTJ = PORTJ & ~(1 << PORTJ5) // AP-IBE = 0 
	#define APGE_HIGH			PORTK = PORTK | (1 << PORTK3)  // AP-IGE = 1
	#define APGE_LOW			PORTK = PORTK & ~(1 << PORTK3) // AP-IGE = 0

	#define RELAY_ACTIVE_DEF	DDRH |= (1 << DDH1)		// Relay Activation Pin
	#define RELAY_ACTIVE_HIGH	PORTH |= (1 << PORTH1)	// AP-OE = 1
	#define RELAY_ACTIVE_LOW	PORTH &= ~(1 << PORTH1) // AP-OE = 0

//  NOTE-------------NOTE--------------NOTE: deprecated	
//    #define   EXTERNALRAM_BASE  0x8000
//    #define   EXTERNALRAM_END   0xA027

/***********************************************************************************
 *  Hardware definitions for AO672 Cards
 ***********************************************************************************/
#elif  (CARD_CODE == AO672_CODE)

	#define OUTPUTEN_DEF		DDRF  |= (1 << DDF3)	// Output Disable (XTR111)
    #define DISABLE_OUTPUTS		PORTF |= (1 << PORTF3)	// AP-OE = 1
    #define ENABLE_OUTPUTS		PORTF &= ~(1 << PORTF3)	// AP-OE = 0
	
	#define ANALOGPOWER_DEF	 	DDRE &= ~(1 << DDE5)  	// Alarm/Rest voltage monitor
    #define ANALOGPOWER_READ   (PINE & (1 << PORTE5)) 	// ARST_IRQ

	#define FIELDPOWER_DEF	 	DDRG &= ~(1 << DDG5)  	// Field Power input
    #define FIELDPOWER_READ	   (PING & (1 << PORTG5)) 	// FPOW  
	
    #define POWER_RST_DEF    	DDRK |= (1 << DDK1)    	// En of DC-DC Converter
    #define POWER_RST_HIGH   	PORTK |= (1 << PORTK1) 	// Power_RST = 1
    #define POWER_RST_LOW    	PORTK &= ~(1 << PORTK1)	// Power_RST = 0

    #define LED_ENABLE_DEF      DDRK  |= (1 << DDK0)    // OE Latch-Alarm LED
    #define LED_ENABLE_HIGH     PORTK |= (1 << PORTK0)  // LED_OE = 1
    #define LED_ENABLE_LOW      PORTK &= ~(1 << PORTK0) // LED_OE = 0

    #define LED_LATCH_DEF       DDRK  |= (1 << DDK2)    // LE Latch-Alarm LED
    #define LED_LATCH_HIGH      PORTK |= (1 << PORTK2)  // LED_LE = 1
    #define LED_LATCH_LOW       PORTK &= ~(1 << PORTK2) // LED_LE = 0

	#define STATUS_READ_DEF     DDRJ  |= (1 << DDJ2)    // OE Latch-Output Failure
    #define STATUS_READ_HIGH    PORTJ |= (1 << PORTJ2)  // STE = 1
    #define STATUS_READ_LOW     PORTJ &= ~(1 << PORTJ2) // STE = 0
   

	#if ModuleTypeSelection == 0

		//HART modem Definition
		#define INRTS_DEF   	DDRK |= (1 << DDK3)     // Request To Send of HART
		#define INRTS_HIGH  	PORTK |= (1 << PORTK3)	// INRTS-GPIO = 1
		#define INRTS_LOW   	PORTK &= ~(1 << PORTK3)	// INRTS-GPIO = 0
		
		#define OCD_DEF     	DDRE &= ~(1 << DDE7)    // Carrier Detect input
		#define OCD_RD		   (PINE & (1 << PORTE7))	// Read OCD_IRQ
		
		//HART Address channel Definition
		#define HART_ADDR0_DEF  DDRJ |= (1 << DDJ5) 	//8-in Channel HART MUX Addr0
		#define HART_ADDR1_DEF  DDRJ |= (1 << DDJ6) 	//8-in Channel HART MUX Addr1
		#define HART_ADDR2_DEF  DDRJ |= (1 << DDJ7) 	//8-in Channel HART MUX Addr2
		
		#define HART_ADDR0_HIGH PORTJ |= (1 << PORTJ5)  // HART_CH-AD0 = 1
		#define HART_ADDR0_LOW  PORTJ &= ~(1 << PORTJ5) // HART_CH-AD0 = 0
		#define HART_ADDR1_HIGH PORTJ |= (1 << PORTJ6)  // HART_CH-AD1 = 1
		#define HART_ADDR1_LOW  PORTJ &= ~(1 << PORTJ6) // HART_CH-AD1 = 0
		#define HART_ADDR2_HIGH PORTJ |= (1 << PORTJ7)  // HART_CH-AD2 = 1
		#define HART_ADDR2_LOW  PORTJ &= ~(1 << PORTJ7) // HART_CH-AD2 = 0
	
		//UART Interrupt vectors definition
		#define USART2_RXC_INT  USART2_RX_vect // HART RX UART interrupt vector
		#define USART2_TXC_INT  USART2_TX_vect // not used
	#endif // MODULE_TYPE_SELECTION

//  NOTE-------------NOTE--------------NOTE: deprecated	
//	#define EXTERNALRAM_BASE        0x8000
//	#define EXTERNALRAM_END         0xA027

#endif

#endif //__HARDWARE_H_

//***********************************************************************************
// No More
//***********************************************************************************