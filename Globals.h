#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#include "Arduino.h"
#include "DataTypes.h"

enum class Pins : byte
{
    //NMEA_RX = 9,
    //NMEA_TX = 8,

    LPM_RX = 7,
    LPM_TX = 8,

    LCD_STATE = 4,

    SD_ENABLE = 10,

    LCD_BUTTONS = A0,

    RED_LED = 3,
};

enum class I2C : byte
{
    LCD = 0x3F,
};

namespace NmeaData
{
    extern ::DateTime DateTime;
    extern ::Location Location;
    extern byte Version;
}

extern byte PressedButtons;

class SdWriter;

extern SdWriter *card;

#endif
