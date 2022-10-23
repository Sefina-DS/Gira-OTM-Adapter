#include "SysHeaders.h"

Config config;

AsyncWebServer *server;
WiFiClient espClient;
PubSubClient client(espClient);
int seri_status = 0;
unsigned long timer_time = 0;
unsigned long timer_alarm = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println();

  pinMode(output_led_esp, OUTPUT);
  pinMode(output_led_detector, OUTPUT);
  pinMode(output_comport_activ, OUTPUT);

  pinMode(input_comport_activ, INPUT);
  pinMode(input_webportal, INPUT);
  pinMode(input_reset, INPUT);

  led_flash_timer(5000, 0, 1);

  spiffs_starten();

  spiffs_scan();
  spiffs_config_read_part_a();
  spiffs_config_read_part_b();

  server = new AsyncWebServer(80);
  webserver_art();

  Serial.println();
  Serial.print(wifi.esp_name);
  Serial.println(" wird gestartet");
  Serial.println();

  wlan_config();

  config.seriel = true;

  if (config.seriel == true)
  {
    digitalWrite(output_comport_activ, LOW);
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  }
  if (mqtt.aktiv)
  {
    mqtt_config();
  }
  if (bluetooth.aktiv)
  {
    bluetooth_config();
  }
  bme_config();
}

void loop()
{
  serial_status();

  // Rücksetzen Alarmtimer
  if (millis() >= timer_alarm && timer_alarm != 0) timer_alarm = 0;
  // 5 Sekunden intervall
  if (millis() >= timer_time)
  {
    timer_time = millis() + 5000;
    timer_funktion();
  }
  // MQTT Funktion
  if (mqtt.aktiv) client.loop();
  // Seriele Funktionen
  serial_read();
  if (seri_status > 0) serial_send("");
  // bluetooth !
  if (bluetooth.konfiguriert &&
      bluetooth.timer < millis())
  {
    bluetooth.timer = millis() + 5000;
    bluetooth_scan();
  }
}
