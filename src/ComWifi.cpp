#include "SysHeaders.h"

WIFI wifi;
boolean AP_Mode = false;

void wlan_connect()
{
    static int fail_wifi = 0;
    WiFi.setHostname(wifi.esp_name.c_str());
    WiFi.begin(wifi.ssid.c_str(), wifi.pw.c_str());

    #ifdef DEBUG_SERIAL_WIFI
        Serial.print("Wifi - Verbinden - start : ");
    #endif

    for (int i = 0; i < 20; i++)
    {
        if (WiFi.isConnected())
        {
            #ifdef DEBUG_SERIAL_WIFI
                Serial.println(" / erfolgreich ");
                Serial.println("die Mac ist : " + WiFi.macAddress());
                Serial.print("die IP Adresse ist : ");
                Serial.println(WiFi.localIP());
                Serial.println();
            #endif
            fail_wifi = 0;
            server->begin();
            delay(2000);
            return;
        }
        else
        {
            #ifdef DEBUG_SERIAL_WIFI
                Serial.print(".");
            #endif
            fail_wifi ++;
            led_flash_timer(500, 500, 1);
            if ( fail_wifi == 15 ) ESP.restart();
        }
    }
    #ifdef DEBUG_SERIAL_WIFI
        Serial.println(" / fehlgeschlagen ");
    #endif
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
            
            #ifdef DEBUG_SERIAL_WIFI
                Serial.println("Es wird mit Statischer IP verbunden");
            #endif
            WiFi.mode(WIFI_STA);
            WiFi.config(temp_ip, temp_gw, temp_subnet, temp_dns);
            delay(1000);
            wlan_connect();
        }
        else
        {
            // dynamisch oder kein Typ gewählt
            #ifdef DEBUG_SERIAL_WIFI
                Serial.println("Es wird mit DHCP verbunden");
            #endif
            WiFi.mode(WIFI_STA);
            WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
            delay(1000);
            wlan_connect();
        }
    }
    else
    {
        AP_Mode = true;
        #ifdef DEBUG_SERIAL_WIFI
            Serial.println("Acces- Modus wird gestartet");
        #endif
        IPAddress IP = IPAddress(10, 50, 0, 1);
        IPAddress gateway = IPAddress(10, 50, 0, 1);
        IPAddress NMask = IPAddress(255, 255, 255, 0);

        WiFi.mode(WIFI_AP);
        WiFi.softAP(wifi.esp_name.c_str());
        delay(1000);
        WiFi.softAPConfig(IP, gateway, NMask);
        delay(1000);
        IPAddress myIP = WiFi.softAPIP();
        #ifdef DEBUG_SERIAL_WIFI
            Serial.println();
            Serial.print("AP IP address: ");
            Serial.println(myIP);
            Serial.println("mit der SSID : " + wifi.esp_name);
            Serial.println();
        #endif
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

String getSelectedOption(const String &option) {
    return "<option value='" + option + "' selected>" + option + "</option>";
}

String web_request_wifi(const String &var) {
    if          (var == "ph_wifi_esp")          { return wifi.esp_name;
    } else if   (var == "ph_wifi_ssid")         { return (wifi.ssid != "")      ? getSelectedOption(wifi.ssid)
                                                                                : "<option selected>keins ausgewählt</option>";
    } else if   (var == "ph_wifi_ssiddisplay")  { scan_wifi_ssid();
                                                  return webserver.wifi_ssid;
    } else if   (var == "ph_wifi_pw")           { return (wifi.pw != "")        ? "---FFF---FFF---" 
                                                                                : "Bitte eintragen !";
    } else if   (var == "ph_wifi_dhcp")         { return (wifi.dhcp)            ? "'dynamisch' selected>dynamisch</option><option value='statisch'</option>statisch" 
                                                                                : "'statisch' selected>statisch</option><option value='dynamisch'</option>dynamisch";
    } else if   (var == "ph_wifi_dhcpdisplay")  { return (wifi.dhcp)            ? "display: none; " 
                                                                                : "";
    } else if   (var == "ph_wifi_ip")           { return wifi.ip;
    } else if   (var == "ph_wifi_subnet")       { return wifi.subnet;
    } else if   (var == "ph_wifi_gw")           { return wifi.gw;
    } else if   (var == "ph_wifi_dns")          { return wifi.dns;
    }

    return String();
}

void webserver_triger_wifi(String name, String msg)
{
    if (msg != "")
    {
        if (name == "esp_name")     { wifi.esp_name = msg; }
        if (name == "wifi_ssid")    { wifi.ssid = msg; }
        if (name == "wifi_pw")      { wifi.pw = msg; }
        if (name == "wifi_ip")      { wifi.ip = msg; }
        if (name == "wifi_gw")      { wifi.gw = msg; }
        if (name == "wifi_subnet")  { wifi.subnet = msg; }
        if (name == "wifi_dns")     { wifi.dns = msg; }
    }
    
    if (name == "wifi_dhcp")
    {
        if (msg == "dynamisch") { wifi.dhcp = true; } else { wifi.dhcp = false; }
    }
    
    if (    webserver.notbetrieb &&
            wifi.ssid != "" &&
            wifi.pw != "" )
    {
        spiffs_config_save();
        delay(1000);
        ESP.restart();
    }

}

void load_conf_wifi(StaticJsonDocument<1024> doc)
{
    #ifdef DEBUG_SERIAL_WIFI
        Serial.println("... WiFi- Variablen ...");
    #endif
    wifi.esp_name = doc["esp_name"] | "";
    wifi.ssid = doc["wifi_ssid"] | "-";
    wifi.pw = doc["wifi_pw"] | "";
    wifi.dhcp = doc["wifi_dhcp"] | true;
    wifi.ip = doc["wifi_ip"] | "0.0.0.0";
    wifi.gw = doc["wifi_gw"] | "0.0.0.0";
    wifi.subnet = doc["wifi_subnet"] | "0.0.0.0";
    wifi.dns = doc["wifi_dns"] | "0.0.0.0";

    uint32_t chipId = 0;
        for (int i = 0; i < 17; i = i + 8)
        {
            chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
        }
    String temp = String(chipId);
    
    if (wifi.esp_name == "") wifi.esp_name = "ESP-" + temp;
    wifi.esp_id = temp;
}

StaticJsonDocument<1024> safe_conf_wifi(StaticJsonDocument<1024> doc)
{
    #ifdef DEBUG_SERIAL_WIFI
        Serial.println("... WiFi- Variablen ...");
    #endif
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
