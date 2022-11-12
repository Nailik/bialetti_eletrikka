/******************************************************************
  sTune QuickPID Example
  This sketch does on-the-fly tunning and PID Digital Output control
  of a thermal heater (TIP31C). Tunning parameters are quickly
  determined and applied during the temperature ramp-up to setpoint.
  Open the serial plotter to view the graphical results.

bei 65 grad kocht das wasser schon

 ----------------------------------------------
 testTimeSec = 50
 SetPoint = 40
 outputStep = 1

  .00
 Sampl:        0.1000

 Pv Start:          0.000
 Pv Max:            50.418
 Pv Diff:           50.418

 Process Gain:      252.092
 Dead Time Sec:     1.209
 Tau Sec:           87.079

 Tau/Dead Time:     72.0 (easy to control)
 Tau/Sample Period: 870.8 (good sample rate)

 Tuning Method: NoOvershoot_PID
  Kp: 0.171
  Ki: 0.011  Ti: 14.926
  Kd: 1.654  Td: 0.104


   ----------------------------------------------
 SetPoint = 65 (overshoot in beginning big, later up to 67,5°C)
 outputStep = 10
 directIP

  Ip Pv:             34.250
 Pv Start:          0.000
 Pv Max:            162.689
 Pv Diff:           162.689

 Process Gain:      271.148
 Dead Time Sec:     1.207
 Tau Sec:           1.880

 Tau/Dead Time:     1.6 (easy to control)
 Tau/Sample Period: 56.4 (good sample rate)

 Tuning Method: NoOvershoot_PID
  Kp: 0.003
  Ki: 0.532  Ti: 0.006
  Kd: 1.658  Td: 0.002





 ________________________________________





 Testen:


 Heizen -> Druckaufbau -> mehr Energie notwendig
 sobald Druck > Kaffee Druck -> Druck fällt -> weniger Energie notwendig
 Wasser neigt sich Ende -> deutlich weniger Energie -> zufuhr Abschalten, kein Sprudeln

 eigenes soft pwm
 tau (differenz zwischen windows messen bei x energie)
 0 - 100% energie





  *****************************************************************/

#include <cstdint>
#include "sensors.h"
#include "pid.h"
#include "main.h"
#include <Arduino.h>

float Pid::Output = 480;
float Pid::SetPoint = 98;
float Pid::FirstSetPoint = 80;
float Pid::Offset = 1;
float Pid::MaximumOutput = 480;
//window size in ms
uint32_t Pid::WindowSize = 480;
float Pid::TempLimit = 120;
bool Pid::IsActive = false;

void Pid::setup() {

}

//läuft alle 100 ms
void Pid::loop() {
    if (IsActive) {//TODO reading input too fast
        Output = softPwm(Sensors::temp_1, Output, SetPoint, WindowSize);
    } else {
        digitalWrite(PIN_RELAY, LOW);
    }
}

//output = ms zeit im fenster
float Pid::softPwm(float input, float output, float setpoint, uint32_t windowSize) {
//TODO every 8 ms
    static bool forceStop;
    static float inputChangeLastWindowStart;
    static float inputWindowStart;

    if (input >= TempLimit) {
        //blink led
        //forceStop = true;
    }

    if (forceStop) {
      //  digitalWrite(PIN_RELAY, LOW);
      //  return -1;
    }

    //current time
    uint32_t msNow = millis();

    static uint32_t windowStartTime;

    //seit begin dess windows ist alle zeit vergangen -> neues window hat gestartet
    if (msNow - windowStartTime >= windowSize) {
        windowStartTime = msNow;
        inputChangeLastWindowStart = input - inputWindowStart;
        inputWindowStart = input;
    }

    // SSR optimum AC half-cycle controller
    static float optimumOutput;
    static bool reachedFirstSetpoint;
    static bool reachedSetpoint;

    if (input >= FirstSetPoint) reachedFirstSetpoint = true;
    if (input >= SetPoint) reachedSetpoint = true;

    //degree to power percentage ratio 1:1
    if(!reachedFirstSetpoint) {
        //1:1
        optimumOutput = MaximumOutput;
    } else {
        //1:1
        optimumOutput = MaximumOutput * (1-(input/SetPoint));
    }

    /*
    static bool reachedSetpoint;
    static bool reachedFirstSetpoint;

    if (input >= FirstSetPoint) reachedFirstSetpoint = true;

    //wurde setpoint erreicht?
    if (input >= setpoint) reachedSetpoint = true;

    if (reachedSetpoint && setpoint > 0 && input > setpoint) {
        //wurde setpoint schonmal erreicht und input immernoch größer als setpoint, dann output -8
        optimumOutput = output - 8;
    }
    else if (reachedSetpoint && setpoint > 0 && input < setpoint) {
    //wurde setpoint schonmal erreicht und input kleiner setpoint, dann output erhöhen
        optimumOutput = output + 8;
    }
    else if (reachedFirstSetpoint)  {
        //would setpoint be reached within overshoottime?
        if(input + (inputChangeLastWindowStart * (OvershootTime/WindowSize)) >= setpoint) {
            //decrease power
            optimumOutput = output - 8;
        } else {
            //increase power
            optimumOutput = output + 8;
        }
        //decrease power slowly if temp is rising
    }
    else {
        optimumOutput = output;
    }*/

    //optimum output = output, setpoint wurde noch nie erreicht
    if (optimumOutput < 0) optimumOutput = 0;

    if (optimumOutput > MaximumOutput) optimumOutput = MaximumOutput;

    //optimale output größer als restzeit des fensters
    if (optimumOutput > (msNow - windowStartTime)) {

        digitalWrite(PIN_RELAY, HIGH);
    } else if (optimumOutput < (msNow - windowStartTime)) {

        digitalWrite(PIN_RELAY, LOW);
    }

    //return optimumOutput
    return optimumOutput;
}