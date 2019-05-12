#include "Globals.h"
#include "SdWriter.h"

namespace NmeaData
{
    ::DateTime DateTime;
    ::Location Location;
    byte Version;
}

byte PressedButtons;

SdWriter* card;
