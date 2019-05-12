#include "LpmIoTask.h"
#include <SoftwareSerial.h>
#include "Arduino.h"
#include "ExpressTask.h"
#include "StaticAllocActivator.h"
#include "WaitHandles.h"
#include "Globals.h"

#include "LcdIoTask.h"

static WaitHandles::Event beginMeasureEvent;
static SoftwareSerial lpmSerialPort = SoftwareSerial((byte)Pins::LPM_RX, (byte)Pins::LPM_TX);
//#define lpmSerialPort Serial
static WaitHandles::ValueHolder<Magnitude> lpmValue;
static Magnitude lpmCalibration;

TASK_BEGIN(LpmIoTask, { Magnitude value; } _)

lpmSerialPort.begin(9600);

while (lpmCalibration.Value() == 0)
{
    ClearBuffer();
    lpmSerialPort.write('C');
    TASK_YIELD_WHILE(!lpmSerialPort.available());
    lpmCalibration = Magnitude(ReadValue('\n'));
    TASK_YIELD();
}

for (;;)
{
    beginMeasureEvent.Reset();
    TASK_WAIT_SIGNAL(&beginMeasureEvent);
    ClearBuffer();
    lpmSerialPort.write('V');
    TASK_YIELD_WHILE(!lpmSerialPort.available());
    TASK_SLEEP(10);
    _.value = Magnitude(ReadValue('#'));
    lpmValue.Set(_.value);
}

TASK_BODY_END

void ClearBuffer()
{
    while (lpmSerialPort.available())
    {
        lpmSerialPort.read();
    }
}

int ReadValue(char delim)
{
    char buffer[9] = { 0 };
    for (byte i = 0; i < 8; ++i)
    {
        int v = lpmSerialPort.read();
        if (v == delim)
        {
            break;
        }
        buffer[i] = v;
    }
        
    int value = 0;
    for (const char *ptr = buffer; *ptr; ++ptr)
    {
        if (*ptr >= '0' && *ptr <= '9')
        {
            value *= 10;
            value += *ptr - '0';
        }
    }
    return value;
}

TASK_CLASS_END

void RegisterLpmIoTask(Scheduler & scheduler)
{
    scheduler.Register(InstanceOnce<LpmIoTask>(), TaskPriority::SensorPoll);
}

bool LpmStartMeasure(WaitHandles::ValueHolder<Magnitude>** target)
{
    if (beginMeasureEvent.IsSignalled())
    {
        return false;
    }
    if (target)
    {
        *target = &lpmValue;
    }
    beginMeasureEvent.Set();
    return true;
}


Magnitude LpmGetCalibration()
{
    return lpmCalibration;
}

Magnitude::Magnitude() : value() {}

Magnitude::Magnitude(int value) : value(value)
{
}


bool Magnitude::operator<(const Magnitude& other)
{
    return this->value < other.value;
}
