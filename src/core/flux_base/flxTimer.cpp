

#include "flxTimer.h"
template <> void flxObjDescriptor<flxTimer>::setupDescriptor()
{
    _name = "Timer";
    _description = "A timer that can be used to trigger actions at a set interval.";
}