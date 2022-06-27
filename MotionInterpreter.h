#ifndef __MOTIONINTERPRETER_H_
#define __MOTIONINTERPRETER_H_

#include <MPU6050_tockn.h>
#include <NewPing.h>
#include "InstructionExternalMemory.h"
#include "ControlElements.h"

//  komunikacni piny pro rizeni motoru
#define MOTOR_LEFT_FORWARD      11
#define MOTOR_LEFT_BACKWARD     10

//  volba pinu dle verze arduina (zbytecna funkce vytvoreno podle spatneho pinoutu)
//  Arduino Uno
//#if defined (ARDUINO_AVR_UNO)
#define MOTOR_RIGHT_FORWARD     5
//  Arduino Nano
//#elif defined (ARDUINO_AVR_NANO)
// #define MOTOR_RIGHT_FORWARD     3
//#endif

#define MOTOR_RIGHT_BACKWARD    6

//  vzdalenosti objektu (0-255) cm
#define MAX_DISTANCE_SONAR      5

class MotionInterpreter
{
private:
    //  ukazatele na instance vstupujicich trid
    MPU6050& mpu6050;
    InstructionExternalMemory& externalMemory;
    NewPing& sonar;
    ControlElements& controls;

    bool gyroCalibration = false;
    //	pole instrukcnich parametru
    uint8_t MotionParameters[3];
public:
    //  konstruktor pro vychozi rezim
    MotionInterpreter(InstructionExternalMemory &externalMemory, MPU6050 &mpu6050, NewPing& sonar, ControlElements& controls);
    //  destruktor
    ~MotionInterpreter();

    //  metoda pro vyber pohybu a vykonani dle zavedenych parametru 
    uint8_t goByInstructions();

    //  pomocne metody
    bool readInstruction();
    
    //  metody pro vykonani jednotlivych pohybu
    
    //  funkce pro zataceni doprava
    void steeringRight(uint8_t steeringAngleValue, uint8_t steeringSpeedValue);

    //  funkce pro zataceni doleva
    void steeringLeft(uint8_t changeDirectionAngle, uint8_t steeringSpeedValue);

    //  funkce pro jizdu dopredu
    void goForward(uint8_t ridingTimeValue, uint8_t ridingSpeedValue);
    
    //  funkce pro jizdu dozadu
    void goBackward(uint8_t ridingTimeValue, uint8_t ridingSpeedValue);

    //  funkce pro detekci objektu v urcite vzdalenosti
    bool isSomethingThere(uint8_t maxDist);
};
#endif

