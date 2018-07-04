#include "SdWriter.h"
#include "Globals.h"

void SdWriter::StartSD()
{
    static bool sdStarted;
    if (sdStarted) return;
    sdStarted = true;
    SD.begin((byte)Pins::SD_ENABLE);
}

SdWriter & SdWriter::Open(::DateTime dateTime)
{
    StartSD();
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
