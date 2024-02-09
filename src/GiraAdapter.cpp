#include "SysHeaders.h"

AsyncWebServer *server;
WiFiClient espClient;
PubSubClient client(espClient);
HTTPClient http;

void setup()
{
  #ifdef DEBUG_SERIAL_OUTPUT  
    Serial.begin(115200);
    Serial.println("Serial, Debug gestartet ...");
  #endif

  pinMode(output_led_esp, OUTPUT);
  pinMode(output_led_detector, OUTPUT);
  pinMode(output_comport_activ, OUTPUT);

  pinMode(input_comport_activ, INPUT);
  pinMode(input_webportal, INPUT);
  pinMode(input_reset, INPUT);            // DIP 2 !

  led_flash_timer(5000, 0, 1);

  spiffs_starten();
  spiffs_scan();
  spiffs_config_read();

  if (digitalRead(input_reset) == 0)
  {
    spiffs_format();
  }
  
  #ifdef DEBUG_SERIAL_OUTPUT
    Serial.println();
    Serial.print(wifi.esp_name);
    Serial.println(" wird gestartet");
    Serial.println();
  #endif

  //SPIFFS.remove("/config.html");

  webserver_art();
  wlan_config();
  version_check();

  if ( mqtt.aktiv )     { mqtt_config(); }


  //comserial.aktiv = true;

  /*
  if (comserial.aktiv == true)
  {
    digitalWrite(output_comport_activ, LOW);
    Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  }
  if ( mqtt.aktiv ) { mqtt_config(); }

  bme_config();

  */
  if ( webserver.notbetrieb && WiFi.isConnected() == true )
  {
    #ifdef DEBUG_SERIAL_OUTPUT
      Serial.print("Wifi ist vorhanden, nötige Daten werden gedownloadet");
    #endif
    file_download("/config.html");
    file_download("/config.css");
    delay(1000);
    ESP.restart();
  }
}

void loop()
{
  if ( !WiFi.isConnected() && !AP_Mode )                              wlan_connect(); 
  if ( WiFi.isConnected() && mqtt.aktiv && !client.connected() )      mqtt_connect(); 
  if ( client.connected() && mqtt.aktiv )                             client.loop();
  
  /*
  serial_status();
  if ( client.connected() && detectordiag.timer <= millis() && comserial.gestartet == true ) detector_serial_timer(); 
  if ( client.connected() && mqtt.timer <= millis() ) mqtt_esp_status();
  if ( system_funktion.timer <= millis() ) system_timer();

  // Rücksetzen Alarmtimer
  if (millis() >= detector.timer && detector.timer != 0) detector.timer = 0;
  
  // MQTT Funktion
  if (mqtt.aktiv) client.loop();
  // Seriele Funktionen
  serial_read();
  if (comserial.com_status > 0) serial_send("", comserial.com_status);
  // 30 Sekunden intervall für Erweiterungen
  if (millis() >= sensor.timer)
  {
    sensor.timer = millis() + 30000;
    bme_refresh();
  }
  */






}
