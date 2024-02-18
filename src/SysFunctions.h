#pragma once

struct SYSTEM_FUNKTION
{
    unsigned long timer = 0;
    String version_old = "beta-20240212";
    String github_path = "https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/Firmwareupdate";
    String version_new ;
    String fw_art ;
    boolean new_version ;
};
extern SYSTEM_FUNKTION system_funktion;
extern HTTPClient http;

#define DEBUG_SERIAL_START
#define DEBUG_SERIAL_OUTPUT // Aktivieren für Serial- Ausgabe
//#define DEBUG_SERIAL_SENSOR
#define DEBUG_SERIAL_SPIFFS
//#define DEBUG_SERIAL_MQTT
#define DEBUG_SERIAL_WIFI
#define DEBUG_SERIAL_WEBSERVER
//#define DEBUG_SERIAL_DETECTOR

//      Inputs
#define RXD2 17
#define TXD2 16
#define input_light 36
#define input_ubext 39
#define input_comport_activ 15
#define input_webportal 13
#define input_reset 25

#define output_led_esp 19
#define output_led_detector 27
#define output_comport_activ 23


void system_timer();
void version_check();
const String& firmware_path();

void firmwareupdate_http();
void file_download(String filePath);
void folder_download(const String& folderPath);
void led_flash_timer(int timer_on,int timer_off, int number);

String web_request_sys(const String &var);
void web_response_sys(String name, String msg);


void load_conf_sys(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_sys(StaticJsonDocument<1024> doc);
