#include "SysHeaders.h"

DETECTORDIAG detectordiag;

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
    #ifdef DEBUG_SERIAL_OUTPUT
        Serial.print("Variable orderSTR : ");
        Serial.println(orderSTR);
    #endif
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
        detectordiag.serial_nr = temp_msg;
        if ( detectordiag.status == 5 ) detectordiag.status ++;
        //mqtt_link("Melder-Diagnose/Seriennummer", temp_msg.c_str());

        break;
    //      C9 / 6757 = Betriebszeit
    case 6757:

        for (int i = 2; i < 10; i++)
        {
            temp_input8[i - 2] = msg[i];
        }
        temp_long = strtol(temp_input8, &endptr, 16);
        temp_long = (((temp_long / 4) / 60) / 60);
        detectordiag.time = String(temp_long);
        if ( detectordiag.status == 1 ) detectordiag.status ++;
        //mqtt_link("Melder-Diagnose/Betriebsstunden", (String(temp_long)).c_str());
        break;
    //      CC / 6767 = Batteriespannung + 2x Temperatur
    case 6767:
        for (int i = 6; i < 8; i++)
        {
            temp_input2[i - 6] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        temp_float = (temp_long / 2) - 20;
        detectordiag.temp_1 = String(temp_float);
        //mqtt_link("Melder-Diagnose/Temperatur-1", (String(temp_float)).c_str());
        for (int i = 8; i < 10; i++)
        {
            temp_input2[i - 8] = msg[i];
        }
        temp_float = strtol(temp_input2, &endptr,6);
        temp_float = (temp_float / 2) - 20;
        detectordiag.temp_2 = String(temp_float);
        //mqtt_link("Melder-Diagnose/Temperatur-2", (String(temp_float)).c_str());
        for (int i = 2; i < 6; i++)
        {
            temp_input4[i - 2] = msg[i];
        }
        temp_float = strtol(temp_input4, &endptr, 16);
        temp_float = temp_float * 0.018369;
        detectordiag.ub_batterie = String(temp_float);
        if ( detectordiag.status == 7 ) detectordiag.status ++;
        //mqtt_link("Melder-Diagnose/Batteriespannung", (String(temp_float)).c_str());
        break;
    //      CB / 6766 = Rauchkammer, Verschmutzung, Anzahl Optische- Alarme
    case 6766:
        for (int i = 6; i < 8; i++)
        {
            temp_input2[i - 6] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        detectordiag.nr_alarm_l_opti = String(temp_long);
        //mqtt_link("Melder-Diagnose/Anzahl_Optisch-Alarm", (String(temp_long)).c_str());
        for (int i = 8; i < 10; i++)
        {
            temp_input2[i - 8] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        detectordiag.optical_dirt = String(temp_long);
        //mqtt_link("Melder-Diagnose/Verschmutzung_%", (String(temp_long)).c_str());
        for (int i = 2; i < 6; i++)
        {
            temp_input4[i - 2] = msg[i];
        }
        temp_long = strtol(temp_input4, &endptr, 16);
        temp_float = temp_long * 0.003223;
        detectordiag.optical_smoke = String(temp_float);
        if ( detectordiag.status == 9 ) detectordiag.status ++;
        //mqtt_link("Melder-Diagnose/Rauchkammerwert", (String(temp_float)).c_str());
        break;
    //      CD / 6768 = Alarme, Testalarme
    case 6768:
        for (int i = 2; i < 4; i++)
        {
            temp_input2[i - 2] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        detectordiag.nr_alarm_l_temp = String(temp_long);
        //mqtt_link("Melder-Diagnose/Anzahl_Temperatur-Alarm", (String(temp_long)).c_str());
        for (int i = 4; i < 6; i++)
        {
            temp_input2[i - 4] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        detectordiag.nr_talarm_l = String(temp_long);
        //mqtt_link("Melder-Diagnose/Anzahl_Test-Alarm_Local", (String(temp_long)).c_str());
        for (int i = 6; i < 8; i++)
        {
            temp_input2[i - 6] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        detectordiag.nr_alarm_k = String(temp_long);
        //mqtt_link("Melder-Diagnose/Anzahl_Draht_Alarm", (String(temp_long)).c_str());
        for (int i = 8; i < 10; i++)
        {
            temp_input2[i - 8] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        detectordiag.nr_alarm_f = String(temp_long);
        //mqtt_link("Melder-Diagnose/Anzahl_Funk_Alarm", (String(temp_long)).c_str());
        if ( detectordiag.status == 11 ) detectordiag.status ++;
        break;
    //      CE / 6769 = Alarme, Testalarme
    case 6769:
        for (int i = 2; i < 4; i++)
        {
            temp_input2[i - 2] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        detectordiag.nr_talarm_k = String(temp_long);
        //mqtt_link("Melder-Diagnose/Anzahl_Test-Alarm_Kabel", (String(temp_long)).c_str());
        for (int i = 4; i < 6; i++)
        {
            temp_input2[i - 4] = msg[i];
        }
        temp_long = strtol(temp_input2, &endptr, 16);
        detectordiag.nr_talarm_f = String(temp_long);
        //mqtt_link("Melder-Diagnose/Anzahl_Test-Alarm_Funk", (String(temp_long)).c_str());
        if ( detectordiag.status == 13 ) detectordiag.status ++;
        break;
    //      C2 / 6750 || 82 / 5650 Statusmeldungen
    case 6750:
    case 5650:
        bitTemp = msg[2];
        switch (bitTemp)
        {
        case 48: // 0
            //mqtt_link("Melder-Diagnose/Spannungsversorgung Ext.", "true");
            detectordiag.ub_ext = "true";
            mqtt_link("Melder-Status/Alarm Thermisch", "false");
            break;
        case 49: // 1
            //mqtt_link("Melder-Diagnose/Spannungsversorgung Ext.", "true");
            detectordiag.ub_ext = "true";
            mqtt_link("Melder-Status/Alarm Thermisch", "true");
            if (detector.timer < millis())
            {
                detector.timer = millis() + 300000;
                mqtt_publish(mqtt.topic_base + "/" + group_control + detector.group + "/" + "Alarm_Funk", "true", "filter");
            }
            break;
        case 50: // 2
            //mqtt_link("Melder-Diagnose/Spannungsversorgung Ext.", "false");
            detectordiag.ub_ext = "false";
            mqtt_link("Melder-Status/Alarm Thermisch", "false");
            break;
        case 51: // 3
            //mqtt_link("Melder-Diagnose/Spannungsversorgung Ext.", "false");
            detectordiag.ub_ext = "false";
            mqtt_link("Melder-Status/Alarm Thermisch", "true");
            if (detector.timer < millis())
            {
                detector.timer = millis() + 300000;
                mqtt_publish(mqtt.topic_base + "/" + group_control + detector.group + "/" + "Alarm_Funk", "true", "filter");
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
            if (comserial.com_status > 0) serial_send("030200", 1);
            topic_temp = "";
            for (int i = 0; i < detector.alarm_group_size + 1; i++)
            {
                topic_temp = mqtt.topic_base + "/" + group_control + detector.alarm_group[i] + "/";
                mqtt_publish(topic_temp + "Alarm_Funk", "false", "filter");
                mqtt_publish(topic_temp + "Testalarm_Funk", "false", "filter");
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
            if (detector.timer < millis())
            {
                detector.timer = millis() + 300000;
                mqtt_publish(mqtt.topic_base + "/" + group_control + detector.group + "/" + "Alarm_Funk", "true", "filter");
            }
            break;
        default:
            break;
        }
        if ( detectordiag.status == 3 ) detectordiag.status ++;
        break;
    default:
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.print("Achtung, falscher Wert / ");
            Serial.print("Order String : ");
            Serial.print(orderSTR);
            Serial.print(" | Decimal : ");
            Serial.println(order);
        #endif
        break;
    }
}

void mqtt_link(String topic, String msg)
{
    String temp = mqtt.topic_base + "/" + mqtt.topic_define + "/" + topic;
    mqtt_publish(temp, msg, "mqtt_link");
}

void detector_serial_timer()
{
    switch ( detectordiag.status )
    {
        // Betriebszeit
        case 0:
        if ( comserial.com_status == 0 ) 
        {
            serial_send("09", 1);
            detectordiag.status ++;

            break;
        }
        // Statusabfrage
        case 2:
        if ( comserial.com_status == 0 ) 
        {
            serial_send("02", 1);
            detectordiag.status ++;

            break;
        }
        // Seriennummer
        case 4:
        if ( comserial.com_status == 0 ) 
        {
            serial_send("04", 1);
            detectordiag.status ++;

            break;
        }
        // Batterie + 2x Temperaturfühler 45
        case 6:
        if ( comserial.com_status == 0 ) 
        {
            serial_send("0C", 1);
            detectordiag.status ++;

            break;
        }
        // Rauchkammer + Verschmutzung + Anzahl Optische Alarme 65
        case 8:
        if ( comserial.com_status == 0 ) 
        {
            serial_send("0B", 1);
            detectordiag.status ++;

            break;
        }
        // Alarme - 1 85
        case 10:
        if ( comserial.com_status == 0 ) 
        {
            serial_send("0D", 1);
            detectordiag.status ++;

            break;
        }
        // Alarme - 2 105
        case 12:
        if ( comserial.com_status == 0 ) 
        {
            serial_send("0E", 1);
            detectordiag.status ++;

            break;
        }
        // Finish
        case 14:
        if ( client.connected() )
        {
            StaticJsonDocument<1024> temp_json;
            String temp_string;
  
            temp_json["Counter_Test_Kabel"]         = detectordiag.nr_talarm_k;
            temp_json["Counter_Test_Funk"]          = detectordiag.nr_talarm_f;
            temp_json["Counter_Test_Local"]         = detectordiag.nr_talarm_l;
            temp_json["Counter_Alarm_Kabel"]        = detectordiag.nr_alarm_k;
            temp_json["Counter_Alarm_Funk"]         = detectordiag.nr_alarm_f;
            temp_json["Counter_Alarm_Optisch"]      = detectordiag.nr_alarm_l_opti;
            temp_json["Counter_Alarm_Thermisch"]    = detectordiag.nr_alarm_l_temp;
            temp_json["Sensor_Verschmutzung"]       = detectordiag.optical_dirt;
            temp_json["Sensor_Rauchwert"]           = detectordiag.optical_smoke;
            temp_json["Temperatur_1"]               = detectordiag.temp_1;
            temp_json["Temperatur_2"]               = detectordiag.temp_2;
            temp_json["UB_Extern"]                  = detectordiag.ub_ext;
            temp_json["UB_Batterie"]                = detectordiag.ub_batterie;
            temp_json["Seriennummer"]               = detectordiag.serial_nr;
            temp_json["Betriebsstunden"]            = detectordiag.time;

            serializeJson(temp_json, temp_string);
            mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/Detector/" + "-Diagnose-", temp_string, "detector_serial_timer");

            detectordiag.status = 0;
            detectordiag.timer = millis() + 600000;

        }


        default:
            break;
    }
}