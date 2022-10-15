#include "SysHeaders.h"

BLUETOOTH bluetooth;
BLEScan *pBLEScan;

void bluetooth_config()
{
    bluetooth.konfiguriert = true;
    for (int i = 0; i < bluetooth.mac_size; i++)
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
        Serial.print(bluetooth.mac_adress[i]);
        Serial.print("   |   ");
        Serial.println(bluetooth.mac_name[i]);
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
        for (i = 0; i < bluetooth.mac_size; i++)
        {
            String temp_adress = bluetooth.mac_adress[i];
            String temp_name = bluetooth.mac_name[i];
            if ( gefunden == temp_adress)
            {
                bluetooth.empfang[i] = empfang;
                mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + extension + ext_bluetooth + temp_name, String(empfang));
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
        for (i = 0; i < bluetooth.mac_size; i++)
        {
            String temp_name = bluetooth.mac_name[i];
            if (bluetooth.empfang[i] == 0)
            {
                mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + extension + ext_bluetooth + temp_name, String(0));
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
    //Serial.println("Bluetooth WEB");
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

void load_conf_bluetooth(StaticJsonDocument<1024> doc)
{
    Serial.println("... Bluetooth- Variablen ...");
    
    bluetooth.aktiv = doc["bluetooth"] | false;
    bluetooth.memory_file = doc["bluetooth"] | "589ec621333b(G-Tag David) 589ec61a30ca(G-Tag Janin) 589ec61a30dd(G-Tag Florentine) 589ec6204371(G-Tag Hundeleine) 589ec62043c6(G-Tag Cedrik)";
    bluetooth_mac_diagnose(bluetooth.memory_file);
}

StaticJsonDocument<1024> safe_conf_bluetooth(StaticJsonDocument<1024> doc)
{
    Serial.println("... Bluetooth- Variablen ...");
    
    doc["bluetooth"] = bluetooth.aktiv;

    return doc;
}

void bluetooth_mac_diagnose(String msg)
{
    String temp_name = "";
    char temp_char;

    bluetooth.temp_mac_a = 0;
    bluetooth.temp_mac_b = 0;
    bluetooth.temp_mac_klein = "";
    bluetooth.temp_mac_gross = ""; 
    bluetooth.diagnose_mac = true;  
    bluetooth.memory_file = ""; 
    bluetooth.mac_size = 0;

    for (int i = 0; msg[i] != 0; i++)
    {
        if (msg[i] == 40) // = (
        {
            bluetooth.diagnose_mac = false;
            bluetooth.memory_file += msg[i];
        }
        if (bluetooth.diagnose_mac == true)
        {
            // ZAHLEN
            if (msg[i] > 47 && msg[i] < 58)
            {
                if (bluetooth.temp_mac_a == 2 && bluetooth.temp_mac_b == 5)
                {
                    bluetooth_mac();
                }
                bluetooth.temp_mac_klein += msg[i];
                bluetooth.temp_mac_gross += msg[i];
                bluetooth.memory_file += msg[i];
                bluetooth.temp_mac_a ++;    
            }
            // BUCHSTABEN KLEIN
            if (msg[i] > 96 && msg[i] < 123)
            {
                if (bluetooth.temp_mac_a == 2 && bluetooth.temp_mac_b == 5)
                {
                    bluetooth_mac();
                }
                temp_char = msg[i] - 32;
                bluetooth.temp_mac_klein += msg[i];
                bluetooth.temp_mac_gross += temp_char;
                bluetooth.memory_file += msg[i];
                bluetooth.temp_mac_a ++;    
            }
            // BUCHSTABEN GROß
            if (msg[i] > 64 && msg[i] < 91)
            {
                if (bluetooth.temp_mac_a == 2 && bluetooth.temp_mac_b == 5)
                {
                    bluetooth_mac();
                }
                temp_char = msg[i] + 32;
                bluetooth.temp_mac_klein += temp_char;
                bluetooth.temp_mac_gross += msg[i];
                bluetooth.memory_file += temp_char;
                bluetooth.temp_mac_a ++;    
            }
            // -> : <- WERDEN EINGEFÜGT
            if (bluetooth.temp_mac_a == 2 && bluetooth.temp_mac_b != 5)
            {
                bluetooth.temp_mac_gross += ":";
                bluetooth.temp_mac_klein += ":";
                bluetooth.temp_mac_a = 0;
                bluetooth.temp_mac_b ++;
            }
        }
        else
        {
            if (msg[i] == 41)
            {
                bluetooth.memory_file += ")";
                bluetooth.temp_mac_gross = temp_name + " (" + bluetooth.temp_mac_gross + ")";
                temp_name = "";
                bluetooth_mac();
            }
            else
            {
                if (msg[i] != 40)
                {
                    temp_name += msg[i];
                }
                bluetooth.memory_file += msg[i];
            }
        }
    }
    if (bluetooth.temp_mac_b == 5)
    {
        bluetooth_mac();
    }

}

void bluetooth_mac()
{
    for (int j = 0; bluetooth.temp_mac_gross[j] != 0 ; j++)
    {
        bluetooth.mac_name[bluetooth.mac_size][j] = bluetooth.temp_mac_gross[j];
    }
    for (int k = 0; bluetooth.temp_mac_klein[k] != 0 ; k++)
    {
        bluetooth.mac_adress[bluetooth.mac_size][k] = bluetooth.temp_mac_klein[k];
    }
    bluetooth.memory_file += " ";
    bluetooth.diagnose_mac = true;
    bluetooth.temp_mac_a = 0;
    bluetooth.temp_mac_b = 0;
    bluetooth.temp_mac_klein = "";
    bluetooth.temp_mac_gross = "";
    bluetooth.mac_size ++;
}
