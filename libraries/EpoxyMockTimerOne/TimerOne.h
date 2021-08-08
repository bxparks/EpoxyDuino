/*
 * MIT License
 * Copyright (c) 2021 Brian T. Park
 */

/**
 * @file Simple mock implementation of the TimerOne library
 * (https://github.com/PaulStoffregen/TimerOne) to allow code written against
 * that library to compile under EpoxyDuino.
 */

#ifndef EPOXY_MOCK_TIMER_ONE_H
#define EPOXY_MOCK_TIMER_ONE_H

#include <stdint.h> // uint8_t

/**
 * Mock implementation of the TimerOne class from the TimerOne library
 * (https://github.com/PaulStoffregen/TimerOne). Since TimerThree
 * (https://github.com/PaulStoffregen/TimerThree) has the exact same API, this
 * class can be substituted for TimerThree for mocking purposes.
 */
class TimerOne {
  public:
    //-------------------------------------------------------------------------
    // Configuration
    //-------------------------------------------------------------------------

    void initialize(unsigned long microseconds=1000000) {
      setPeriod(microseconds);
    }

    void setPeriod(unsigned long /*microseconds*/) {}

    //-------------------------------------------------------------------------
    // Run Control
    //-------------------------------------------------------------------------

    void start() {}
    void stop() {}
    void restart() {}
    void resume() {}

    //-------------------------------------------------------------------------
    // PWM outputs
    //-------------------------------------------------------------------------

    void setPwmDuty(char /*pin*/, unsigned int /*duty*/) {}
    void pwm(char /*pin*/, unsigned int /*duty*/) {}
    void pwm(char /*pin*/, unsigned int /*duty*/,
        unsigned long /*microseconds*/) {}
    void disablePwm(char /*pin*/) {}

    //-------------------------------------------------------------------------
    // Interrupt Function
    //-------------------------------------------------------------------------

    void attachInterrupt(void (*isr)()) { isrCallback = isr; }

    void attachInterrupt(void (*isr)(), unsigned long microseconds) {
      if (microseconds > 0) setPeriod(microseconds);
      attachInterrupt(isr);
    }

    void detachInterrupt() {}

    static void (*isrCallback)(); // this is a static variable, not a function
    static void isrDefaultUnused() {}
};

extern TimerOne Timer1;

#endif
