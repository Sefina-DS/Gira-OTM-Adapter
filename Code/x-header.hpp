#pragma once

// Library´s

#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>

#include <SPIFFS.h>
#include <ArduinoJson.h>

#include <ESPAsyncWebServer.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

//#include <AsyncTCP.h>
//#include <Update.h>

#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAddress.h>

// Datein

#include "x-variablen.hpp"

#include "network-wifi.hpp"
#include "network-mqtt.hpp"
#include "network-mqtt-subscribe.hpp"
#include "network-bluetooth.hpp"

#include "sonderfunktionen.hpp"

#include "serial_transfer.hpp"
#include "serial_filter.hpp"

#include "timer.hpp"
#include "spiffs_filemanager.hpp"

#include "sensoren.hpp"

#include "webserver.hpp"
#include "webserver-funktion.hpp"
//#include "webserver-get-var.hpp"