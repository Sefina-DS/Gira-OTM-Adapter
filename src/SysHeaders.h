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
#include <Adafruit_BME680.h>

//#include <AsyncTCP.h>
//#include <Update.h>

#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAddress.h>

// Datein

#include "SpiffsManager.h"
#include "ComWifi.h"
#include "ComWebserver.h"
#include "ComMqtt.h"
#include "ComBluetooth.h"
#include "ComSerial.h"
#include "SysFunctions.h"
#include "PluginSensors.h"
#include "PluginDetector.h"
#include "PluginDetectorSerial.h"
#include "SysTimer.h"
