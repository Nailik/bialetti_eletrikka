//
// Created by kilia on 11.08.2022.
//

#include "ESPAsyncWebServer.h"



#ifndef UNTITLED_WEBSERVER_H
#define UNTITLED_WEBSERVER_H

class WebServer {
public:
    static void setup();
    static void sendFileResponse(const String &uri, const String &type, AsyncWebServerRequest *request);
    static void loop();
};

#endif //UNTITLED_WEBSERVER_H