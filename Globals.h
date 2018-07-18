#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#include "Arduino.h"
#include "DataTypes.h"

enum class Pins : byte
{
    //NMEA_RX = 0xFF,
    //NMEA_TX = 0xFF,

    LPM_RX = 9,
    LPM_TX = 8,

    SD_ENABLE = 13,

    LCD_BUTTONS = A0,
};

enum class I2C : byte
{
    LCD = 0x3F,
};

namespace NmeaData
{
    extern ::DateTime DateTime;
    extern ::Location Location;

    extern ::DateTime FirstDateTime;
    extern byte FirstDateTimeHasValue;
}

extern byte PressedButtons;

#endif
