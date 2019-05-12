#include "ControlTask.h"
#include "Arduino.h"
#include "ExpressTask.h"
#include "StaticAllocActivator.h"
#include "Buttons.h"
#include "Globals.h"
#include "LcdIoTask.h"
#include "LcdIo.h"
#include <EEPROM.h>
#include "Format.h"
#include "LpmIoTask.h"
#include "Median.h"
#include "SdWriter.h"

#include <avr/pgmspace.h>

static const char MenuItem1[] PROGMEM = "Date/Time";
static const char MenuItem2[] PROGMEM = "Location";
static const char MenuItem3[] PROGMEM = "Parameters";
static const char MenuItem4[] PROGMEM = "Measure";

static const char* const Menu[] PROGMEM = {
    MenuItem1,
    MenuItem2,
    MenuItem3,
    MenuItem4,
};


static const byte Intervals[8] PROGMEM = {
    1,
    2,
    3,
    6,
    2*6,
    3*6,
    5*6,
    10*6
};

struct IntervalPointer
{
    byte Position;

    int Next()
    {
        if (Position < 7)
        {
            ++Position;
        }
        return Value();
    }

    int Prev()
    {
        if (Position > 0)
        {
            --Position;
        }
        return Value();
    }

    int Value() const
    {
        return pgm_read_byte(Intervals + Position);
    }
};

TASK_BEGIN(ControlTask, { 
byte SelectedItem;
byte buttons;

int MeasurementCount;
int Interval;
char bufferA[17];
char bufferB[17];
byte nmeaPrevVersion;

byte pos_Location;
bool updateRequired;

byte params_Position;
byte settings_Position;

uint32_t measureDelay;
uint32_t measureSkip;
uint16_t measureCounter;
SdWriter* writer;
unsigned long now;

})


LcdCommand::Write(F("Welcome")).Enqueue();
TASK_SLEEP(2 SECONDS);

EEPROM.get(0, Parameters);
Parameters.Count = constrain(Parameters.Count, 1, 999);
Parameters.Interval = constrain(Parameters.Interval, 1, 360);

_menu:
LcdSetState(true);
PrintMenu();

for (;;)
{
    buttons = Buttons::ReadRelease(PressedButtons);
 
    if (Buttons::IsPressed(Buttons::Select, buttons))
    {
        goto _operations;
    }
    else if (Buttons::IsPressed(Buttons::Up, buttons))
    {
        SelectedItem = PrevItem(SelectedItem);
        PrintMenu();
        continue;
    }
    else if (Buttons::IsPressed(Buttons::Down, buttons))
    {
        SelectedItem = NextItem(SelectedItem);
        PrintMenu();
        continue;
    }
    else
    {
        TASK_SLEEP(100);
    }
}

_operations:
LcdCommand::Clear().Enqueue();

updateRequired = true;

for (;; updateRequired = false)
{
    if (SelectedItem == 0)
    {
        // Date/Time
        if (Buttons::ReadRelease(PressedButtons) & Buttons::Select)
        {
            goto _menu;
        }
        if (NmeaData::Version)
        {
            if (NmeaData::Version != nmeaPrevVersion)
            {
                nmeaPrevVersion = NmeaData::Version;
                FormatDateTime(bufferA, bufferB, 16, NmeaData::DateTime);
                LcdCommand::Write(bufferA, bufferB).Enqueue();
            }
        }
        else if (updateRequired)
        {
            PrintNoNmea();
        }
    }
    else if (SelectedItem == 1)
        // Location
    {
        if (Buttons::ReadRelease(PressedButtons, Buttons::Select))
        {
            goto _menu;
        }
        else if (Buttons::ReadRelease(PressedButtons, Buttons::Up))
        {
            if (pos_Location == 0)
            {
                pos_Location = 2;
            }
            else
            {
                --pos_Location;
            }
            updateRequired = true;
        }
        else if (Buttons::ReadRelease(PressedButtons, Buttons::Down))
        {
            if (++pos_Location == 3)
            {
                pos_Location = 0;
            }
            updateRequired = true;
        }
        else
        {
            Buttons::ReadRelease(PressedButtons, Buttons::Left | Buttons::Right);
        }

        if (NmeaData::Version)
        {
            if (NmeaData::Version != nmeaPrevVersion || updateRequired)
            {
                nmeaPrevVersion = NmeaData::Version;
                PrintLocation();
            }
        }
        else if (updateRequired)
        {
            PrintNoNmea();
        }
    }
    else if (SelectedItem == 2)
    { 
        // Parameters

        if (updateRequired)
        {
            _params_redraw:
            char *ptrA = bufferA;
            byte sizeA = 16;

            char *ptrB = bufferB;
            byte sizeB = 16;


            uint16_t interval = Parameters.Interval;

            put(ptrA, sizeA, '*');
            put(ptrB, sizeB, params_Position == 0 ? '^' : ' ');

            put(ptrA, sizeA, ' ');
            put(ptrB, sizeB, ' ');

            put(ptrA, sizeA, (char)('0' + interval / 60));
            put(ptrB, sizeB, params_Position == 1 ? '^' : ' ');

            interval %= 60;

            put(ptrA, sizeA, (char)('0' + interval / 6));
            put(ptrB, sizeB, params_Position == 2 ? '^' : ' ');

            put(ptrA, sizeA, ':');
            put(ptrB, sizeB, ' ');

            put(ptrA, sizeA, (char)('0' + interval % 6));
            put(ptrB, sizeB, params_Position == 3 ? '^' : ' ');

            put(ptrA, sizeA, '0');
            put(ptrB, sizeB, ' ');

            put(ptrA, sizeA, ' ');
            put(ptrB, sizeB, ' ');

            put(ptrA, sizeA, '/');
            put(ptrB, sizeB, ' ');

            put(ptrA, sizeA, ' ');
            put(ptrB, sizeB, ' ');

            uint16_t count = Parameters.Count;

            put(ptrA, sizeA, (char)('0' + count / 100));
            put(ptrB, sizeB, params_Position == 4 ? '^' : ' ');

            count %= 100;
            put(ptrA, sizeA, (char)('0' + count / 10));
            put(ptrB, sizeB, params_Position == 5 ? '^' : ' ');

            put(ptrA, sizeA, (char)('0' + count % 10));
            put(ptrB, sizeB, params_Position == 6 ? '^' : ' ');

            put(ptrA, sizeA, '\0');
            put(ptrB, sizeB, '\0');
            
            LcdCommand::Write(bufferA, bufferB).Enqueue();
        }

        byte b = Buttons::ReadRelease(PressedButtons);
        if (Buttons::IsPressed(b, Buttons::Select))
        {
            EEPROM.put(0, Parameters);
            goto _menu;
        }
        else if (Buttons::IsPressed(b, Buttons::Left))
        {
            if (params_Position == 0)
            {
                params_Position = 6;
            }
            else
            {
                --params_Position;
            }
            goto _params_redraw;
        }
        else if (Buttons::IsPressed(b, Buttons::Right))
        {
            if (++params_Position == 7)
            {
                params_Position = 0;
            }
            goto _params_redraw;
        }
        else if (Buttons::IsPressed(b, Buttons::Down))
        {
            switch (params_Position)
            {
            case 0: // *
                Parameters.Interval = shortcutInterval.Prev();
                break;

                // ##:#0
            case 1:
                if ((Parameters.Interval -= 60) <= 0)
                {
                    Parameters.Interval = 1;
                }
                break;
            case 2:
                if ((Parameters.Interval -= 6) <= 0)
                {
                    Parameters.Interval = 1;
                }
                break;
            case 3:
                if ((Parameters.Interval -= 1) <= 0)
                {
                    Parameters.Interval = 1;
                }
                break;

                // ###
            case 4:
                if ((Parameters.Count -= 100) <= 0)
                {
                    Parameters.Count = 1;
                }
                break;
            case 5:
                if ((Parameters.Count -= 10) <= 0)
                {
                    Parameters.Count = 1;
                }
                break;
            case 6:
                if ((Parameters.Count -= 1) <= 0)
                {
                    Parameters.Count = 1;
                }
                break;
            }
            goto _params_redraw;
        }
        else if (Buttons::IsPressed(b, Buttons::Up))
        {
            switch (params_Position)
            {
            case 0: // *
                Parameters.Interval = shortcutInterval.Next();
                break;

                // ##:#0
            case 1:
                if ((Parameters.Interval += 60) >= 360)
                {
                    Parameters.Interval = 360;
                }
                break;
            case 2:
                if ((Parameters.Interval += 6) >= 360)
                {
                    Parameters.Interval = 360;
                }
                break;
            case 3:
                if ((Parameters.Interval += 1) >= 360)
                {
                    Parameters.Interval = 360;
                }
                break;

                // ###
            case 4:
                if ((Parameters.Count += 100) >= 999)
                {
                    Parameters.Count = 999;
                }
                break;
            case 5:
                if ((Parameters.Count += 10) >= 999)
                {
                    Parameters.Count = 999;
                }
                break;
            case 6:
                if ((Parameters.Count += 1) >= 999)
                {
                    Parameters.Count = 999;
                }
                break;
            }
            goto _params_redraw;
        }

    }
    else if (SelectedItem == 3)
    {        
        if (writer == nullptr)
        {
            if (NmeaData::Version != 0)
            {
                LcdCommand::Write(F("Init SD...")).Enqueue();
                TASK_SLEEP(50);
                writer = SdWriter::Open(NmeaData::DateTime);
                if (writer == nullptr)
                {
                    LcdCommand::WriteAt(0, 7, F(": Error")).Enqueue();
                    TASK_SLEEP(3 SECONDS);
                    goto _menu;
                }
                else
                {
                    Wrap(writer)
                        << F("Date/Time:   ") << NmeaData::DateTime << eol;
                    Wrap(writer)
                        << F("Location:    ") << NmeaData::Location << eol;
                    Wrap(writer)
                        << F("Calibration: ") << Magnitude(LpmGetCalibration()) << eol;
                    TASK_YIELD();
                    Wrap(writer)
                        << flush;
                    TASK_YIELD();
                    LcdCommand::WriteAt(0, 7, F(": Success")).Enqueue();
                    TASK_SLEEP(500);
                }
            }
            else
            {
                PrintNoNmea();
                TASK_SLEEP(3000);
            }
        }
        else
        {
            LcdCommand::Write(F("SD ready")).Enqueue();
            TASK_SLEEP(100);
        }

        measureCounter = Parameters.Count;
        measureDelay = millis() + 3000;
        measureSkip = measureDelay + 2000;

        Wrap(writer) << eol << eol
            << F("Measurements: ") << measureCounter << eol
            << F("Interval    : ") << (Parameters.Interval * 10) << F("s") << eol
            << eol;
        static const __FlashStringHelper *tab = F("\t");
        
        Wrap(writer)
            << F("Date/Time") << tab
            << F("Location") << tab
            << F("Last mag") << tab
            << F("Median<7> filtered mag") << eol
            << eol << flush;

        median.Reset();
        
        for (;;)
        {
            if (Buttons::ReadRelease(PressedButtons) & Buttons::Select)
            {
                Wrap(writer) << flush;
                goto _menu;
            }
        
            now = millis();

            if (now < measureSkip)
            {
                if (now >= measureDelay - 900)
                {
                    measureSkip = 0;

                    LcdSetState(false);
                    TASK_SLEEP(1000);
                    LcdCommand::CharAt(1, 15, SYMBOL_START).Enqueue();
                    LpmStartMeasure(&Measurement);
                }
            }
            else
            {
                while (now > measureSkip)
                {
                    measureDelay += Parameters.Interval * 10000;
                    measureSkip = measureDelay + 1000;
                }
            }

            if (Measurement && Measurement->IsSignalled())
            {
                LcdCommand::CharAt(1, 15, SYMBOL_STOP).Enqueue();

                TASK_SLEEP(1000);

                static Magnitude mag;
                mag = Measurement->Get();
                median.write(mag);
                static Magnitude med;
                med = median.get();

                static char* ptr;
                ptr = bufferA;
                static byte size;
                size = 16;
                static char* ptr2;
                ptr2 = FormatTime(ptr, size, NmeaData::DateTime);

                size -= (ptr2 - ptr);
                put(ptr2, size, ' ');
                put(ptr2, size, ' ');
                FormatNumber(ptr2, size, measureCounter);
                --measureCounter;

                ptr = bufferB;
                size = 16;
                FormatMagnitude(ptr, size, mag);
                put(ptr, size, ' ');
                put(ptr, size, '<');
                FormatMagnitude(ptr, size, med);
                put(ptr, size, '>');
                put(ptr, size, '\0');

                Wrap(writer)
                    << NmeaData::DateTime << tab
                    << NmeaData::Location << tab
                    << mag << tab
                    << med << eol;

                LcdSetState(true);
                LcdCommand::Write(bufferA, bufferB).Enqueue();
                TASK_SLEEP(50);
            }

            if (measureCounter == 0)
            {
                LcdSetState(true);
                Wrap(writer) << flush;

                LcdCommand::CharAt(1, 15, 'R').Enqueue();
                while (true)
                {
                    if (Buttons::ReadRelease(PressedButtons) & Buttons::Select)
                    {
                        break;
                    }
                    digitalWrite((byte)Pins::RED_LED, 1);
                    TASK_SLEEP(100);
                    digitalWrite((byte)Pins::RED_LED, 0);
                    TASK_SLEEP(900);
                }

                goto _menu;
            }

            TASK_SLEEP(100);
        }
        // Measure
    }
    
    TASK_YIELD();
}
    

TASK_BODY_END

WaitHandles::ValueHolder<Magnitude>* Measurement;

struct 
{
    int16_t Count;
    
    int16_t Interval;
    
} Parameters;

Median<Magnitude, 7, byte> median;

IntervalPointer shortcutInterval;

void PrintNoNmea()
{
    LcdCommand::Write(F("No NMEA data")).Enqueue();
}

void PrintLocation() 
{
    static const char dms[] PROGMEM = { SYMBOL_DEG, SYMBOL_MIN, SYMBOL_SEC };
    static const char lat[] PROGMEM = { 'S', 'N' };
    static const char lon[] PROGMEM = { 'W', 'E' };

_restart:
    switch (pos_Location)
    {
    case 0:
        *FormatDMS(bufferA, 16, NmeaData::Location.Latitude, lat, dms) = 0;
        *FormatDMS(bufferB, 16, NmeaData::Location.Longitude, lon, dms) = 0;
        break;
    case 1:
        *FormatDDD(bufferA, 16, NmeaData::Location.Latitude, lat, SYMBOL_DEG) = 0;
        *FormatDDD(bufferB, 16, NmeaData::Location.Longitude, lon, SYMBOL_DEG) = 0;
        break;
    case 2:
        {
            byte size = 16;
            char *ptr = bufferB;
            FormatNumber(ptr, size, NmeaData::Location.Altitude);
            put(ptr, size, 'm');
            put(ptr, size, '\0'); 
            bufferA[0] = 'A';
            bufferA[1] = 'L';
            bufferA[2] = 'T';
            bufferA[3] = '\0';
        }
        break;
    default:
        pos_Location = 0;
        goto _restart;
        return;
    }

    LcdCommand::Write(bufferA, bufferB).Enqueue();
}


byte NextItem(byte selected)
{
    return (selected + 1) & 3;
}

byte PrevItem(byte selected)
{
    return (selected - 1) & 3;
}

void PrintMenu()
{
    LcdCommand::Clear().Enqueue();
    LcdCommand::WriteAt(0, 2, (const __FlashStringHelper*)pgm_read_ptr(Menu + SelectedItem)).Enqueue();
    LcdCommand::WriteAt(1, 2, (const __FlashStringHelper*)pgm_read_ptr(Menu + NextItem(SelectedItem))).Enqueue();
    LcdCommand::CharAt(0, 0, '>').Enqueue();
}


TASK_CLASS_END

void RegisterControlTask(Scheduler & scheduler)
{
    scheduler.Register(InstanceOnce<ControlTask>());
}
