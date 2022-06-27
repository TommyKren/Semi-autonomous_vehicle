#ifndef __CONTROLELEMENTS_H_
#define __CONTROLELEMENTS_H_

//  piny pro tlacitko a LED
#define PIN_BUTTON              2
#define PIN_GREEN_LED           7
#define PIN_YELLOW_LED          8
#define PIN_RED_LED             9

//	delka bliknuti
#define	BLINK_TIME				200

class ControlElements
{
private:
	volatile bool buttonStatus;
public:
	ControlElements();
	~ControlElements();

	bool setButtonStatus(bool Status);
	bool getButtonStatus();
	void blinkLED(uint8_t blinkingLED);
	void lightLED(uint8_t lightingLED);
	void lightoffLED(uint8_t lightingLED);
	bool endCheck();
};

//	extern ControlElements controlsExtern;

#endif