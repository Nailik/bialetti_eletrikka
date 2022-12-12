//
// Created by kilia on 27.09.2022.
//

#ifndef SOFTWARE_PID_H
#define SOFTWARE_PID_H

#include "Arduino.h"


class Pid {
private:
    static int value;
public:
    static float Output;
    static float SetPoint;
    static float FirstSetPoint;
    static float MaximumOutput;
    static float TempLimit;
    static int ZCTime;
    static unsigned long ZCTimeLast;
    static float Offset;
    static uint32_t WindowSize;
    static void IRAM_ATTR read();

    static void setup();
    static void loop();
    static void setPidPercentage(int percentage);
    static int getPidPercentage();
    static float softPwm(float input, float output, float setpoint, uint32_t windowSize);
};


#endif //SOFTWARE_PID_H
