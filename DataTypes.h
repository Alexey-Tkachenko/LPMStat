#ifndef _DATETYPES_H_
#define _DATETYPES_H_

class Angle
{
    long value;

public:
    explicit inline Angle(long value) : value(value)
    {}

    Angle() : value() {}

    inline long Value() const
    {
        return value;
    }
};

struct Location
{
    Angle Latitude;

    Angle Longitude;

    int Altitude;
};

struct DateTime
{
    uint32_t Year : 6;
    uint32_t Month : 4;
    uint32_t Day : 5;
    uint32_t Hour : 5;
    uint32_t Minute : 6;
    uint32_t Second : 6;
};

#endif
