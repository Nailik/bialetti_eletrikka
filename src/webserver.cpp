#include <Arduino.h>

#include "webserver.h"
#include "sensors.h"
#include "actions.h"
#include "pid.h"
#include "WebSerialLite.h"

#if defined(ESP8266)

#include <ESP8266WiFi.h>
#include <FS.h>           //https://github.com/esp8266/Arduino

#else
#include <WiFi.h>
#include <SPIFFS.h>
#endif

#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <ArduinoOTA.h>

AsyncWebServer server(80);
DNSServer dns;

static void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void WebServer::setup() {

    Serial.println("WebServer start");
    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    AsyncWiFiManager wifiManager(&server, &dns);
    //reset saved settings
    //wifiManager.resetSettings();

    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect();
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();


    //update remote
    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";

        SPIFFS.end();
        Serial.println("Start updating " + type);
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();

    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("WiFi Failed!\n");
        return;
    }

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.println("WebServer setup");

    SPIFFS.begin();
    //initial page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendFileResponse("/index.html", "text/html", request);
    });
    //file requests and respones
    server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendFileResponse("/bootstrap.min.css", "text/css", request);
    });
    server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendFileResponse("/bootstrap.min.js", "text/js", request);
    });
    server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendFileResponse("/index.html", "text/html", request);
    });
    server.on("/chart.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendFileResponse("/chart.js", "text/js", request);
    });
    server.on("/luxon.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendFileResponse("/luxon.js", "text/js", request);
    });
    server.on("/chartjs-adapter-luxon.js", HTTP_GET, [](AsyncWebServerRequest *request) {
        sendFileResponse("/chartjs-adapter-luxon.js", "text/js", request);
    });



    //get requests for sensors etc
    server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
        String response = "";

        if (request->hasParam("type")) {
            String type = request->getParam("type")->value();

            if (type == "output") {
                //relay state
                response = "{\"x\":" + String(millis()) + ",\"y\":" + String(Pid::Output) + "}";
            } else if (type == "pid") {
                //relay state
                response = String(Pid::status);
            } else if (type == "setPoint") {
                //relay state
                response = "{\"x\":" + String(millis()) + ",\"y\":" + String(Pid::SetPoint) + "}";
            } else if (type == "led_1") {
                //led state
                response = String(Sensors::led_1);
            } else if (type == "led_2") {
                //led state
                response = String(Sensors::led_2);
            } else if (type == "temp_1") {
                //temp_base state
                response = "{\"x\":" + String(millis()) + ",\"y\":" + String(Sensors::temp_1) + "}";
            } else if (type == "temp_2") {
                //temp_chip state
                response = "{\"x\":" + String(millis()) + ",\"y\":" + String(Sensors::temp_2) + "}";
            } else if (type == "relay") {
                //temp_chip state
                response = "{\"x\":" + String(millis()) + ",\"y\":" + String(Sensors::relay) + "}";
            }  else {
                request->send(500, "text/plain", "wrong type");
                return;
            }
            request->send(200, "text/plain", response);
        } else {
            request->send(500, "text/plain", "type missing");
        }

    });

    //action requests for sensors etc
    server.on("/set", HTTP_PUT, [](AsyncWebServerRequest *request) {
        String response = "";

        Serial.println("set action");

        if (request->hasParam("type")) {
            String type = request->getParam("type")->value();

            if (type == "pid") {
                Serial.println("toggle relay");
                Actions::toggleRelay();
                response = String(Pid::status);
            } else if (type == "led_1") {
                response = String(Sensors::led_1);
            } else if (type == "led_2") {
                response = String(Sensors::led_2);
            } else {
                request->send(500, "text/plain", "wrong type");
                return;
            }
            request->send(200, "text/plain", response);
        } else {
            request->send(500, "text/plain", "type missing");
        }

    });

    server.onNotFound(notFound);



    // WebSerial is accessible at "<IP Address>/webserial" in browser
    WebSerial.begin(&server);

    server.begin();

    server.begin();
}


/**
 * sends file as response to request automaticall uses gzip file if client accepts it
 *
 * @param uri String to path
 * @param type Type of file data
 * @param request request pointer
 */
void WebServer::sendFileResponse(const String &uri, const String &type, AsyncWebServerRequest *request) {
    Serial.println("response3 " + uri + " | " + type);
    if (request->hasHeader("Accept-Encoding") && request->getHeader("Accept-Encoding")->value().indexOf("gzip") != -1) {
        AsyncWebServerResponse *response = request->beginResponse(SPIFFS, uri /*+ ".gz"*/, type);
        //    response->addHeader("Content-Encoding", "gzip");
        request->send(response);
    } else {
        request->send(SPIFFS, uri);
    }
}

void WebServer::loop() {
    ArduinoOTA.handle();
}