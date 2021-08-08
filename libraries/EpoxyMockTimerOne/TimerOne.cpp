/*
 * MIT License
 * Copyright (c) 2021 Brian T. Park
 */

#include "TimerOne.h"

void (*TimerOne::isrCallback)() = TimerOne::isrDefaultUnused;

TimerOne Timer1;
