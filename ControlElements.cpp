#include "ControlElements.h"

ControlElements::ControlElements()
{
	//	piny
	pinMode(PIN_BUTTON, INPUT_PULLUP);
	pinMode(PIN_GREEN_LED, OUTPUT);
	pinMode(PIN_YELLOW_LED, OUTPUT);
	pinMode(PIN_RED_LED, OUTPUT);

	//	stav tlacitka
	buttonStatus = false;
}
ControlElements::~ControlElements()
{

}

bool ControlElements::setButtonStatus(bool Status)
{
	return buttonStatus = Status;
}

bool ControlElements::getButtonStatus()
{
	bool x = this->buttonStatus;
	this->buttonStatus = false;
	return x;
}

void ControlElements::blinkLED(uint8_t blinkingLED)
{
	//Serial.println(blinkingLED);
	switch (blinkingLED)
	{
	case 0:
		digitalWrite(PIN_GREEN_LED, LOW);
		digitalWrite(PIN_YELLOW_LED, LOW);
		digitalWrite(PIN_RED_LED, LOW);

		digitalWrite(PIN_GREEN_LED, HIGH);
		digitalWrite(PIN_YELLOW_LED, HIGH);
		digitalWrite(PIN_RED_LED, HIGH);
		//	delka bliknuti
		delay(BLINK_TIME);

		digitalWrite(PIN_GREEN_LED, LOW);
		digitalWrite(PIN_YELLOW_LED, LOW);
		digitalWrite(PIN_RED_LED, LOW);
		return;
	case 1:
		digitalWrite(PIN_GREEN_LED, LOW);

		digitalWrite(PIN_GREEN_LED, HIGH);
		//	delka bliknuti
		delay(BLINK_TIME);

		digitalWrite(PIN_GREEN_LED, LOW);
		return;
	case 2:
		digitalWrite(PIN_YELLOW_LED, LOW);

		digitalWrite(PIN_YELLOW_LED, HIGH);
		//	delka bliknuti
		delay(BLINK_TIME);

		digitalWrite(PIN_YELLOW_LED, LOW);
		return;
	case 3:
		digitalWrite(PIN_RED_LED, LOW);

		digitalWrite(PIN_RED_LED, HIGH);
		//	delka bliknuti
		delay(BLINK_TIME);

		digitalWrite(PIN_RED_LED, LOW);
		return;
	default:
		return;
	}
	return;
}

void ControlElements::lightLED(uint8_t lightingLED)
{
	//Serial.println(lightingLED);
	switch (lightingLED)
	{
	case 0:
		digitalWrite(PIN_GREEN_LED, HIGH);
		digitalWrite(PIN_YELLOW_LED, HIGH);
		digitalWrite(PIN_RED_LED, HIGH);
		return;
	case 1:
		digitalWrite(PIN_GREEN_LED, HIGH);
		digitalWrite(PIN_YELLOW_LED, LOW);
		digitalWrite(PIN_RED_LED, LOW);
		return;
	case 2:
		digitalWrite(PIN_GREEN_LED, LOW);
		digitalWrite(PIN_YELLOW_LED, HIGH);
		digitalWrite(PIN_RED_LED, LOW);
		return;
	case 3:
		digitalWrite(PIN_GREEN_LED, LOW);
		digitalWrite(PIN_YELLOW_LED, LOW);
		digitalWrite(PIN_RED_LED, HIGH);
		return;
	default:
		return;
	}
}

void ControlElements::lightoffLED(uint8_t lightingLED)
{
	switch (lightingLED)
	{
	case 0:
		digitalWrite(PIN_GREEN_LED, LOW);
		digitalWrite(PIN_YELLOW_LED, LOW);
		digitalWrite(PIN_RED_LED, LOW);
		return;
	case 1:
		digitalWrite(PIN_GREEN_LED, LOW);
		return;
	case 2:
		digitalWrite(PIN_YELLOW_LED, LOW);
		return;
	case 3:
		digitalWrite(PIN_RED_LED, LOW);
		return;
	default:
		return;
	}
}

bool ControlElements::endCheck()
{
	int i = 1;
	this->setButtonStatus(false);
	//	nekonecny cyklus pro kontrolu stavu
	while (true)
	{
		//	vyhodnoceni prvniho kliku
		if (this->getButtonStatus() == true)
		{
			this->lightLED(0);
			delay(500);
			//	vyhodnoceni druheho kliku
			if (this->getButtonStatus() == true)
			{
				return false;
			}
			return true;
		}
		this->blinkLED(i);
		i++;
		if (i > 3)
			i = 1;
		delay(100);
	}
	return false;
}