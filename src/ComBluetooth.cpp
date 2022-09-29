#include "SysHeaders.h"

BLUETOOTH bluetooth;
BLEScan *pBLEScan;

void bluetooth_config()
{
    bluetooth.konfiguriert = true;
    for (int i = 0; i < bluetooth.anzahl; i++)
    {
        bluetooth.empfang[i] = 0;
    }
    //strcpy(bluetooth.arr[3],"Test");
    
    Serial.println("Starte BLE Scanner");
    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan();
    pBLEScan->setActiveScan(true); // Aktives Bluetooth Scannen
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99);

    for (int i = 0; i < 5; i++)
    {
        Serial.print("Adresse ");
        Serial.print(i);
        Serial.print(" : ");
        Serial.println(bluetooth.adressen[i]);
    }
    
    //Serial.println(bluetooth.adressen[0]);
}

void bluetooth_scan()
{
    int static number_circle = 0;
    number_circle ++;
    BLEScanResults foundDevices = pBLEScan->start(1); // BLE Scanen

    int j;
    int i;
    for (j = 0; j < foundDevices.getCount(); j++) // Alle Gefundene Bluetooth Device durchgehen
    {
        String gefunden = foundDevices.getDevice(j).getAddress().toString().c_str();
        int empfang = foundDevices.getDevice(j).getRSSI() + 100;
        for (i = 0; i < bluetooth.anzahl; i++)
        {
            String temp = bluetooth.adressen[i];
            if ( gefunden == temp)
            {
                bluetooth.empfang[i] = empfang;
                mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + extension + ext_bluetooth + temp, String(empfang));
/*
                String topic = 
                mqtt_publish()
                Serial.print("ES HAT GEKLAPPT : ");
                Serial.print(temp);
                Serial.print(" | ");
                Serial.println(empfang);*/
            }
        }
        
        
        Serial.print(foundDevices.getDevice(j).getAddress().toString().c_str());
        Serial.print(" | ");
        Serial.print(foundDevices.getDevice(j).getName().c_str());
        Serial.print(" | ");
        Serial.println(foundDevices.getDevice(j).getRSSI());
    
    }
    pBLEScan->stop();
    pBLEScan->clearResults();

    if (number_circle == 10)
    {
        for (i = 0; i < bluetooth.anzahl; i++)
        {
            String temp = bluetooth.adressen[i];
            if (bluetooth.empfang[i] == 0)
            {
                mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + extension + ext_bluetooth + temp, String(0));
            }
            else
            {
                bluetooth.empfang[i] = 0;
            }
        }
    number_circle = 0;
    }
}


String web_server_bluetooth(const String &var)
{
    String temp = "";
    Serial.println("Bluetooth WEB");
    if (var == "nav-sen-bt")
    {
        if (!bluetooth.aktiv)
        {
            return "<br/><div class='bt' style=' display : none;'>";
        }
        else
        {
            return "<div class='bt'>";
        }
    }
    if (var == "place_sensor_bt")
    {
        if (bluetooth.aktiv)
        {
            temp = "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert";
        }
        else
        {
            temp = "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
        }
        return temp;
    }

    return String();
}

void web_server_bluetooth_get(String name, String msg)
{
    if (name == "bluetooth")
    {
        if (msg == "aktiviert")
        {
            bluetooth.aktiv = true;
        }
        else
        {
            bluetooth.aktiv = false;
        }
    }
}