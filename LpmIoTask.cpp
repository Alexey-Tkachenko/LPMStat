#include "LpmIoTask.h"
#include <SoftwareSerial.h>
#include "Arduino.h"
#include "ExpressTask.h"
#include "StaticAllocActivator.h"
#include "WaitHandles.h"
#include "Globals.h"

static WaitHandles::Event beginMeasureEvent;
static SoftwareSerial lpmSerialPort = SoftwareSerial((byte)Pins::LPM_RX, (byte)Pins::LPM_TX);
static WaitHandles::ValueHolder<int> lpmValue;
static int lpmCalibration;

TASK_BEGIN(LpmIoTask, { })

lpmSerialPort.begin(9600);

this->ClearBuffer();
lpmSerialPort.println(F("C#"));
lpmCalibration = ReadValue();


for (;;)
{
    TASK_WAIT_SIGNAL(&beginMeasureEvent);
    beginMeasureEvent.Reset();
    ClearBuffer();
    lpmSerialPort.println(F("V#"));
    TASK_YIELD_WHILE(!lpmSerialPort.available());
    ReadValue();
    
}


TASK_BODY_END


int ReadValue()
{
    char buffer[8];
    lpmSerialPort.readBytesUntil('\n', buffer, 8);
    const char* ptr = buffer;
    int value = 0;
    while (*ptr)
    {
        if (isdigit(*ptr))
        {
            value *= 10;
            value += *ptr - '0';
        }
    }
    return value;
}

void ClearBuffer()
{
    while (lpmSerialPort.available())
    {
        lpmSerialPort.read();
    }
}

TASK_CLASS_END

void RegisterLpmIoTask(Scheduler & scheduler)
{
    scheduler.Register(Instance<LpmIoTask>(), TaskPriority::SensorPoll);
}

bool LpmStartMeasure(WaitHandles::ValueHolder<int>** target)
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

int LpmGetCalibration()
{
    return lpmCalibration;
}

