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

    for (int i = 0; i < bluetooth.mac_size; i++)
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
                Serial.print("Nummer ");
                Serial.print(i);
                Serial.print(" : ");
                Serial.print(bluetooth.mac_adress[i]);
                Serial.print(" | ");
                Serial.println(temp_name);
                Serial.print("Empfang : ");
                Serial.println(String(empfang));
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


String webserver_call_bluetooth(const String &var)
{
    String temp = "";
    if (var == "web_sensor_bluetooth")
    {
        temp += F("<div class='sensor'><br/><div class='box'>");
        temp += F("<h3>Bluetooth Einstellungen</h3>");
        temp += F("<form action='/get'>");
        temp += F("<table>");
        // MQTT Aktiv/Deaktiv
        temp += F("<tr><td>Bluetooth :</td>");
        temp += F("<td><select name='bluetooth'><option value='");
        if (bluetooth.aktiv)
        {
            temp += F("aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert");
        }
        else
        {
            temp += F("deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert");
        }
        temp += F("</select>");
        temp += F("</td></tr>");
        temp += F("</table>");
        // Verdeckte Einstellungen (Bluetooth Aktiv)
        temp += F("<table ");
        if (!bluetooth.aktiv)
        {
            temp += F("style='display: none'>");
        }
        else
        {
            temp += F(">");
        }
      // MAC Adressen
      temp += F("<tr><td>MAC Adressen :</td></tr>");
      temp += F("<tr><td><textarea rows='20' cols='40' name='bluetooth_mac' placeholder='");
      temp +=     bluetooth.memory_file;
      temp += F("'></textarea></td></tr>");
      
    temp += F("</table><br/>");
    temp += F("<input type='submit' value='Submit' />");
    temp += F("</form></div></div>");
    return temp;
    }
    
    return String();
    
}

void webserver_triger_bluetooth(String name, String msg)
{
    if (name == "bluetooth" && msg == "aktiviert") bluetooth.aktiv = true;
    if (name == "bluetooth" && msg == "deaktiviert") bluetooth.aktiv = false;
    if (name == "bluetooth_mac" && msg != "") bluetooth_mac_diagnose(msg);
}

void load_conf_bluetooth(StaticJsonDocument<1024> doc)
{
    Serial.println("... Bluetooth- Variablen ...");
    
    bluetooth.aktiv = doc["bluetooth"] | false;
    bluetooth.memory_file = doc["bluetooth_mac"] | "";
    bluetooth_mac_diagnose(bluetooth.memory_file);
}

StaticJsonDocument<1024> safe_conf_bluetooth(StaticJsonDocument<1024> doc)
{
    Serial.println("... Bluetooth- Variablen ...");
    
    doc["bluetooth"] = bluetooth.aktiv;
    doc["bluetooth_mac"] = bluetooth.memory_file;

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
                bluetooth.temp_mac_gross = temp_name + "_%_"+ bluetooth.temp_mac_gross;
                temp_name = "";
                bluetooth_mac();
            }
            else
            {
                if (msg[i] == 32) 
                {
                    temp_name += "_";
                    bluetooth.memory_file += "_";
                }
                else if (msg[i] != 40) 
                {
                    temp_name += msg[i];
                    bluetooth.memory_file += msg[i];
                }
            }
        }
    }
    if (bluetooth.temp_mac_b == 5)
    {
        bluetooth_mac();
    }
    Serial.print("Bluetooth Safefile : ");
    Serial.println(bluetooth.memory_file);
    Serial.println("Bluetooth MAC detailiert :");
    for (int i = 0; i < bluetooth.mac_size; i++)
    {
        Serial.print("Adresse ");
        Serial.print(i);
        Serial.print(" : ");
        Serial.print(bluetooth.mac_adress[i]);
        Serial.print("   |   ");
        Serial.println(bluetooth.mac_name[i]);
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
