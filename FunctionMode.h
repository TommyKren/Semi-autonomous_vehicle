#ifndef __FUNCTIONMODE_H_
#define __FUNCTIONMODE_H_

#include "ControlElements.h"
#include <stdint.h>

class FunctionMode
{
private:
    //  ukazatele na instance vstupujicich trid
    ControlElements& controls;

    //  nastaveny rezim 1-n
    uint8_t SelectedMode;
public:
    // konstruktor pro vychozi rezim
    FunctionMode(ControlElements& controls);
    // destruktor
    ~FunctionMode();
    // metoda pro rucni volbu
    bool setModeExact(uint8_t ModeNumber);
    bool setModeAnalog(uint8_t AnalogRead);
    uint8_t getMode();
};
#endif
