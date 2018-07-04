#include <SPI.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include "Scheduler.h"

#include "NmeaReaderTask.h"
#include "SdWriter.h"


void setup()
{
    Scheduler scheduler;
    RegisterNmeaReaderTask(scheduler);
    SdWriter &writer = SdWriter::Open(DateTime());
    for (;;)
    {
        writer << millis() << SdWriter::eol;
        scheduler.Invoke();
    }
}

void loop()
{
}
