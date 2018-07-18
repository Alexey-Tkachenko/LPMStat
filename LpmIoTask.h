#ifndef _LPM_IO_TASK_H_
#define _LPM_IO_TASK_H_

#include "Scheduler.h"

void RegisterLpmIoTask(Scheduler& scheduler);

bool LpmStartMeasure(WaitHandles::ValueHolder<int>** target);

int LpmGetCalibration();

#endif

