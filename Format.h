#ifndef _FORMAT_H_
#define _FORMAT_H_

#include "Arduino.h"
#include "DataTypes.h"
#include "LpmIoTask.h"

void put(char* &buffer, byte &size, char c);

char* FormatDMS(char * ptr, byte size, Angle val, const char * hsv, const char * dms);

char* FormatDDD(char * ptr, byte size, Angle val, const char * hsv, char deg);

void PrintNumber3(char *& ptr, byte & size, byte v);

void PrintNumber2(char *& ptr, byte & size, byte v);

void FormatNumber(char *& ptr, byte & size, int number);

void FormatDateTime(char *bufferA, char *bufferB, byte size, const ::DateTime& moment);

char* FormatTime(char* ptr, byte& size, const DateTime& now);

char* FormatDate(char* ptr, byte& size, const DateTime& now);

void FormatMagnitude(char*& ptr, byte& size, Magnitude value);

#endif
