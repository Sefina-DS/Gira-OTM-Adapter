#include "x-header.hpp"

Config config;
WebServer webserver;
AsyncWebServer *server;
WiFiClient espClient;
PubSubClient client(espClient);
int seri_status = 0;
unsigned long timer_time = 0;
unsigned long timer_alarm = 0;
// unsigned long timer_bluetooth = 0;

// TaskHandle_t Core0TaskHnd;

//#include "include-variables.h"

void setup()
{
  Serial.begin(9600);
  Serial.println();

  pinMode(led_esp, OUTPUT);
  pinMode(led_detector, OUTPUT);
  pinMode(serial_detector_aktiv, OUTPUT);

  pinMode(serial_detector_scan, INPUT);

  digitalWrite(led_esp, HIGH);
  digitalWrite(led_detector, HIGH);
  digitalWrite(serial_detector_aktiv, HIGH);

  spiffs_starten();

  spiffs_scan();
  spiffs_config_read();

  server = new AsyncWebServer(80);
  webserver_art();

  Serial.println();
  Serial.print(config.esp_name);
  Serial.println(" wird gestartet");
  Serial.println();

  wlan_config();

  // config.bluetooth = true;
  config.seriel = true;

  if (config.seriel == true)
  {
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  }
  if (config.mqtt)
  {
    mqtt_config();
  }
  if (config.bluetooth)
  {
    // bluetooth_config();
  }
}

void loop()
{
  // Rücksetzen Alarmtimer
  if (millis() >= timer_alarm &&
      timer_alarm != 0)
  {
    timer_alarm = 0;
  }
  // 5 Sekunden intervall
  if (millis() >= timer_time)
  {
    timer_time = millis() + 5000;
    timer_funktion();
  }
  // MQTT Funktion
  if (config.mqtt)
  {
    client.loop();
  }
  // Seriele Funktionen
  serial_read();
  if (seri_status > 0)
  {
    serial_send("");
  }

  // bluetooth !
  if (config.bluetooth &&
      timer_bluetooth + 20000 < millis())
  {
    // bluetooth_scan();
  }
}
