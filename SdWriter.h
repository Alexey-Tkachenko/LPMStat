#ifndef _SD_WRITER_H_
#define _SD_WRITER_H_

#include "libs/SD/SD.h"
#include "DataTypes.h"
#include "Format.h"
#include "LpmIoTask.h"

class SdWriter : public Print
{
private:
    SdWriter() {}
    SDLib::SDX_File file;

    static SdWriter* StartSD();

    SdWriter(const SdWriter&) = delete;
    SdWriter(SdWriter&&) = delete;
    SdWriter& operator=(const SdWriter&) = delete;
    SdWriter& operator=(SdWriter&&) = delete;

public:
    static SdWriter* Open(const ::DateTime& dateTime);
    void flush() override;
    ~SdWriter();

    size_t write(uint8_t data) override;
};

struct eol_t {};

const eol_t eol;

struct flush_t {};
const flush_t flush;

struct SdWrapper
{
    SdWriter* writer;
    inline SdWrapper(SdWriter* writer) : writer(writer) {}

    template<class T>
    inline SdWrapper& operator<<(const T& value)
    {
        if (writer)
        {
            writer->print(value);
        }
        return *this;
    }

    inline SdWrapper& operator<< (const char* ptr)
    {
        if (writer)
        {
            writer->print(ptr);
        }
        return *this;
    }

    inline SdWrapper& operator<< (const __FlashStringHelper* ptr)
    {
        if (writer)
        {
            writer->print(ptr);
        }
        return *this;
    }

    inline SdWrapper& operator<< (const ::DateTime &moment)
    {
        if (writer)
        {
            char bufferA[17];
            char bufferB[17];
            FormatDateTime(bufferA, bufferB, 16, moment);
            writer->print(bufferA);
            writer->write(' ');
            writer->print(bufferB);
        }
        return *this;
    }

    inline SdWrapper& operator<< (const ::Location &place)
    {
        static const char dms[3] PROGMEM = { '\xB0', '\'', '"' };
        static const char lat[] PROGMEM = { 'S', 'N' };
        static const char lon[] PROGMEM = { 'W', 'E' };
        if (writer)
        {
            char buffer[17];
            *FormatDMS(buffer, 16, place.Latitude, lat, dms) = 0;
            writer->print(buffer);
            writer->write('=');
            *FormatDDD(buffer, 16, place.Latitude, lat, '\xB0') = 0;
            writer->print(buffer);
            writer->write(' ');

            *FormatDMS(buffer, 16, place.Longitude, lon, dms) = 0;
            writer->print(buffer);
            writer->write('=');
            *FormatDDD(buffer, 16, place.Longitude, lon, '\xB0') = 0;
            writer->print(buffer);
            writer->write(' ');

            byte size = 16;
            char* ptr = buffer;
            FormatNumber(ptr, size, place.Altitude);
            put(ptr, size, 0);
            writer->print(buffer);
        }
        return *this;
    }

    inline SdWrapper& operator<<(const Magnitude& mag)
    {
        if (writer)
        {
            int val = mag.Value();
            char buffer[8];
            byte size = 8;
            char *ptr = buffer;
            PrintNumber2(ptr, size, (byte)(val / 100));
            put(ptr, size, '.');
            PrintNumber2(ptr, size, (byte)(val % 100));
            put(ptr, size, '\0');
            writer->print(buffer);
        }
        return *this;
    }

    inline SdWrapper& operator<<(const eol_t&)
    {
        if (writer)
        {
            writer->println();
        }
        return *this;
    }

    inline SdWrapper& operator<<(const flush_t&)
    {
        if (writer)
        {
            writer->flush();
        }
        return *this;
    }


};

inline SdWrapper Wrap(SdWriter* target)
{
    return target;
}

#endif
