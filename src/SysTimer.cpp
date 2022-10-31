#include "SysHeaders.h"

TIMER timer;

void timer_funktion()
{
    String msg;
    String temp_topic;
    int temp = 0;
    static int nr = 0;
    if (nr == 120)
    {
        nr = 0;
    }
    nr++;
    
    /*Serial.print("Timerdurchlauf : ");
    Serial.println(nr);*/
    if (nr % 2 == 0)
    {
        led_flash_timer(100 ,0 ,1);
        if (WiFi.isConnected() &&
            mqtt.aktiv &&
            !client.connected())
        {
            mqtt_connect();
        }
        if (!WiFi.isConnected() &&
            !AP_Mode)
        {
            wlan_connect();
        }
        if (sensor.bme != "keiner vorhanden")
        {
            bme_refresh();
        }
    }
    if (nr % 2 == 0)
    {
        if (sensor.ubext)
        {
            ubext_refresh();
        }
        if (sensor.light)
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
        //comserial.com_status = 1;
        if ( comserial.com_status == 0 ) serial_send("09", 1);
        temp = ((millis() / 1000) / 60) / 60;
        temp_topic = mqtt.topic_base + "/" + mqtt.topic_define + "/" + "ESP_Status/";
        mqtt_publish( temp_topic + "Betriebsstunden", String(temp));

        break;
    // Statusmeldungen
    case 13:
        //comserial.com_status = 1;
        if ( comserial.com_status == 0 ) serial_send("02", 1);
        break;
    // Serienummer 25
    case 25:
        //seri_status = 1;
        if ( comserial.com_status == 0 ) serial_send("04", 1);
        break;
    // Batterie + 2x Temperaturfühler 45
    case 45:
        //seri_status = 1;
        if ( comserial.com_status == 0 ) serial_send("0C", 1);
        break;
    // Rauchkammer + Rauchalarme + Verschmutzung 65
    case 65:
        //seri_status = 1;
        if ( comserial.com_status == 0 ) serial_send("0B", 1);
        break;
    // Alarme - 1 85
    case 85:
        //seri_status = 1;
        if ( comserial.com_status == 0 ) serial_send("0D", 1);
        break;
    // Alarme - 2 105
    case 105:
        //seri_status = 1;
        if ( comserial.com_status == 0 ) serial_send("0E", 1);
        break;
    // Abbruch
    default:
        break;
    }
}
