//
// Created by kilia on 27.09.2022.
//

#ifndef SOFTWARE_PID_H
#define SOFTWARE_PID_H


#include "sTune.h"
#include <QuickPID.h>

class Pid {
private:
    static uint32_t settleTimeSec;
    static uint32_t testTimeSec;  // runPid interval = testTimeSec / samples
    static const uint16_t samples;
    static const float inputSpan;
    static const uint32_t outputSpan;
    static float outputStart;
    static float outputStep;
    static float tempLimit;
    static uint8_t debounce;
    // variables
    static float Input;
    static float Kp;
    static float Ki;
    static float Kd;

    static sTune tuner;
    static QuickPID quickPid;
public:
    static float Output;
    static float SetPoint;

    static bool isActive;
    static int status;

    static void setup();
    static void loop();
};


#endif //SOFTWARE_PID_H
