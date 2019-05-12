#include "ButtonsReaderTask.h"

#include "Arduino.h"
#include "ExpressTask.h"
#include "StaticAllocActivator.h"
#include "Globals.h"
#include "Buttons.h"

static const int FirstTick = 1000;
static const int NextTick = 600;

TASK_BEGIN(ButtonsReaderTask, { 
	byte adc; 
	byte prevButtons;
	byte current;
	unsigned long nextTick;
})

Buttons::Reset(::PressedButtons);

for (;;)
{
	adc = analogRead((byte)Pins::LCD_BUTTONS) >> 2;
	
    /*
	None - 321

	Right - 399
	Up - 788
	Down - 500
	Left - 929
	Select - 637
	*/
	
    if (adc < 90)
	{
		current = Buttons::None;
	}
	else if (adc < 110)
	{
        current = Buttons::Up;// Buttons::Right;
	}
	else if (adc < 140)
	{
        current = Buttons::Right; //Buttons::Down;
	}
	else if (adc < 180)
	{
		current = Buttons::Select;
	}
	else if (adc < 220)
	{
        current = Buttons::Left;// Buttons::Up;
	}
	else
	{
        current = Buttons::Down;// Buttons::Left;
	}

	if (current != Buttons::None)
	{
		if (prevButtons != current)
		{
			Buttons::Set(::PressedButtons, current);
			nextTick = millis() + FirstTick;
		}
		else
		{
			if (millis() > nextTick)
			{
				Buttons::Set(::PressedButtons, current);
				nextTick += NextTick;
			}
		}
	}
	prevButtons = current;

	TASK_SLEEP(10);
}

TASK_END;

bool RegisterButtonsReaderTask(Scheduler& scheduler)
{
	return scheduler.Register(InstanceOnce<ButtonsReaderTask>(), TaskPriority::RealTime);
}
