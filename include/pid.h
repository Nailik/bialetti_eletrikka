//
// Created by kilia on 27.09.2022.
//

#ifndef SOFTWARE_PID_H
#define SOFTWARE_PID_H

class Pid {
private:
public:
    static float Output;
    static float SetPoint;
    static float FirstSetPoint;
    static float MaximumOutput;
    static float TempLimit;
    static float Offset;
    static bool IsActive;
    static uint32_t WindowSize;

    static void setup();
    static void loop();
    static float softPwm(float input, float output, float setpoint, uint32_t windowSize);
};


#endif //SOFTWARE_PID_H
