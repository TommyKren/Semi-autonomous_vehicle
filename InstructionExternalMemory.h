#ifndef __INSTRUCTIONEXTERNALMEMORY_H_
#define __INSTRUCTIONEXTERNALMEMORY_H_

#include <Wire.h>
#include <Arduino.h>
#include <Math.h>

#include "ControlElements.h"

//  velikost I2C eeprom (pocet Bytu)
#define eepromSize        256

class InstructionExternalMemory
{
private:
	//  ukazatele na instance vstupujicich trid
	ControlElements& controls;

	//	vychozi hodnoty pameti
	byte EepromAdress;
	const byte SavedInstrBackup = 0x04;
	const byte StartingAdress = 0x05;
	const uint16_t MaxInstructionsQuantity = (eepromSize - StartingAdress + 1) / 3;
	
	//	promene pro praci s instrukcemi
	uint16_t NumSavedInstructions, NumReadInstructions;

public:
	//	vnitrni funkce pro zapsani a nacteni bytu z pameti
	bool writeByte(byte writingByte, byte CellAdress);
	byte readByte(byte CellAdress);
	
public:
	//	konstruktor
	InstructionExternalMemory(ControlElements &controls);
	//	destruktor
	~InstructionExternalMemory();
	//	metody pro praci s instrukcemi
	void setAdress(byte EepromAdress);
	void checkSavedData();
	uint8_t *getSavedInstruction(uint8_t* Instruction);
	bool writeInputInstructions();

};
#endif