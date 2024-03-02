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

    if ( !SPIFFS.exists("/config.json") ) {
      webserver.notbetrieb = true;
      webserver_setup();
    } else {
      webserver.notbetrieb = check_files();
      webserver_setup();
    }

    version_check();
    time_setup();

    if ( mqtt.aktiv )     mqtt_setup(); 
    if ( sensors.bme )    bme_setup();
  }

  serial_setup();

  // Setzen Sie das Passwort für das OTA-Update
  //ArduinoOTA.setPassword("Rauchmelder");
  // Setzen Sie den Port für das OTA-Update
  if (system_funktion.ota) {
    const char* otaPassword = system_funktion.ota_pw.c_str();
    ArduinoOTA.setPassword(otaPassword);
    ArduinoOTA.begin();
  }
  

  log_write("Startsequenz vollständig");
}

void loop()
{
  sensor_data();
  if ( system_funktion.ota )                ArduinoOTA.handle();
  if ( !WiFi.isConnected() && !AP_Mode )    wlan_connect(); 
  if ( mqtt.configured )                    mqtt_reconnect() ;
  if ( millis() > wifi.ntp_timer )          time_sync();
  if ( comserial.aktiv )                    serial_receive();
  if ( comserial.aktiv )                    serial_transceive_diagnose();

}
