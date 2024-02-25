#pragma once

struct WIFI
{
    String esp_name;
    String esp_id;
    String ssid;
    String pw;
    boolean dhcp;
    String ip;
    String gw;
    String subnet;
    String dns;
    String ntp_date;
    const char* ntpServer = "time.google.com";
    unsigned long ntp_timer = 10000;
};
extern WIFI wifi;
extern String hostname;

extern boolean AP_Mode;
extern WiFiUDP ntpUDP;
extern NTPClient* timeClient;

void wlan_connect();
String scan_wifi_ssid();
void wlan_config();

IPAddress ipwandeln(String temp);

String web_request_wifi(const String &var);
void web_response_wifi(String name, String msg);

void load_conf_wifi(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_wifi(StaticJsonDocument<1024> doc);

void time_setup();
void time_sync();
