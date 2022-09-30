#include "SysHeaders.h"

WIFI wifi;
boolean AP_Mode = false;

void wlan_connect()
{
    WiFi.setHostname(wifi.esp_name.c_str());
    WiFi.begin(wifi.ssid.c_str(), wifi.pw.c_str());

    Serial.print("Wifi - Verbinden : ");

    for (int i = 0; i < 20; i++)
    {
        if (WiFi.isConnected())
        {
            Serial.println(" / erfolgreich ");
            Serial.println("die Mac ist : " + WiFi.macAddress());
            Serial.print("die IP Adresse ist : ");
            Serial.println(WiFi.localIP());
            Serial.println();
            server->begin();
            delay(2000);
            return;
        }
        else
        {
            Serial.print(".");
            led_flash_timer(500, 500, 1);
        }
    }
    Serial.println(" / fehlgeschlagen ");
}

void wlan_config()
{
    byte available_networks = WiFi.scanNetworks();
    int wifi_zahl = 0;
    for (int network = 0; network < available_networks; network++)
    {
        if (WiFi.SSID(network) == wifi.ssid)
        {
            wifi_zahl++;
        }
    }
    if (wifi_zahl > 0)
    {
        if (wifi.dhcp == false)
        {
            IPAddress temp_ip = ipwandeln(wifi.ip);
            IPAddress temp_gw = ipwandeln(wifi.gw);
            IPAddress temp_subnet = ipwandeln(wifi.subnet);
            IPAddress temp_dns = ipwandeln(wifi.dns);
            
            Serial.println("Es wird mit Statischer IP verbunden");
            WiFi.mode(WIFI_STA);
            WiFi.config(temp_ip, temp_gw, temp_subnet, temp_dns);
            delay(1000);
            wlan_connect();
        }
        else
        {
            // dynamisch oder kein Typ gewählt
            Serial.println("Es wird mit DHCP verbunden");
            WiFi.mode(WIFI_STA);
            WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
            delay(1000);
            wlan_connect();
        }
    }
    else
    {
        AP_Mode = true;

        Serial.println("Acces- Modus wird gestartet");
        IPAddress IP = IPAddress(10, 50, 0, 1);
        IPAddress gateway = IPAddress(10, 50, 0, 1);
        IPAddress NMask = IPAddress(255, 255, 255, 0);

        WiFi.mode(WIFI_AP);
        WiFi.softAP(wifi.esp_name.c_str());
        delay(1000);
        WiFi.softAPConfig(IP, gateway, NMask);
        delay(1000);
        IPAddress myIP = WiFi.softAPIP();
        Serial.println();
        Serial.print("AP IP address: ");
        Serial.println(myIP);
        Serial.println("mit der SSID : " + wifi.esp_name);
        Serial.println();

        server->begin();
    }
}

IPAddress ipwandeln(String temp)
{
    const char *apipch;
    IPAddress temp_ip;
    apipch = temp.c_str();
    temp_ip.fromString(apipch);
    return temp_ip;
}

void scan_wifi_ssid()
{
    webserver.wifi_ssid = "";
    int n = WiFi.scanNetworks();
    if (n > 0)
    {
        for (int i = 0; i < n; ++i)
        {
            if (WiFi.SSID(i) != "")
            {
                webserver.wifi_ssid += F("<option value='");
                webserver.wifi_ssid += WiFi.SSID(i);
                webserver.wifi_ssid += F("'>");
                webserver.wifi_ssid += WiFi.SSID(i);
                webserver.wifi_ssid += F(" | ");
                webserver.wifi_ssid += WiFi.RSSI(i);
                webserver.wifi_ssid += F("</option>");
            }
        }
    }
}

String web_server_wifi(const String &var)
{
    String temp = "";
    if (var == "nav-net-dhcp")
    {
        if (wifi.dhcp)
        {
            return "<br/><div class='dhcp' style=' display : none;'>";
        }
        else
        {
            return "<div class='dhcp'>";
        }
    }
    if (var == "place_esp_name")
    {
        temp = "placeholder = '" + wifi.esp_name + "'";
        return temp;
    }
    if (var == "place_wifi_ssid")
    {
        temp = F("<select name='wifi_ssid'>");
        if (wifi.ssid != "")
        {
            temp += F("<option value='");
            temp += wifi.ssid;
            temp += F("' selected>");
            temp += wifi.ssid;
            temp += F("</option>");
        }
        else
        {
            temp += F("<option selected>keins ausgewählt</option>");
        }
        temp += webserver.wifi_ssid;
        temp += F("</select>");
        return temp;
    }
    if (var == "place_wifi_pw")
    {
        if (wifi.pw != "")
        {
            temp = "placeholder = '---FFF---FFF---'";
        }
        else
        {
            temp = "placeholder = 'Bitte eintragen !'";
        }

        return temp;
    }
    if (var == "place_wifi_typ")
    {
        if (wifi.dhcp)
        {
            temp = "<option value='dynamisch' selected>dynamisch</option><option value='statisch'</option>statisch";
        }
        else
        {
            temp = "<option value='statisch' selected>statisch</option><option value='dynamisch'</option>dynamisch";
        }

        return temp;
    }
    if (var == "place_wifi_ip")
    {
        temp = "placeholder = '" + wifi.ip + "'";
        return temp;
    }
    if (var == "place_wifi_subnet")
    {
        temp = "placeholder = '" + wifi.subnet + "'";
        return temp;
    }
    if (var == "place_wifi_gw")
    {
        temp = "placeholder = '" + wifi.gw + "'";
        return temp;
    }
    if (var == "place_wifi_dns")
    {
        temp = "placeholder = '" + wifi.dns + "'";
        return temp;
    }

    return String();
}

void web_server_wifi_get(String name, String msg)
{
    if (name == "esp_name")
    {
        if (msg != "")
        {
            wifi.esp_name = msg;
        }
    }
    if (name == "wifi_ssid")
    {
        if (msg != "")
        {
            wifi.ssid = msg;
        }
    }
    if (name == "wifi_pw")
    {
        if (msg != "")
        {
            wifi.pw = msg;
        }
    }
    if (name == "wifi_dhcp")
    {
        if (msg == "dynamisch")
        {
            wifi.dhcp = true;
        }
        else
        {
            wifi.dhcp = false;
        }
    }
    if (name == "wifi_ip")
    {
        if (msg != "")
        {
            wifi.ip = msg;
        }
    }
    if (name == "wifi_gw")
    {
        if (msg != "")
        {
            wifi.gw = msg;
        }
    }
    if (name == "wifi_subnet")
    {
        if (msg != "")
        {
            wifi.subnet = msg;
        }
    }
    if (name == "wifi_dns")
    {
        if (msg != "")
        {
            wifi.dns = msg;
        }
    }
}

void load_conf_wifi(StaticJsonDocument<1024> doc)
{
    Serial.println("... WiFi- Variablen ...");
    
    wifi.esp_name = doc["esp_name"] | "";
    wifi.ssid = doc["wifi_ssid"] | "-";
    wifi.pw = doc["wifi_pw"] | "";
    wifi.dhcp = doc["wifi_dhcp"] | true;
    wifi.ip = doc["wifi_ip"] | "0.0.0.0";
    wifi.gw = doc["wifi_gw"] | "0.0.0.0";
    wifi.subnet = doc["wifi_subnet"] | "0.0.0.0";
    wifi.dns = doc["wifi_dns"] | "0.0.0.0";

    if (wifi.esp_name == "")
    {
        uint32_t chipId = 0;
        for (int i = 0; i < 17; i = i + 8)
        {
            chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
        }
        wifi.esp_name = chipId;
        wifi.esp_name = "ESP-" + wifi.esp_name;
        
    }
}

StaticJsonDocument<1024> safe_conf_wifi(StaticJsonDocument<1024> doc)
{
    Serial.println("... WiFi- Variablen ...");
    
    doc["esp_name"] = wifi.esp_name;
    doc["wifi_ssid"] = wifi.ssid;
    doc["wifi_pw"] = wifi.pw;
    doc["wifi_dhcp"] = wifi.dhcp;
    doc["wifi_ip"] = wifi.ip;
    doc["wifi_gw"] = wifi.gw;
    doc["wifi_subnet"] = wifi.subnet;
    doc["wifi_dns"] = wifi.dns;

    return doc;
}