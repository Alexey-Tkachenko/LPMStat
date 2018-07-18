#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <SD.h>

#include "Scheduler.h"

#include "NmeaReaderTask.h"
#include "SdWriter.h"
#include "ButtonsReaderTask.h"
#include "LcdIoTask.h"
#include "ControlTask.h"

void setup()
{
    Scheduler scheduler;
    RegisterNmeaReaderTask(scheduler);
    RegisterButtonsReaderTask(scheduler);
    RegisterLcdIoTask(scheduler);
    RegisterControlTask(scheduler);

    File f = SD.open("schedst.txt", FILE_WRITE);
    if (f)
    {
        scheduler.PrintStatistics(f);
        f.close();
    }
    

    for (;;)
    {
        scheduler.Invoke();
    }
}

void loop()
{
}
