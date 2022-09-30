#pragma once

struct WIFI
{
    String esp_name;
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

void wlan_connect();
void scan_wifi_ssid();
void wlan_config();

IPAddress ipwandeln(String temp);

String web_server_wifi(const String &var);
void web_server_wifi_get(String name, String msg);

void load_conf_wifi(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_wifi(StaticJsonDocument<1024> doc);