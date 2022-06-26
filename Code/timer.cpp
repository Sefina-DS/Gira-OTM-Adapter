#include "timer.hpp"

void timer_funktion()
{
    String msg;
    int temp = 0;
    static int nr = 0;
    if (nr == 120)
    {
        nr = 0;
    }
    nr++;
    if (WiFi.isConnected() &&
        client.connected())
    {
        if (config.bluetooth)
        {
            bluetooth_scan();
        }
    }
    /*Serial.print("Timerdurchlauf : ");
    Serial.println(nr);*/
    if (nr % 2 == 0)
    {
        led_flash_timer(100 ,0 ,1);
        if (WiFi.isConnected() &&
            config.mqtt &&
            !client.connected())
        {
            mqtt_connect();
        }
        if (!WiFi.isConnected() &&
            !AP_Mode)
        {
            wlan_connect();
        }
        if (WiFi.isConnected() &&
            client.connected())
        {
            if (seri_run == true)
            {
                msg = "true";
            }
            else
            {
                msg = "false";
            }
            mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Komunikation", String(msg));
            if (config.bme_280)
            {
                bme_refresh();
            }
        }
    }
    if (nr % 2 == 0)
    {
        if (config.ubext)
        {
            ubext_refresh();
        }
        if (config.light)
        {
            light_refresh();
        }
    }

    switch (nr)
    {
    case 22:
    case 52:
    case 82:
    case 112:
        seri_status = 1;
        serial_send("09");
        temp = ((millis() / 1000) / 60) / 60;
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + esp_status + "Betriebsstunden").c_str(), (String(temp)).c_str());
        break;
    // Statusmeldungen
    case 13:
        seri_status = 1;
        serial_send("02");
        break;
    // Serienummer 25
    case 25:
        seri_status = 1;
        serial_send("04");
        break;
    // Batterie + 2x Temperaturfühler 45
    case 45:
        seri_status = 1;
        serial_send("0C");
        break;
    // Rauchkammer + Rauchalarme + Verschmutzung 65
    case 65:
        seri_status = 1;
        serial_send("0B");
        break;
    // Alarme - 1 85
    case 85:
        seri_status = 1;
        serial_send("0D");
        break;
    // Alarme - 2 105
    case 105:
        seri_status = 1;
        serial_send("0E");
        break;
    // Abbruch
    default:
        break;
    }
}
