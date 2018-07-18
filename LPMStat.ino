#include <SPI.h>
#include <SoftwareSerial.h>
#include <SD.h>

#include "Scheduler.h"

#include "NmeaReaderTask.h"
#include "SdWriter.h"
#include "ButtonsReaderTask.h"
#include "LcdIoTask.h"
#include "ControlTask.h"

void setup() __ATTR_NORETURN__;

void setup()
{
    Scheduler scheduler;

    RegisterNmeaReaderTask(scheduler);
    RegisterButtonsReaderTask(scheduler);
    RegisterLcdIoTask(scheduler);
    RegisterControlTask(scheduler);

    for (;;)
    {
        scheduler.Invoke();
    }
}

void loop()
{
}
