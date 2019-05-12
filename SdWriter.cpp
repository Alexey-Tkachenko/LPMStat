#include "SdWriter.h"
#include "Globals.h"
#include "PlacementNew.h"
#include "Assert.h"

static void SDDateTimeHook(uint16_t* date, uint16_t* time) 
{
    *date = FAT_DATE(NmeaData::DateTime.Year + 2000, NmeaData::DateTime.Month, NmeaData::DateTime.Day);
    *time = FAT_TIME(NmeaData::DateTime.Hour, NmeaData::DateTime.Minute, NmeaData::DateTime.Second);
}

SdWriter* SdWriter::StartSD()
{
    static SdWriter* sdPtr;
    static byte instance[sizeof(SdWriter)];
    static byte state;

    if (state == 0)
    {
        sdPtr = new(instance)SdWriter();
    }
    if (state != 1)
    {
        if (!SDX.begin((byte)Pins::SD_ENABLE))
        {
            SdFile::dateTimeCallback(SDDateTimeHook);
            state = 0x10;
            return nullptr;
        }
        state = 1;
    }
    return sdPtr;
}

SdWriter * SdWriter::Open(const ::DateTime &dateTime)
{
    SdWriter* writer = StartSD();
    if (writer == nullptr)
    {
        return nullptr;
    }

    uint8_t year = dateTime.Year;
    uint8_t month = dateTime.Month;
    uint8_t day = dateTime.Day;
    uint8_t hour = dateTime.Hour;
    uint8_t minute = dateTime.Minute;

    char buf[] = {
        (char)('0' + year / 10),
        (char)('0' + year % 10),
        '-',
        (month > 9 ? '1' : '0'),
        (char)('0' + (month > 9 ? month - 10 : month)),
        '-',
        (char)('0' + day / 10),
        (char)('0' + day % 10),
        '\0', // [8]
        (char)('0' + hour / 10),
        (char)('0' + hour % 10),
        (char)('0' + minute / 10),
        (char)('0' + minute % 10),
        '.',
        'T',
        'X',
        'T',
        '\0'
    };

    SDX.mkdir(buf);
    buf[8] = '/';
    writer->file = SDX.open(buf, FILE_WRITE);
    if (writer->file)
    {
        return writer;
    }
    return nullptr;
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
