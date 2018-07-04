#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#include "Arduino.h"
#include "DataTypes.h"

enum class Pins : byte
{
    NMEA_RX = 4,
    NMEA_TX = 5,

    SD_ENABLE = 13,
};

namespace NmeaData
{
    extern ::DateTime DateTime;
    extern ::Location Location;
}

#endif
