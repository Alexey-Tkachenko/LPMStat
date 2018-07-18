#include "LcdIoTask.h"
#include "Arduino.h"

#include "ExpressTask.h"
#include "Globals.h"
#include "StaticAllocActivator.h"
#include "LcdIo.h"
#include "WString.h"

WaitHandles::DataQueue<LcdCommand, 16> command;

TASK_BEGIN(LcdIoTask, { LcdCommand cmd; byte backlight; } _)
_.backlight = 0xFF;
for (;;)
{
    TASK_WAIT_VALUE(&command, _.cmd);

    switch (_.cmd.Code)
    {
    case LcdCommandCode::None:
        break;
    case LcdCommandCode::Clear:
        lcdClear();
        break;
    case LcdCommandCode::Write:
        lcdPrint(_.cmd.LineA, _.cmd.LineB);
        break;
    case LcdCommandCode::WriteKeep:
        lcdPrint(_.cmd.LineA, _.cmd.LineB, false);
        break;
    case LcdCommandCode::WriteFlash:
        lcdPrint(_.cmd.LineAFlash, _.cmd.LineBFlash);
        break;
    case LcdCommandCode::WriteKeepFlash:
        lcdPrint(_.cmd.LineAFlash, _.cmd.LineBFlash, false);
        break;
    case LcdCommandCode::WriteAt:
        lcdPutLineAt(_.cmd.Row, _.cmd.Column, _.cmd.Line);
        break;
    case LcdCommandCode::WriteAtFlash:
        lcdPutLineAt(_.cmd.Row, _.cmd.Column, _.cmd.LineFlash);
        break;
    case LcdCommandCode::CharAt:
        lcdPutCharAt(_.cmd.Row, _.cmd.Column, _.cmd.Char);
        break;
    case LcdCommandCode::Backlight:
        lcdSetBacklight(_.cmd.BacklightValue ? _.backlight : 0);
        break;
    case LcdCommandCode::BacklightPwm:
        lcdSetBacklight(_.backlight = _.cmd.BacklightPwmValue);
        break;
    }

    TASK_YIELD();
}
TASK_END;

void RegisterLcdIoTask(Scheduler &scheduler)
{
    scheduler.Register(Instance<LcdIoTask>());
}

bool LcdCommandEnqueue(const LcdCommand & command)
{
    return ::command.Put(command);
}

static LcdCommand InitNone()
{
    LcdCommand cmd;
    memset(&cmd, 0, sizeof(cmd));
    cmd.Code = LcdCommandCode::None;
    return cmd;
}

const LcdCommand & LcdCommand::None()
{
    static LcdCommand noneCommand = InitNone();
    return noneCommand;
}

LcdCommand LcdCommand::Write(const char * first, const char * second)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::Write;
    cmd.LineA = first;
    cmd.LineB = second;
    return cmd;
}

LcdCommand LcdCommand::Write(const __FlashStringHelper * first, const __FlashStringHelper * second)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::WriteFlash;
    cmd.LineAFlash = first;
    cmd.LineBFlash = second;
    return cmd;
}

LcdCommand LcdCommand::WriteKeep(const char * first, const char * second)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::WriteKeep;
    cmd.LineA = first;
    cmd.LineB = second;
    return cmd;
}

LcdCommand LcdCommand::WriteKeep(const __FlashStringHelper * first, const __FlashStringHelper * second)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::WriteKeepFlash;
    cmd.LineAFlash = first;
    cmd.LineBFlash = second;
    return cmd;;
}

LcdCommand LcdCommand::WriteAt(byte row, byte column, const char * str)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::WriteAt;
    cmd.Row = row;
    cmd.Column = column;
    cmd.Line = str;
    return cmd;
}

LcdCommand LcdCommand::WriteAt(byte row, byte column, const __FlashStringHelper * str)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::WriteAtFlash;
    cmd.Row = row;
    cmd.Column = column;
    cmd.LineFlash = str;
    return cmd;
}

LcdCommand LcdCommand::CharAt(byte row, byte column, char c)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::CharAt;
    cmd.Row = row;
    cmd.Column = column;
    cmd.Char = c;
    return cmd;
}

LcdCommand LcdCommand::Backlight(bool value)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::Backlight;
    cmd.BacklightValue = value;
    return cmd;
}

LcdCommand LcdCommand::BacklightPwm(byte value)
{
    LcdCommand cmd;
    cmd.Code = LcdCommandCode::BacklightPwm;
    cmd.BacklightPwmValue = value;
    return cmd;
}

void LcdCommand::Enqueue()
{
    LcdCommandEnqueue(*this);
}
