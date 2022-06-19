#include "serial_filter.hpp"

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
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Seriennummer").c_str(), temp_msg.c_str());
        break;
    //      C9 / 6757 = Betriebszeit
    case 6757:

        for (int i = 2; i < 10; i++)
        {
            temp_input8[i - 2] = msg[i];
        }
        temp_long = strtol(temp_input8, &endptr, 16);
        temp_long = (((temp_long / 4) / 60) / 60);
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Betriebsstunden").c_str(), (String(temp_long)).c_str());
        break;
    //      CC / 6767 = Batteriespannung + 2x Temperatur
    case 6767:
        for (int i = 6; i < 8; i++)
        {
            temp_input2[i - 6] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        temp_float = (temp_long / 2) - 20;
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Temperatur-1").c_str(), (String(temp_float)).c_str());
        for (int i = 8; i < 10; i++)
        {
            temp_input2[i - 8] = msg[i];
        }
        temp_float = strtol(temp_input2, &endptr, 16);
        temp_float = (temp_float / 2) - 20;
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Temperatur-2").c_str(), (String(temp_float)).c_str());
        for (int i = 2; i < 6; i++)
        {
            temp_input4[i - 2] = msg[i];
        }
        temp_float = strtol(temp_input4, &endptr, 16);
        temp_float = temp_float * 0.018369;
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Batteriespannung").c_str(), (String(temp_float)).c_str());
        break;
    //      CB / 6766 = Rauchkammer, Verschmutzung, Anzahl Optische- Alarme
    case 6766:
        for (int i = 6; i < 8; i++)
        {
            temp_input2[i - 6] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Anzahl_Optisch-Alarm").c_str(), (String(temp_long)).c_str());
        for (int i = 8; i < 10; i++)
        {
            temp_input2[i - 8] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Verschmutzung_%").c_str(), (String(temp_long)).c_str());
        for (int i = 2; i < 6; i++)
        {
            temp_input4[i - 2] = msg[i];
        }
        temp_long = strtol(temp_input4, &endptr, 16);
        temp_float = temp_long * 0.003223;
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Rauchkammerwert").c_str(), (String(temp_float)).c_str());
        break;
    //      CD / 6768 = Alarme, Testalarme
    case 6768:
        for (int i = 2; i < 4; i++)
        {
            temp_input2[i - 2] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Anzahl_Temperatur-Alarm").c_str(), (String(temp_long)).c_str());
        for (int i = 4; i < 6; i++)
        {
            temp_input2[i - 4] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Anzahl_Test-Alarm_Local").c_str(), (String(temp_long)).c_str());
        for (int i = 6; i < 8; i++)
        {
            temp_input2[i - 6] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Anzahl_Draht_Alarm").c_str(), (String(temp_long)).c_str());
        for (int i = 8; i < 10; i++)
        {
            temp_input2[i - 8] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Anzahl_Funk_Alarm").c_str(), (String(temp_long)).c_str());
        break;
    //      CE / 6769 = Alarme, Testalarme
    case 6769:
        for (int i = 2; i < 4; i++)
        {
            temp_input2[i - 2] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Anzahl_Test-Alarm_Kabel").c_str(), (String(temp_long)).c_str());
        for (int i = 4; i < 6; i++)
        {
            temp_input2[i - 4] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Anzahl_Test-Alarm_Funk").c_str(), (String(temp_long)).c_str());
        break;
    //      C2 / 6750 || 82 / 5650 Statusmeldungen
    case 6750:
    case 5650:
        bitTemp = msg[2];
        switch (bitTemp)
        {
        case 48: // 0
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Spannungsversorgung Ext.").c_str(), "true");
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Alarm Thermisch").c_str(), "false");
            break;
        case 49: // 1
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Spannungsversorgung Ext.").c_str(), "true");
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Alarm Thermisch").c_str(), "true");
            if (timer_alarm < millis())
            {
                timer_alarm = millis() + 300000;
                client.publish((config.mqtt_topic_base + "/" + group_control + config.detector_group + "/" + "Alarm").c_str(), "true");
            }
            break;
        case 50: // 2
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Spannungsversorgung Ext.").c_str(), "false");
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Alarm Thermisch").c_str(), "false");
            break;
        case 51: // 3
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_diagnose + "Spannungsversorgung Ext.").c_str(), "false");
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Alarm Thermisch").c_str(), "true");
            if (timer_alarm < millis())
            {
                timer_alarm = millis() + 300000;
                client.publish((config.mqtt_topic_base + "/" + group_control + config.detector_group + "/" + "Alarm").c_str(), "true");
            }
            break;
        default:
            break;
        }
        bitTemp = msg[3];
        switch (bitTemp)
        {
        case 48: // 0
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Taster Melder").c_str(), "false");
            break;
        case 56: // 8
            serial_send("030200");
            topic_temp = "";
            for (int i = 0; i < config.detector_alarm_group_size + 1; i++)
            {
                topic_temp = config.mqtt_topic_base + "/" + group_control + config.detector_alarm_group_int[i] + "/";
                client.publish((topic_temp + "Alarm").c_str(), "false");
                client.publish((topic_temp + "Testalarm").c_str(), "false");
            }
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Taster Melder").c_str(), "true");
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Alarm-Funk").c_str(), "false");
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Alarm Optisch").c_str(), "false");
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Alarm Thermisch").c_str(), "false");
            break;
        default:
            break;
        }
        bitTemp = msg[4];
        switch (bitTemp)
        {
        case 48: // 0
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Testalarm-Funk").c_str(), "false");
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Alarm-Funk").c_str(), "false");
            break;
        case 49: // 1
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Alarm-Funk").c_str(), "true");
            break;
        case 56: // 8
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Testalarm-Funk").c_str(), "true");
            break;

        default:
            break;
        }
        bitTemp = msg[5];
        switch (bitTemp)
        {
        case 48: // 0
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Alarm Optisch").c_str(), "false");
            break;
        case 52: // 4
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Alarm Optisch").c_str(), "true");
            if (timer_alarm < millis())
            {
                timer_alarm = millis() + 300000;
                client.publish((config.mqtt_topic_base + "/" + group_control + config.detector_group + "/" + "Alarm").c_str(), "true");
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
