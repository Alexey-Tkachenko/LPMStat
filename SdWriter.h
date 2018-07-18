#ifndef _SD_WRITER_H_
#define _SD_WRITER_H_

#include <SD.h>
#include "DataTypes.h"

class SdWriter : Print
{
private:
    SdWriter();
    SdFile file;

    static SdWriter& StartSD();

    SdWriter(const SdWriter&) = delete;
    SdWriter(SdWriter&&) = delete;
    SdWriter& operator=(const SdWriter&) = delete;
    SdWriter& operator=(SdWriter&&) = delete;

public:
    struct eol_t {};
    static const eol_t eol;

    static SdWriter& Open(::DateTime dateTime);
    void flush() override;
    ~SdWriter();

    size_t write(uint8_t data) override;

    template<class T>
    SdWriter& operator<<(const T& content)
    {
        file.print(content);
        return *this;
    }

    SdWriter& operator<<(eol_t)
    {
        flush();
        return *this;
    }
};

#endif
