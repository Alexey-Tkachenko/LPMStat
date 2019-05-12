#ifndef _LPM_IO_TASK_H_
#define _LPM_IO_TASK_H_

#include "Scheduler.h"

void RegisterLpmIoTask(Scheduler& scheduler);

class Magnitude
{
    int value;
public:
    Magnitude();

    explicit Magnitude(int value);

    inline int Value() const
    {
        return value;
    }

    bool operator<(const Magnitude& other);
    
};


bool LpmStartMeasure(WaitHandles::ValueHolder<Magnitude>** target);

Magnitude LpmGetCalibration();

#endif

