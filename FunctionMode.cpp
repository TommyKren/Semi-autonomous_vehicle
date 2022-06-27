#include "FunctionMode.h"
#include <Arduino.h>
#include <stdint.h>

//  limity pro analogovy prepinac rezimu
//  programovaci mody [V], interpretace rezimu neprobiha, vystup 1-n
#define MODE_1         2
#define MODE_2         4
#define MODE_3         6
// pocet modu
#define MODE_NUMBERS   3

FunctionMode::FunctionMode(ControlElements& controls) : controls(controls), SelectedMode(0)
{

}

FunctionMode::~FunctionMode()
{

}

bool FunctionMode::setModeExact(uint8_t ModeNumber)
{
	// nastaveni hodnoty 
	if (ModeNumber > 0 && ModeNumber <= MODE_NUMBERS)
	{
		this->SelectedMode = ModeNumber;
		return true;
	}
	return false;
	
}

bool FunctionMode::setModeAnalog(uint8_t AnalogPin)
{
	//	promena pro praci s rezimem
	uint8_t preselectedMode;
	//	promena analogove hodnoty
	double VoltageRead;
	//	zablikani
	for (uint8_t i = 0; i < 5; i++)
	{
		controls.blinkLED(0);
		delay(200);
	}
	
	do
	{
		//	nacteni a namapovani hodnoty na napeti
		VoltageRead = ((double)analogRead(AnalogPin) - 0.00) * (5.00 - 0.00) / (1023.00 - 0);
		//	nastaveni hodnoty 
		if (VoltageRead < 0 || VoltageRead > 5)
		{
			controls.lightLED(0);
			delay(500);
			controls.lightoffLED(0);
			return false;
		}

		// zvoleni funkcniho rezimu
		if (VoltageRead < MODE_1) {
			preselectedMode = 1;
		}
		else if (VoltageRead < MODE_2) {
			preselectedMode = 2;
		}
		else if (VoltageRead < MODE_3) {
			preselectedMode = 3;
		}
		else
		{
			controls.lightLED(0);
			delay(500);
			controls.lightoffLED(0);
			return false;
		}
		if (controls.getButtonStatus() == true)
			controls.setButtonStatus(false);
		/*
		Serial.println("");
		Serial.print("Nastaveny rezim ");
		Serial.print(preselectedMode);
		Serial.print(" pri napeti ");
		Serial.println(VoltageRead);
		*/

		//	blikani zvoleneho rezimu
		controls.blinkLED(preselectedMode);
		delay(200);
		if (controls.getButtonStatus() == true)
		{
			//	potvrzeni tlacitkem
			this->SelectedMode = preselectedMode;
			controls.lightLED(preselectedMode);
			return true;
		}
		
	} while (true);
	return false;
	
	/*Serial.println("");
	Serial.println("");
	Serial.print("Nastaveny rezim ");
	Serial.print(getMode());
	Serial.print(" pri napeti ");
	Serial.println(((double)analogRead(A3) - 0) * (5 - 0) / (1023 - 0));*/
}

uint8_t FunctionMode::getMode()
{
	return this->SelectedMode;
}