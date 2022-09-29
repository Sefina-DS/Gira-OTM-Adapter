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

#include "SysVars.h"

#include "SpiffsManager.h"
#include "ComWifi.h"
#include "ComWebserver.h"
#include "ComWebserverFunctions.h"
#include "ComMqtt.h"
#include "ComMqttSubscribe.h"
#include "ComBluetooth.h"
#include "ComSerialTransfer.h"
#include "ComSerialFilter.h"
#include "AdvancedFunctions.h"
#include "PluginSensors.h"
#include "SysTimer.h"