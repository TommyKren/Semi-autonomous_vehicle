#include "MotionInterpreter.h"

//  konstruktor se vstupem pouzitych instanci (externi pamet, gyro, sonar)
MotionInterpreter::MotionInterpreter(InstructionExternalMemory& externalMemory, MPU6050& mpu6050, NewPing& sonar, ControlElements& controls) :
    externalMemory(externalMemory), mpu6050(mpu6050), sonar(sonar), controls(controls)
{
    //  inicializace pinu
    pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
    pinMode(MOTOR_LEFT_BACKWARD, OUTPUT);
    pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
    pinMode(MOTOR_RIGHT_BACKWARD, OUTPUT);
}

MotionInterpreter::~MotionInterpreter()
{

}

uint8_t MotionInterpreter::goByInstructions()
{
	if (gyroCalibration == false)
	{
		//  startovani gyra
		mpu6050.begin();
		//  kalibrace gyra
		mpu6050.calcGyroOffsets(true);
		this->gyroCalibration = true;
	}
    //  kontrola nactenych dat
    externalMemory.checkSavedData();
    //  nacteni a provedeni instrukce
    while (readInstruction() == true)
    {
        switch (MotionParameters[0])
        {
        case 0:
            Serial.println("Na vstupu neni instrukce");
            delay(500);
            break;
        case 1:
            this->goForward(MotionParameters[1], MotionParameters[2]);
            break;
        case 2:
            this->goBackward(MotionParameters[1], MotionParameters[2]);
            break;
        case 3:
            this->steeringRight(MotionParameters[1], MotionParameters[2]);
            break;
        case 4:
            this->steeringLeft(MotionParameters[1], MotionParameters[2]);
            break;
        }
    }
    //  Vyhodnoceni konec
    Serial.println("");
    Serial.println("========================================");
    Serial.println("Instrukce dokonceny (potvrdte)");
    
    //  varianta se vstupem z pocitace
    /*
    while (Serial.available() <= 0)
        delay(100);
    if (Serial.read() == 'Y')
    {
        Serial.println("Repete");
        return 1;
    }
    */
    
    //  varianta s LED a tlacitkem
    return controls.endCheck();
}

bool MotionInterpreter::readInstruction()
{
    externalMemory.getSavedInstruction(MotionParameters);
    if (MotionParameters[0] == 0)
        return false;
    return true;
}

//==============================================================================================
//  funkce pro zataceni doprava

void MotionInterpreter::steeringRight(uint8_t steeringAngleValue, uint8_t steeringSpeedValue) {
    mpu6050.update();
    float requiredDirectionAngle = mpu6050.getAngleZ() - steeringAngleValue;
    analogWrite(MOTOR_LEFT_FORWARD, steeringSpeedValue);
    do {
        delay(50);
        //  Serial.println(mpu6050.getAngleZ());
        mpu6050.update();
    } while (mpu6050.getAngleZ() > requiredDirectionAngle);

    analogWrite(MOTOR_LEFT_FORWARD, 0);

    Serial.println("========================================");
    Serial.println("Zatoceno doprava");

    //  Testovaci funkce
    /*Serial.print("pozadovany uhel - ");
    Serial.println(requiredDirectionAngle-2);

    if(mpu6050.getAngleZ() < requiredDirectionAngle + 2 && mpu6050.getAngleZ() > requiredDirectionAngle - 2){
      Serial.print("OK - ");
      Serial.println(mpu6050.getAngleZ());}
    else{
      Serial.print("chyba - ");
      Serial.println(mpu6050.getAngleZ());}*/
}

//==============================================================================================
//  funkce pro zataceni doleva

void MotionInterpreter::steeringLeft(uint8_t changeDirectionAngle, uint8_t steeringSpeedValue) {
    mpu6050.update();
    float requiredDirectionAngle = mpu6050.getAngleZ() + changeDirectionAngle;
    analogWrite(MOTOR_RIGHT_FORWARD, steeringSpeedValue);
    do {
        delay(50);
        //  Serial.println(mpu6050.getAngleZ());
        mpu6050.update();
    } while (mpu6050.getAngleZ() < requiredDirectionAngle);

    analogWrite(MOTOR_RIGHT_FORWARD, 0);

    Serial.println("========================================");
    Serial.println("Zatoceno doleva");
}

//==============================================================================================
//  funkce pro jizdu dopredu

void MotionInterpreter::goForward(uint8_t ridingTimeValue, uint8_t ridingSpeedValue) {
    // urceni vychoziho stavu
    mpu6050.update();
    float defaultDirectionAngle = mpu6050.getAngleZ();
    uint8_t defaultTime = millis() / 1000;
    Serial.println("========================================");
    Serial.print("Vychozi cas ");
    Serial.print(defaultTime);
    Serial.println(" s");
    // spusteni motoru
    analogWrite(MOTOR_RIGHT_FORWARD, ridingSpeedValue);
    analogWrite(MOTOR_LEFT_FORWARD, ridingSpeedValue);
    // cyklus pro pohyb s podminkou pro dobu jizdy
    do {
        // aktualizace dat z gyroskopu v kazde iteraci
        delay(100);
        mpu6050.update();
        // overeni prekazky
        if (isSomethingThere(MAX_DISTANCE_SONAR) == true) {
            analogWrite(MOTOR_RIGHT_FORWARD, 0);
            analogWrite(MOTOR_LEFT_FORWARD, 0);
            Serial.println("========================================");
            Serial.println("Prekazka");
            return;
        }
        // korekce doprava
        else if (mpu6050.getAngleZ() < defaultDirectionAngle - 2) {
            analogWrite(MOTOR_RIGHT_FORWARD, ridingSpeedValue + 50);
            Serial.println("Vychyleni doprava");
        }
        // korekce doleva
        else if (mpu6050.getAngleZ() > defaultDirectionAngle + 2) {
            analogWrite(MOTOR_LEFT_FORWARD, ridingSpeedValue + 50);
            Serial.println("Vychyleni doleva");
        }
        // jizda rovne
        else {
            analogWrite(MOTOR_RIGHT_FORWARD, ridingSpeedValue);
            analogWrite(MOTOR_LEFT_FORWARD, ridingSpeedValue);
            //Serial.println("Jedu rovne");
        }
    } while (millis() / 1000 - defaultTime < ridingTimeValue);
    // zastaveni motoru
    analogWrite(MOTOR_RIGHT_FORWARD, 0);
    analogWrite(MOTOR_LEFT_FORWARD, 0);

    Serial.print("Dojeto na misto v case ");
    Serial.print(millis() / 1000);
    Serial.println(" s");
}

//==============================================================================================
//  funkce pro jizdu dozadu

void MotionInterpreter::goBackward(uint8_t ridingTimeValue, uint8_t ridingSpeedValue) {

}

//==============================================================================================
//  funkce pro detekci objektu v urcite vzdalenosti

bool MotionInterpreter::isSomethingThere(uint8_t maxDist) {
    // nacteni vzdalenosti v centimetrech do vytvorene promenne vzdalenost
    int detectDistance = sonar.ping_cm(maxDist);
    // pokud byla detekovana vzdalenost vetsi nez 0, neco tam je a koncime true
    if (detectDistance > 0) {
        return true;
    }
    // ve zbylich pripadech vracime "nikde nic"
    else {
        return false;
    }
}