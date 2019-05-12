#include <EEPROM.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include "Scheduler.h"

#include "NmeaReaderTask.h"
#include "SdWriter.h"
#include "ButtonsReaderTask.h"
#include "LcdIoTask.h"
#include "ControlTask.h"
#include "Assert.h"
#include "Globals.h"

void setup() __ATTR_NORETURN__;


#define CANNARY

void blink(byte type) __ATTR_NORETURN__;


void setup()
{
    static byte _scheduler[sizeof(Scheduler)];
    Scheduler& scheduler = *new(_scheduler)Scheduler();

    RegisterNmeaReaderTask(scheduler);
    RegisterButtonsReaderTask(scheduler);
    RegisterLcdIoTask(scheduler);
    RegisterControlTask(scheduler);
    RegisterLpmIoTask(scheduler);

    Serial.begin(9600);
    scheduler.PrintStatistics(Serial);
    Serial.end();

#ifdef CANNARY
    pinMode(3, OUTPUT);
    digitalWrite(3, 1);
    delay(500);
    digitalWrite(3, 0);

    const size_t cannarySize = 2;

    uint32_t* cannary = (uint32_t*)malloc(cannarySize * 4);
    if (cannary)
    {
        for (size_t i = 0; i < cannarySize; ++i)
        {
            cannary[i] = 0xDEADBEEF;
        }
    }
    else
    {
        blink(1);
    }

#endif

    for (int i = 0;; ++i)
    {
        scheduler.Invoke();
#ifdef CANNARY
        for (size_t i = 0; i < cannarySize; ++i)
        {
            if (cannary[i] != 0xDEADBEEF)
            {
                blink(2);
            }
        }
#endif
    }
}

void loop()
{
}

void blink(byte type)
{
    const int period = type == 1 ? 500 : 1000;
    for (bool v = true;; v = !v)
    {
        digitalWrite((byte)Pins::RED_LED, v);
        delay(period);
    }
}