#include "SysHeaders.h"

AsyncWebServer *server;
WiFiClient espClient;
PubSubClient client(espClient);
HTTPClient http;

static unsigned long ntp_timer = 10000;
static long log_counter = 0;

void setup()
{
  #ifdef DEBUG_SERIAL_START  
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
  
  if (digitalRead(input_reset) == 0) {
    spiffs_format();
  } else {
    spiffs_scan();
    spiffs_config_read();
    spiffs_config_load();

    wlan_config();

    //SPIFFS.remove("/config.html");
    //SPIFFS.remove("/config.css");
    if (SPIFFS.exists("/config.html")) webserver.notbetrieb = false;
    webserver_setup();


    //webserver_art();

    version_check();

    void time_setup();

    if ( mqtt.aktiv )     mqtt_setup(); 
    if ( sensors.bme )    bme_setup();
  }

  serial_setup();

  if ( webserver.notbetrieb && WiFi.isConnected() == true ) {
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
  
  if ( !WiFi.isConnected() && !AP_Mode )    wlan_connect(); 
  if ( mqtt.configured )                    mqtt_reconnect() ;
  sensor_data();
  if ( millis() > wifi.ntp_timer )          time_sync();
  if ( comserial.aktiv ) serial_receive();
  if ( comserial.aktiv ) serial_transceive_diagnose();

  #ifdef DEBUG_SERIAL_WIFI
      if ( millis() >= ntp_timer ) {
        ntp_timer = millis() + 10000;
        Serial.println(wifi.ntp_date + " " + timeClient->getFormattedTime() + " // " + timeClient->getDay());
        log_counter++;
        log_write("Logcounter = " + log_counter);
      }
  #endif

  
  
  /*
  
  if ( WiFi.isConnected() && mqtt.aktiv && !client.connected() )      mqtt_connect(); 
  if ( client.connected() && mqtt.aktiv )                             client.loop();
  
  
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
