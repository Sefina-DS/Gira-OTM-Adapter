#include "SysHeaders.h"

void filter(byte msg[10], int size)
{
    String topic_temp = "";
    int bitTemp = 0;
    int order = 0;
    String orderSTR = "";
    char temp_input2[10];
    char temp_input4[10];
    char temp_input6[10];
    char temp_input8[10];
    char letter;
    String temp_msg = "";
    float temp_float = 0;
    unsigned long temp_long = 0;
    char *endptr;
    order = (((int)msg[0]) * 100) + (int)msg[1];
    for (int i = 0; i < 2; i++)
    {
        letter = msg[i];
        orderSTR += letter;
    }
    //      "order" wird ab hier gifiltert
    //      ACHTUNG in 2x DECIMAL !!!!!
    switch (order)
    {
    //      C4 / 6752 = Seriennummer
    case 6752:
        for (int i = 2; i < 10; i++)
        {
            letter = msg[i];
            temp_msg += letter;
        }
        mqtt_link("Melder-Diagnose/Seriennummer", temp_msg.c_str());
        break;
    //      C9 / 6757 = Betriebszeit
    case 6757:

        for (int i = 2; i < 10; i++)
        {
            temp_input8[i - 2] = msg[i];
        }
        temp_long = strtol(temp_input8, &endptr, 16);
        temp_long = (((temp_long / 4) / 60) / 60);
        mqtt_link("Melder-Diagnose/Betriebsstunden", (String(temp_long)).c_str());
        break;
    //      CC / 6767 = Batteriespannung + 2x Temperatur
    case 6767:
        for (int i = 6; i < 8; i++)
        {
            temp_input2[i - 6] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        temp_float = (temp_long / 2) - 20;
        mqtt_link("Melder-Diagnose/Temperatur-1", (String(temp_float)).c_str());
        for (int i = 8; i < 10; i++)
        {
            temp_input2[i - 8] = msg[i];
        }
        temp_float = strtol(temp_input2, &endptr, 16);
        temp_float = (temp_float / 2) - 20;
        mqtt_link("Melder-Diagnose/Temperatur-2", (String(temp_float)).c_str());
        for (int i = 2; i < 6; i++)
        {
            temp_input4[i - 2] = msg[i];
        }
        temp_float = strtol(temp_input4, &endptr, 16);
        temp_float = temp_float * 0.018369;
        mqtt_link("Melder-Diagnose/Batteriespannung", (String(temp_float)).c_str());
        break;
    //      CB / 6766 = Rauchkammer, Verschmutzung, Anzahl Optische- Alarme
    case 6766:
        for (int i = 6; i < 8; i++)
        {
            temp_input2[i - 6] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        mqtt_link("Melder-Diagnose/Anzahl_Optisch-Alarm", (String(temp_long)).c_str());
        for (int i = 8; i < 10; i++)
        {
            temp_input2[i - 8] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        mqtt_link("Melder-Diagnose/Verschmutzung_%", (String(temp_long)).c_str());
        for (int i = 2; i < 6; i++)
        {
            temp_input4[i - 2] = msg[i];
        }
        temp_long = strtol(temp_input4, &endptr, 16);
        temp_float = temp_long * 0.003223;
        mqtt_link("Melder-Diagnose/Rauchkammerwert", (String(temp_float)).c_str());
        break;
    //      CD / 6768 = Alarme, Testalarme
    case 6768:
        for (int i = 2; i < 4; i++)
        {
            temp_input2[i - 2] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        mqtt_link("Melder-Diagnose/Anzahl_Temperatur-Alarm", (String(temp_long)).c_str());
        for (int i = 4; i < 6; i++)
        {
            temp_input2[i - 4] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        mqtt_link("Melder-Diagnose/Anzahl_Test-Alarm_Local", (String(temp_long)).c_str());
        for (int i = 6; i < 8; i++)
        {
            temp_input2[i - 6] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        mqtt_link("Melder-Diagnose/Anzahl_Draht_Alarm", (String(temp_long)).c_str());
        for (int i = 8; i < 10; i++)
        {
            temp_input2[i - 8] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        mqtt_link("Melder-Diagnose/Anzahl_Funk_Alarm", (String(temp_long)).c_str());
        break;
    //      CE / 6769 = Alarme, Testalarme
    case 6769:
        for (int i = 2; i < 4; i++)
        {
            temp_input2[i - 2] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        mqtt_link("Melder-Diagnose/Anzahl_Test-Alarm_Kabel", (String(temp_long)).c_str());
        for (int i = 4; i < 6; i++)
        {
            temp_input2[i - 4] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        mqtt_link("Melder-Diagnose/Anzahl_Test-Alarm_Funk", (String(temp_long)).c_str());
        break;
    //      C2 / 6750 || 82 / 5650 Statusmeldungen
    case 6750:
    case 5650:
        bitTemp = msg[2];
        switch (bitTemp)
        {
        case 48: // 0
            mqtt_link("Melder-Diagnose/Spannungsversorgung Ext.", "true");
            mqtt_link("Melder-Status/Alarm Thermisch", "false");
            break;
        case 49: // 1
            mqtt_link("Melder-Diagnose/Spannungsversorgung Ext.", "true");
            mqtt_link("Melder-Status/Alarm Thermisch", "true");
            if (timer_alarm < millis())
            {
                timer_alarm = millis() + 300000;
                client.publish((mqtt.topic_base + "/" + group_control + config.detector_group + "/" + "Alarm").c_str(), "true");
            }
            break;
        case 50: // 2
            mqtt_link("Melder-Diagnose/Spannungsversorgung Ext.", "false");
            mqtt_link("Melder-Status/Alarm Thermisch", "false");
            break;
        case 51: // 3
            mqtt_link("Melder-Diagnose/Spannungsversorgung Ext.", "false");
            mqtt_link("Melder-Status/Alarm Thermisch", "true");
            if (timer_alarm < millis())
            {
                timer_alarm = millis() + 300000;
                client.publish((mqtt.topic_base + "/" + group_control + config.detector_group + "/" + "Alarm").c_str(), "true");
            }
            break;
        default:
            break;
        }
        bitTemp = msg[3];
        switch (bitTemp)
        {
        case 48: // 0
            mqtt_link("Melder-Status/Taster Melder", "false");
            break;
        case 56: // 8
            serial_send("030200");
            topic_temp = "";
            for (int i = 0; i < config.detector_alarm_group_size + 1; i++)
            {
                topic_temp = mqtt.topic_base + "/" + group_control + config.detector_alarm_group_int[i] + "/";
                client.publish((topic_temp + "Alarm").c_str(), "false");
                client.publish((topic_temp + "Testalarm").c_str(), "false");
            }
            mqtt_link("Melder-Status/Taster Melder", "true");
            mqtt_link("Melder-Status/Alarm-Funk", "false");
            mqtt_link("Melder-Status/Alarm Optisch", "false");
            mqtt_link("Melder-Status/Alarm Thermisch", "false");
            break;
        default:
            break;
        }
        bitTemp = msg[4];
        switch (bitTemp)
        {
        case 48: // 0
            mqtt_link("Melder-Status/Testalarm-Funk", "false");
            mqtt_link("Melder-Status/Alarm-Funk", "false");
            break;
        case 49: // 1
            mqtt_link("Melder-Status/Alarm-Funk", "true");
            break;
        case 56: // 8
            mqtt_link("Melder-Status/Testalarm-Funk", "true");
            break;

        default:
            break;
        }
        bitTemp = msg[5];
        switch (bitTemp)
        {
        case 48: // 0
            mqtt_link("Melder-Status/Alarm Optisch", "false");
            break;
        case 52: // 4
            mqtt_link("Melder-Status/Alarm Optisch", "true");
            if (timer_alarm < millis())
            {
                timer_alarm = millis() + 300000;
                client.publish((mqtt.topic_base + "/" + group_control + config.detector_group + "/" + "Alarm").c_str(), "true");
            }
            break;
        default:
            break;
        }
        break;
    default:
        Serial.print("Achtung, falscher Wert / ");
        Serial.print("Order String : ");
        Serial.print(orderSTR);
        Serial.print(" | Decimal : ");
        Serial.println(order);
        break;
    }
}

void mqtt_link(String topic, String msg)
{
    String temp = mqtt.topic_base + "/" + mqtt.topic_define + "/" + topic;
    mqtt_publish(temp, msg);
}
