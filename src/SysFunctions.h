#pragma once

struct SYSTEM_FUNKTION
{
    unsigned long timer = 0;
    String version_old = "beta-20240225";
    String github_path = "https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/Firmwareupdate";
    String version_new ;
    String fw_art ;
    boolean new_version ;
    bool ota = false ;
    String ota_pw ;
    bool config = false;
};
extern SYSTEM_FUNKTION system_funktion;
extern HTTPClient http;

#define DEBUG_SERIAL

#ifdef DEBUG_SERIAL
    #define DEBUG_SERIAL_START
    #define DEBUG_SERIAL_OUTPUT // Aktivieren für Serial- Ausgabe
    #define DEBUG_SERIAL_SENSOR
    #define DEBUG_SERIAL_SPIFFS
    //#define DEBUG_SERIAL_MQTT
    #define DEBUG_SERIAL_WIFI
    #define DEBUG_SERIAL_WEBSERVER
    #define DEBUG_SERIAL_DETECTOR
#endif

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
bool file_download(String download_file_path, String spiff_file_path);
//void downloadFile(const String& url, const String& filePath);
//void test();
//void processJsonArray(const String& jsonContent, const String& spiffs_folder_path);
//void processJsonStream(WiFiClient& client, const String& spiffs_folder_path);
//void folder_download(const String& download_folder_path, const String& spiffs_folder_path);
void led_flash_timer(int timer_on,int timer_off, int number);

String web_request_sys(const String &var);
void web_response_sys(String name, String msg);


void load_conf_sys(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_sys(StaticJsonDocument<1024> doc);
