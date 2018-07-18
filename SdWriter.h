#ifndef _SD_WRITER_H_
#define _SD_WRITER_H_

#include <SD.h>
#include "DataTypes.h"

class SdWriter : Print
{
private:
    SdWriter();
    SDLib::File file;

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

template<class T>
inline SdWriter* operator<<(SdWriter* writer, const T& content)
{
    if (writer)
    {
        writer->print(content);
    }
    return writer;
}

inline SdWriter* operator<< (SdWriter* writer, eol_t)
{
    if (writer)
    {
        writer->flush();
    }
    return writer;
}



#endif
