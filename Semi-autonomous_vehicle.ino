/*
 Name:		Semi_autonomous_vehicle.ino
 Created:	2/9/2020 11:56:08 AM
 Author:	Tomas Krenek
*/

//==============================================================================================
//  Knihovny externiho hardware
#include <Arduino.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <NewPing.h>
//  Tridy
#include "FunctionMode.h"
#include "InstructionExternalMemory.h"
#include "MotionInterpreter.h"
#include "ControlElements.h"

//==============================================================================================
//  Piny pro ultrazvukovy meric vzdalenosti
#define PIN_SONAR_TRIGGER       12
#define PIN_SONAR_ECHO          13

//  piny pro rezimy rizeni
#define MODE_SETTING_PIN        A3

//  adresa I2C eeprom
#define EEPROM_ADRESS           0x50

//==============================================================================================
//  Inicializace knihoven a trid

//  gyroskop
MPU6050 mpu6050(Wire);
//  sonar
NewPing sonar(PIN_SONAR_TRIGGER, PIN_SONAR_ECHO, 255);
//  ovladaci prvky
ControlElements controls;
//  komunikace s externi pameti
InstructionExternalMemory instrMem(controls);
//  funkcni rezim
FunctionMode mode(controls);
//  rizeni pohybu
MotionInterpreter motion(instrMem, mpu6050, sonar, controls);

//==============================================================================================
//  Inicializacni funkce

void setup() {
    //==============================================================================================
    //  vychozi nastaveni knihoven a trid
    //  seriova komunikace
    Serial.begin(9600);
    //  nastartovani sbernice I2C
    Wire.begin();
    //  nastaveni komunikace s pameti
    instrMem.setAdress(EEPROM_ADRESS);
    //  nastaveni interrupt
    attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), button_interrupt, FALLING);

    //==============================================================================================
    //  Nastaveno a jedeme
    Serial.println("A jedem");
    Serial.println("");
}

//==============================================================================================
//  Hlavni funkce

void loop() {
    //  volba funkcniho rezimu
    mode.setModeAnalog(MODE_SETTING_PIN);
    switch (mode.getMode())
    {
    case 1:
        Serial.println("Nastaveny rezim prednastavene jizdy");
        while (motion.goByInstructions() == 1)
        {
        }
        break;
    case 2:
        Serial.println("Nastaveny rezim programovani");
        instrMem.writeInputInstructions();
        break;
    case 3:
        Serial.println("Nastaveny rezim dalkoveho ovladani (neaktivni)");
        while (mode.getMode() == 3)
        {
            mode.setModeAnalog(MODE_SETTING_PIN);
            Serial.println(mode.getMode());
            delay(200);
        }
        break;
    }
    delay(500);
    return;
}

//==============================================================================================
//  Interrupt pro tlacitko

void button_interrupt()
{
    controls.setButtonStatus(true);
}

//==============================================================================================
/*
void testovaci_funkce_pameti()
{
    //  test ulozeni bytu
    //  vstupy
    byte zapisovany_byte = 0x01;
    byte pozice_zapisu = 30;
    //  cteni
    Serial.println("========================================");
    Serial.print("Zapsano v pameti na pozici ");
    Serial.print(pozice_zapisu);
    Serial.print(" je ");
    Serial.println(instrMem.readByte(pozice_zapisu));
    Serial.println("");

    //  zapis
    if (instrMem.writeByte(zapisovany_byte, pozice_zapisu) != 1)
        Serial.println("Zapsani bytu do pameti je chybne");
    else
        Serial.println("Zapsan byte do pameti");

    String vstup = "ahoj";
    String vystup = vstup + ", " + vstup + "!";
    Serial.println(vystup);
}
*/

/*
void testovaci_funkce_ovladani()
{
    controls.lightLED(0);
    Serial.println("========================================");
    Serial.println("Svitim");
    Serial.println("");
    delay(1000);
    controls.lightoffLED(0);
    delay(100);

    for (uint8_t i = 1; i <= 3; i++)
    {
        controls.lightLED(i);
        Serial.println("========================================");
        Serial.print("Svitim ");
        Serial.println(i);
        Serial.println("");
        delay(1000);
        controls.lightoffLED(0);
        delay(100);
    }

    
    Serial.println("========================================");
    Serial.println("Blikam");
    Serial.println("");
    delay(1000);
    controls.blinkLED(0);
    delay(100);

    for (uint8_t i = 1; i <= 3; i++)
    {
        
        Serial.println("========================================");
        Serial.print("Blikam ");
        Serial.println(i);
        Serial.println("");
        delay(1000);
        controls.blinkLED(i);
        delay(100);
    }
}
*/