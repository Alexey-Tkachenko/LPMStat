#include "NmeaReaderTask.h"
#include "Arduino.h"

#include "ExpressTask.h"
#include "StaticAllocActivator.h"
#include "Globals.h"
#include "Critical.h"
#include "TinyGPS-mod.h"
#include "Globals.h"

#include "LcdIo.h"
#include <SoftwareSerial.h>

static TinyGPS_mod gps;

static SoftwareSerial NMEA((byte)Pins::NMEA_RX, (byte)Pins::NMEA_TX);

TASK_BEGIN(NmeaReaderTask, 
{ 
	bool newData;
	int c;
})

for (;;)
{
	TASK_YIELD_WHILE(!NMEA.available());
	while (NMEA.available())
	{
		c = NMEA.read();
		if (gps.encode(c))
		{
			newData = true;
		}
	}
	
	if (newData)
	{
		newData = false;
        
		int year;
		byte month, day, hour, minute, second, hundredths;
		unsigned long age;
		
		gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
		year -= 2000;

		float lon, lat;
		gps.f_get_position(&lat, &lon, &age);
		int alt = (gps.altitude() + 50) / 100;

		NmeaData::DateTime = { (byte)year, month, day, hour, minute, second };
		NmeaData::Location = { lat, lon, alt };
	}
}

TASK_END;

bool RegisterNmeaReaderTask(Scheduler& scheduler)
{
	return scheduler.Register(Instance<NmeaReaderTask>(), TaskPriority::SensorPoll);
}

