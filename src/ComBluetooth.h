#pragma once

struct BLUETOOTH
{
    boolean aktiv;
    boolean konfiguriert = false;
    unsigned long timer = 0;
    int empfang[20];
    char mac_adress[20][20] = { "", };
    char mac_name[20][60] = { "", };
    int mac_size;
    String temp_mac_klein;
    String temp_mac_gross;
    int temp_mac_a;
    int temp_mac_b;
    String memory_file;
    boolean diagnose_mac;
};
extern BLUETOOTH bluetooth;

void bluetooth_config();
void bluetooth_scan();

String web_server_bluetooth(const String &var);
void web_server_bluetooth_get(String name, String msg);

void load_conf_bluetooth(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_bluetooth(StaticJsonDocument<1024> doc);

void bluetooth_mac_diagnose(String msg);
void bluetooth_mac();