#include "Format.h"

void put(char* &buffer, byte &size, char c)
{
    if (size)
    {
        *buffer = c;
        ++buffer;
        --size;
    }
}

char* FormatDMS(char* ptr, byte size, Angle val, const char *hsv, const char* dms)
{
    long v = val.Value();

    char hs = pgm_read_byte(hsv + (v < 0 ? 0 : 1));

    if (v < 0)
    {
        v = -v;
    }
    
    long d = v / 1000000LL;
    v %= 1000000;
    PrintNumber3(ptr, size, (byte)d);
    put(ptr, size, pgm_read_byte(dms + 0));
    v *= 36;
    v /= 10000;
    
    byte m = v / 60;
    PrintNumber2(ptr, size, m);
    put(ptr, size, pgm_read_byte(dms + 1));

    byte s = v % 60;
    PrintNumber2(ptr, size, s);
    put(ptr, size, pgm_read_byte(dms + 2));

    put(ptr, size, hs);

    return ptr;
}

char* FormatDDD(char* ptr, byte size, Angle val, const char *hsv, char deg)
{
    long v = val.Value();

    char hs = pgm_read_byte(hsv + (v < 0 ? 0 : 1));

    if (v < 0)
    {
        v = -v;
    }

    long d = v / 1000000LL;
    v %= 1000000;
    v /= 100;

    PrintNumber3(ptr, size, (byte)d);
    
    put(ptr, size, '.');
    put(ptr, size, (char)('0' + v / 1000));
    v %= 1000;
    put(ptr, size, (char)('0' + v / 100));
    v %= 100;
    put(ptr, size, (char)('0' + v / 10));
    put(ptr, size, (char)('0' + v % 10));
    
    put(ptr, size, hs);

    return ptr;
}

void PrintNumber3(char*&ptr, byte& size, byte v)
{
    if (v >= 100)
    {
        put(ptr, size, '1');
        v -= 100;
    }
    PrintNumber2(ptr, size, v);
}

void PrintNumber2(char*&ptr, byte& size, byte v)
{
    put(ptr, size, (char)('0' + v / 10));
    put(ptr, size, (char)('0' + v % 10));
}

void FormatNumber(char *& ptr, byte & size, int number)
{
    if (number < 0)
    {
        put(ptr, size, '-');
        number = -number;
    }
    char rev[8];
    byte rpos = 0;
    while (number)
    {
        rev[rpos++] = (char)('0' + number % 10);
        number /= 10;
    }
    while (--rpos)
    {
        put(ptr, size, rev[rpos]);
    }
    put(ptr, size, rev[0]);
    put(ptr, size, '\0');
}

char* FormatDate(char* ptr, byte& size, const DateTime& now)
{
    put(ptr, size, (char)('2'));
    put(ptr, size, (char)('0'));
    put(ptr, size, (char)('0' + now.Year / 10));
    put(ptr, size, (char)('0' + now.Year % 10));
    put(ptr, size, (char)('-'));
    put(ptr, size, (char)(now.Month > 9 ? '1' : '0'));
    put(ptr, size, (char)('0' + (now.Month > 9 ? now.Month - 10 : now.Month)));
    put(ptr, size, (char)('-'));
    put(ptr, size, (char)('0' + now.Day / 10));
    put(ptr, size, (char)('0' + now.Day % 10));
    return ptr;
}

char* FormatTime(char* ptr, byte& size, const DateTime& now)
{
    put(ptr, size, (char)('0' + now.Hour / 10));
    put(ptr, size, (char)('0' + now.Hour % 10));
    put(ptr, size, (char)(':'));
    put(ptr, size, (char)('0' + now.Minute / 10));
    put(ptr, size, (char)('0' + now.Minute % 10));
    put(ptr, size, (char)(':'));
    put(ptr, size, (char)('0' + now.Second / 10));
    put(ptr, size, (char)('0' + now.Second % 10));
    put(ptr, size, ' ');
    put(ptr, size, 'U');
    put(ptr, size, 'T');
    return ptr;
}

void FormatDateTime(char * bufferA, char * bufferB, byte _size, const ::DateTime & now)
{
    byte size = _size;
    *FormatDate(bufferA, size, now) = 0;

    size = _size;
    *FormatTime(bufferB, size, now) = 0;
}

void FormatMagnitude(char *& ptr, byte & size, Magnitude value)
{
    int v = value.Value();
    if (v > 1000)
    {
        put(ptr, size, '0' + (v / 1000));
        v %= 1000;
    }
    put(ptr, size, '0' + v / 100);
    v %= 100;
    put(ptr, size, '.');
    put(ptr, size, '0' + (v / 10));
    put(ptr, size, '0' + (v % 10));
}

