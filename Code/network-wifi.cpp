#include "network-wifi.hpp"

boolean AP_Mode = false;

void wlan_connect()
{
    WiFi.setHostname(config.esp_name.c_str());
    WiFi.begin(config.wifi_ssid.c_str(), config.wifi_pw.c_str());

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
        if (WiFi.SSID(network) == config.wifi_ssid)
        {
            wifi_zahl++;
        }
    }
    if (wifi_zahl > 0)
    {
        if (config.wifi_dhcp == false)
        {
            IPAddress temp_ip = ipwandeln(config.wifi_ip);
            IPAddress temp_gw = ipwandeln(config.wifi_gw);
            IPAddress temp_subnet = ipwandeln(config.wifi_subnet);
            IPAddress temp_dns = ipwandeln(config.wifi_dns);
            
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
        WiFi.softAP(config.esp_name.c_str());
        delay(1000);
        WiFi.softAPConfig(IP, gateway, NMask);
        delay(1000);
        IPAddress myIP = WiFi.softAPIP();
        Serial.println();
        Serial.print("AP IP address: ");
        Serial.println(myIP);
        Serial.println("mit der SSID : " + config.esp_name);
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