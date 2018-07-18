#include "Globals.h"
#include "SdWriter.h"

namespace NmeaData
{
    ::DateTime DateTime;
    ::Location Location;

    ::DateTime FirstDateTime;
    byte FirstDateTimeHasValue;
}

byte PressedButtons;

SdWriter* card;
