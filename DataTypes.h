#ifndef _DATETYPES_H_
#define _DATETYPES_H_

struct Location
{
    double Latitude;

    double Longitude;

    int Altitude;
};

struct DateTime
{
    uint8_t Year;
    uint8_t Month;
    uint8_t Day;
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Second;
};

#endif
