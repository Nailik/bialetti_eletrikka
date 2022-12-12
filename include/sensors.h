//
// Created by kilia on 14.08.2022.
//

#ifndef UNTITLED_SENSORS_H
#define UNTITLED_SENSORS_H

class Sensors {
public:
    static int button;
    static int relay;
    static int led;
    static float temp_1;
    static float temp_2;
    static float temp_3;
    static void setup();
    static void loop();
    static void isr_btn();
};

#endif //UNTITLED_SENSORS_H
