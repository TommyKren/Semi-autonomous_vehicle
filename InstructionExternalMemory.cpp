#include "InstructionExternalMemory.h"

InstructionExternalMemory::InstructionExternalMemory(ControlElements& controls) : controls(controls),NumReadInstructions(0)
{
	
}

InstructionExternalMemory::~InstructionExternalMemory()
{

}

void InstructionExternalMemory::setAdress(byte EepromAdress)
{
    //	nastaveni adresy pameti
    this->EepromAdress = EepromAdress;

    this->checkSavedData();
}

void InstructionExternalMemory::checkSavedData()
{
    //  nacteni zalohy z pameti
    Wire.beginTransmission(this->EepromAdress);
    //	adresa hodnoty v EEPROM
    Wire.write(this->SavedInstrBackup);
    //	ukonceni odesilani dat
    Wire.endTransmission();
    //	cekame na jeden byte
    Wire.requestFrom((uint8_t)this->EepromAdress, (uint8_t)1);
    //	kdyz je komunikace aktivni precti byte
    if (Wire.available()) this->NumSavedInstructions = Wire.read();
    delay(10);
    
    //  komentare
    Serial.println("========================================");
    Serial.print("Pocet ulozenych instrukci ");
    Serial.println(NumSavedInstructions);
    Serial.println("========================================");
    Serial.println(" ");
}

bool InstructionExternalMemory::writeByte(byte writingByte, byte CellAdress)
{
    //  inicalizace komunikace s EEPROM
    Wire.beginTransmission(this->EepromAdress);
    //  odesleme jeden byte s adresou v pameti
    Wire.write(CellAdress);
    //  zapiseme data
    Wire.write(writingByte);
    //  pocka na zapsani
    delay(10);
    //  ukonci komunikaci ze pameti
    Wire.endTransmission();
    delay(10);
    //  pripravime promenou pro porovnani
    byte byteEEPROM = 0x69;
    //  nastaveni adresy pameti
    Wire.beginTransmission(this->EepromAdress);
    //  adresa hodnoty  v EEPROM
    Wire.write(CellAdress);
    //  ukonèení odesílání dat
    Wire.endTransmission();
    //  èekáme na jeden byte
    Wire.requestFrom((uint8_t)this->EepromAdress,(uint8_t)1);
    //  když je komunikace aktivní pøeèti byte
    if (Wire.available()) byteEEPROM = Wire.read();
    delay(10);
    //  kontrola zapsaneho bytu
    //  Serial.print("Nacteny byte ");
    //  Serial.println(byteEEPROM);
    if (byteEEPROM == writingByte) {
        return true;
    }
    else {
        //Serial.print("Zapsany byte ");
        //Serial.println(writingByte);
        return false;
    }
    return false;
}

byte InstructionExternalMemory::readByte(byte CellAdress)
{
	//	pripravime promenou pro vycteni
	byte byteEEPROM = 0xFF;
	//	nastaveni adresy pameti
	Wire.beginTransmission(this->EepromAdress);
	//	adresa hodnoty  v EEPROM
	Wire.write(CellAdress);
	//	ukonceni odesilani dat
	Wire.endTransmission();
	//	cekame na jeden byte
	Wire.requestFrom((uint8_t)this->EepromAdress, (uint8_t)1);
	//	kdyz je komunikace aktivni precti a predej dal byte
	if (Wire.available()) byteEEPROM = Wire.read();
	delay(10);
	return byteEEPROM;
}

bool InstructionExternalMemory::writeInputInstructions()
{
    //  sekundarni overeni rezimu programovani
    Serial.println("========================================");
    Serial.println("Opravdu chcete upravit instrukce: Y/N");
    //  Nacteni odpovedi
    while (Serial.available() <= 0)
        delay(100);
    //  Vyhodnoceni
    if (Serial.read() == 'Y')
    {
        Serial.println("Ano");
        
    }
    else
    {
        Serial.println("========================================");
        Serial.println("Zvolte jiny rezim");
        return false;
    }
    while (Serial.available() != 0)
    {
        Serial.read();
        delay(10);
    }
    //  resetovani ulozenych instrukci
    this->NumSavedInstructions = 0;
    //  promena pro pocitani nactenich parametru
    uint16_t inputNumberCount = 0;

    //  cyklus pro nacteni 
    while(inputNumberCount/3 <= MaxInstructionsQuantity)
    {
        //  Dotazy na vstupni parametry
        switch ((inputNumberCount % 3) + 1)
        {
        case 1:
            Serial.println("========================================");
            Serial.println("Zadejte cislo funkce:");
            break;
        case 2:
            Serial.println("Zadejte prvni parametr:");
            break;
        case 3:
            Serial.println("Zadejte druhy parametr:");
            break;
        }
        //  cekani na vstup
        while (Serial.available() <= 0)
            delay(100);
        
        //  pocet znaku na vstupu
        uint8_t inputArrayLength = Serial.available();
        if (inputArrayLength > 3)
        {
            Serial.println("Chybne cislo (0-255)");
            while (Serial.available() != 0)
            {
                Serial.read();
                delay(10);
            }
            continue;
        }
        
        //  vstupni parametr
        uint16_t inputParameter = 0;
        //  nacteni parametru ze seriove linky a jeho prepocitani na int
        for (uint8_t i = 0; i < inputArrayLength; i++)
        {
            uint16_t decimalDigits = 1;
            for (int j = 1; j < inputArrayLength - i; j++)
            {
                decimalDigits *= 10;
            }
            inputParameter += (Serial.read() - 48) * decimalDigits;
        }
        //  Overeni vstupu
        if (inputParameter >= 256)
        {
            Serial.println("Chybne cislo (0-255)");
            continue;
        }
        //  ukoncovaci znak
        if (inputParameter == 0 && inputNumberCount % 3 == 0)
        {
            writeByte((int)(inputNumberCount / 3), SavedInstrBackup);
            delay(100);
            return true;
        }
        //  parametr prosel vstupem a muze se dal zpracovat
        inputNumberCount++;
        //  podminka pro chybu v pameti
        if (writeByte(inputParameter, StartingAdress + inputNumberCount - 1) == false)
            continue;

        //  komentare pro test
        Serial.println("Zapsano do pameti");

        Serial.println("");
        Serial.print("Nacteni cisla ");
        Serial.println(inputParameter);
        Serial.print("Cislo parametru ");
        Serial.println(inputNumberCount);
        Serial.println("");
    }
}

uint8_t *InstructionExternalMemory::getSavedInstruction(uint8_t *Instruction)
{
    
    //	ulozeni instrukce
    //uint8_t Instruction[3] = { 0,0,0 };
    for (uint8_t i = 0; i < 3; i++)
    {
        Instruction[i] = this->readByte(StartingAdress + (NumReadInstructions*3) + i);
        delay(10);
        
        //  komentare
        /*
        Serial.println("");
        Serial.print("Nacteni parametru ");
        Serial.println(Instruction[i]);
        Serial.print("Cislo instrukce ");
        Serial.println(NumReadInstructions+1);
        */
    }
    this->NumReadInstructions++;
    delay(10);
    if (NumReadInstructions > NumSavedInstructions)
    {
        Instruction[0] = 0;
        this->NumReadInstructions = 0;
    }
    return Instruction;
}

//  Stare funkce nepouzite

/*bool InstructionExternalMemory::writeInputInst()
{
    //  resetovani ulozenych instrukci
    this->NumSavedInstructions = 0;
    //  byte pro hodnotu ze seriove linky
    byte byteFromSerial = 0;
    //  pocet mezer v prikazu
    uint8_t spaceCount = 0;
    //  promena pro pocitani poctu nactenych cisel ve stringu
    uint8_t inputNumberCount = 0;
    //  string pro nacteni bytu
    String rawNumberInput[3];
    
    Serial.println("========================================");
    Serial.println("Zadejte prikaz ve formatu { F(000-255) (000-255) (000-255) }"); Serial.println("");

    //  cyklus pro nacteni 
    do {
        //  cekani na seriovou linku
        while (Serial.available() <= 0)
            delay(200);
        //  nacteni bytu
        byteFromSerial = Serial.read();

        //  podminka pro interpretaci specialnich znaku
        switch ((uint8_t)byteFromSerial) {

            //==============================================================================================
            //  Znak na konci zadavaci sekvence (ASCII E)
        case 69:
            Serial.println("Sekvence ulozena, prepnete rezim"); Serial.println("");
            Serial.println("========================================");
            while (Serial.available() > 0) {
                Serial.read();
                delay(10);
            }
            break;

            //==============================================================================================
            //  Prvni znak oznaceni funkce (ASCII F)
        case 70:
            Serial.print("Nacteni instrukce c. "); Serial.println(this->NumSavedInstructions+1); Serial.println("");
            Serial.println("========================================");
            break;

            //==============================================================================================
            //  Znak mezery (ASCII Space)
        case 32:
            Serial.print("--------"); Serial.print(inputByteCount); Serial.println(". parametr zadan--------");
            Serial.println("");
            Serial.println("Mezernik");
            break;

            //==============================================================================================
            // Znak pro odradkovani (ASCII ENTER)
        case 10:
            NumSavedInstructions++;
            for (byte j = 0; j < spaceCount; j++) {
                this->NumSavedInstructions++;
                Serial.print("--------instrukce c. "); Serial.print(this->NumSavedInstructions); Serial.println(" akceptovana--------");
                Serial.println("Zadejte další pøíkaz"); Serial.println("");
                Serial.println("");
                break;
            }

            //==============================================================================================
            //  hodnoty a jejich prevedeni a zapsani 
        default:
            //  podminka pro spatne vstupy
            if (isDigit((uint8_t)byteFromSerial) != 1) {
                Serial.print("--------"); Serial.print(this->LastSavedByte + 1); Serial.println(". prikaz CHYBA--------");
                Serial.println("");
                return false;
            }
            if (inputNumberCount > 2)
            {
                inputNumberCount = 0;
            }
            this->InstructionParameters[inputNumberCount] = (char)byteFromSerial;
            inputNumberCount++;
            Serial.print("--------nacten byte "); Serial.print(byteFromSerial); Serial.println(" ------------------");
            break;
        }
    } 
    //  69 (ASCII E)
    while (byteFromSerial != 69);

    Serial.println("Sekvence ulozena, prepnete rezim a resetujte"); Serial.println("");
    Serial.println("========================================");
}*/