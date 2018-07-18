#include "SdWriter.h"
#include "Globals.h"
#include "PlacementNew.h"

SdWriter& SdWriter::StartSD()
{
    static SdWriter* sdPtr;
    static byte instance[sizeof(SdWriter)];
    if (sdPtr == nullptr)
    {
        sdPtr = new(instance)SdWriter();
        SD.begin((byte)Pins::SD_ENABLE);
    }
    return *sdPtr;
}

SdWriter & SdWriter::Open(::DateTime dateTime)
{
    return StartSD();
}

inline void SdWriter::flush()
{
    file.flush();
}

SdWriter::~SdWriter()
{
    flush();
    file.close();
}

inline size_t SdWriter::write(uint8_t data)
{
    return file.write(data);
}
