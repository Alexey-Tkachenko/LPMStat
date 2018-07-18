#include "ControlTask.h"
#include "Arduino.h"
#include "ExpressTask.h"
#include "StaticAllocActivator.h"

TASK_BEGIN(ControlTask, {})

for (;;)
{
    TASK_YIELD();
}

TASK_BODY_END

TASK_CLASS_END

void RegisterControlTask(Scheduler & scheduler)
{
    scheduler.Register(Instance<ControlTask>());
}
