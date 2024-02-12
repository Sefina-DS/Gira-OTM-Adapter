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
    
};
extern WIFI wifi;
extern String hostname;

extern boolean AP_Mode;

void wlan_connect();
void scan_wifi_ssid();
void wlan_config();

IPAddress ipwandeln(String temp);

String getSelectedOption(const String &option);
String web_request_wifi(const String &var);
void webserver_triger_wifi(String name, String msg);

void load_conf_wifi(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_wifi(StaticJsonDocument<1024> doc);