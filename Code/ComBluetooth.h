#pragma once

struct BLUETOOTH
{
    boolean aktiv;
    boolean konfiguriert = false;
    unsigned long timer = 0;
    int anzahl = 5;
    int empfang[20];
    char adressen[20][20] =
    {  
        "58:9e:c6:21:33:3b",
        "58:9e:c6:1a:30:ca",
        "58:9e:c6:1a:30:dd",
        "58:9e:c6:20:43:71",
        "58:9e:c6:20:43:c6"
    };

    
};
extern BLUETOOTH bluetooth;

void bluetooth_config();
void bluetooth_scan();

String web_server_bluetooth(const String &var);
void web_server_bluetooth_get(String name, String msg);