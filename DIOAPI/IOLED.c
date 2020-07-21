#include "IOLED.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <wiringSerial.h>

enum results initGPIO(int pType)
{
	ProductType = pType;
	// wiringPi for GPIO managment init
	if (wiringPiSetup() < 0)
	{
		return ERROR;
	}

	else
	{
//		if (ProductType == RTU_450)
//		{
//			// Set I/Os direction
//			pinMode(RUN_LED_450, OUTPUT);
//			pinMode(STBY_LED_450, OUTPUT);
//			pinMode(STATUS_LED_450, OUTPUT);
//			pinMode(FLT_LED_450, OUTPUT);
//		}
//		else if(ProductType == RTU_451)
//		{
//			// Set I/Os direction
//			pinMode(RUN_LED_451, OUTPUT);
//			pinMode(STBY_LED_451, OUTPUT);
//			pinMode(STATUS_LED_451, OUTPUT);
//			pinMode(FLT_LED_451, OUTPUT);
//		}
//		else if(ProductType == DIO_560)
//		{
//			// Set I/Os direction
//			pinMode(RUN_LED_451, OUTPUT);
//			pinMode(STBY_LED_451, OUTPUT);
//			pinMode(STATUS_LED_451, OUTPUT);
//			pinMode(FLT_LED_451, OUTPUT);
//		}

		//return SUCCESS;
	}

	/// --- SPI --- ///
	/*if (wiringPiSPISetup(0, 10000000) < -1)
	{
		printf("SPI Init failed.\n");
	}*/
	//wiringPiSPISetupMode(0, 10000000, 0);
	//wiringPiSPISetupMode(1, 10000000, 0);

	return SUCCESS;
}

void IOsetLED(int LED, int State)
{
	// Turn the LEDs On
//	if (ProductType == RTU_450)
//	{
//		if (LED == RUN_LED)
//			digitalWrite(RUN_LED_450, State);
//		else if (LED == STBY_LED)
//			digitalWrite(STBY_LED_450, State);
//		else if (LED == STATUS_LED)
//			digitalWrite(STATUS_LED_450, State);
//		else if (LED == FLT_LED)
//			digitalWrite(FLT_LED_450, State);
//	}
//	else if (ProductType == RTU_451)
//	{
//		if (LED == RUN_LED)
//			digitalWrite(RUN_LED_451, State);
//		else if (LED == STBY_LED)
//			digitalWrite(STBY_LED_451, State);
//		else if (LED == STATUS_LED)
//			digitalWrite(STATUS_LED_451, State);
//		else if (LED == FLT_LED)
//			digitalWrite(FLT_LED_451, State);
//	}
//	else if(ProductType == DIO_560)
//	{
////		if (LED == RUN_LED)
////			
////		else if (LED == STBY_LED)
////			
////		else if (LED == STATUS_LED)
////			
////		else if (LED == FLT_LED)
//	}
}

/*void MainLoopDelay(int interval)
{
	IOsetLED(STAT_LED_1, LED_OFF);
	delay(interval >> 1);
	IOsetLED(STAT_LED_1, LED_ON);
	delay(interval >> 1);
}*/