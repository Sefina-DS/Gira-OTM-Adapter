#include "SysHeaders.h"

DETECTORDIAG detectordiag;

void serial_transceive_diagnose() {
    if ( millis() <= detectordiag.timer ) return;
    switch (detectordiag.counter)
    {
        case 0:
            if ( serial_transceive("C4") ) {
                detectordiag.counter++;
                detectordiag.timer = millis() + 10000;

            } else {
                detectordiag.timer = millis() + 5000;
            }
            break;
        case 1:
            if ( serial_transceive("C9") ) {
                detectordiag.counter++;
                detectordiag.timer = millis() + 10000;

            } else {
                detectordiag.timer = millis() + 5000;
            }
            break;
        case 2:
            if ( serial_transceive("CC") ) {
                detectordiag.counter++;
                detectordiag.timer = millis() + 10000;

            } else {
                detectordiag.timer = millis() + 5000;
            }
            break;
        case 3:
            if ( serial_transceive("CB") ) {
                detectordiag.counter++;
                detectordiag.timer = millis() + 10000;

            } else {
                detectordiag.timer = millis() + 5000;
            }
            break;
        case 4:
            if ( serial_transceive("CD") ) {
                detectordiag.counter++;
                detectordiag.timer = millis() + 10000;

            } else {
                detectordiag.timer = millis() + 5000;
            }
            break;
        case 5:
            if ( serial_transceive("CE") ) {
                detectordiag.counter++;
                detectordiag.timer = millis() + 10000;

            } else {
                detectordiag.timer = millis() + 5000;
            }
            break;
        case 6:
            if ( serial_transceive("C2") ) {
                detectordiag.counter++;
                detectordiag.timer = millis() + 10000;

            } else {
                detectordiag.timer = millis() + 5000;
            }
            break;
        case 7:
            #ifdef DEBUG_SERIAL_DETECTOR
                Serial.print("Neuer Versuch die Diagnose zu versenden ....");
            #endif
            StaticJsonDocument<1024> temp_json;
            String temp_string;
            temp_json["Counter_Test_Kabel"]         = detectordiag.nr_talarm_k;
            temp_json["Counter_Test_Funk"]          = detectordiag.nr_talarm_f;
            temp_json["Counter_Test_Local"]         = detectordiag.nr_talarm_l;
            temp_json["Counter_Alarm_Kabel"]        = detectordiag.nr_alarm_k;
            temp_json["Counter_Alarm_Funk"]         = detectordiag.nr_alarm_f;
            temp_json["Counter_Alarm_Optisch"]      = detectordiag.nr_alarm_l_opti;
            temp_json["Counter_Alarm_Thermisch"]    = detectordiag.nr_alarm_l_temp;
            //temp_json["Sensor_Verschmutzung"]       = detectordiag.optical_dirt;
            //temp_json["Sensor_Rauchwert"]           = detectordiag.optical_smoke;
            temp_json["Temperatur_1"]               = detectordiag.temp_1;
            temp_json["Temperatur_2"]               = detectordiag.temp_2;
            temp_json["UB_Extern"]                  = detectordiag.ub_ext;
            temp_json["UB_Batterie"]                = detectordiag.ub_batterie;
            temp_json["Seriennummer"]               = detectordiag.serial_nr;
            temp_json["Betriebsstunden"]            = detectordiag.time;
            serializeJson(temp_json, temp_string);
            if ( mqtt_publish ( "Detector-Status/Diagnose", temp_string ) ) {
                #ifdef DEBUG_SERIAL_DETECTOR
                    Serial.println("Erfolgreich, Diagnose startet in 15 Minuten.");
                #endif
                detectordiag.counter = 0 ;
                detectordiag.timer = millis() + ( 1000 * 60 * 15 ) ;
            } else {
                #ifdef DEBUG_SERIAL_DETECTOR
                    Serial.println("Fehlgeschlagen, neuer Sendeversuch in 5 Sekunden.");
                #endif
                detectordiag.timer = millis() + 5000;
            }
    }
}

void serial_receive_diagnose(String msg) {
    // Extrahiere die ersten beiden Stellen der Nachricht
    String topic = msg.substring(0, 2);
    String temp_string;
    long temp_long;
    float temp_float;
    char *endptr;
    #ifdef DEBUG_SERIAL_DETECTOR
        Serial.print("Topic: ");
        Serial.println(topic);
    #endif
    if              ( topic == "C4") {
        temp_string = msg.substring(2);
        detectordiag.serial_nr = temp_string;
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Seriennummer: ");
            Serial.println(temp_string);
        #endif
    } else if       ( topic == "C9") {
        temp_string = msg.substring(2);
        temp_long = strtol(temp_string.c_str(), &endptr, 16);
        temp_long = (((temp_long / 4) / 60) / 60);
        detectordiag.time = String(temp_long);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Betriebszeit: ");
            Serial.println(temp_long);
        #endif
    } else if       (topic == "CC") {
        // Extrahiere die Batteriespannung
        temp_string = msg.substring(2, 6); // Extrahiere die Batteriespannung von den Stellen 2 bis 5
        temp_long = strtol(temp_string.c_str(), &endptr, 16); // Konvertiere den Hexadezimalwert in einen Long
        temp_float = temp_long * 9184.0 / 5000.0 ; // Berechne die Batteriespannung gemäß der gegebenen Formel
        temp_float = round(temp_float / 10.0) / 10.0;
        detectordiag.ub_batterie = String(temp_float); // Speichere die Batteriespannung als String
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Batteriespannung: ");
            Serial.println(temp_float); // Gebe die Batteriespannung für Debug-Zwecke aus
        #endif
        // Extrahiere Temperatur #1
        temp_string = msg.substring(6, 8); // Extrahiere die Temperatur #1 von den Stellen 6 bis 7
        temp_long = strtol(temp_string.c_str(), &endptr, 16); // Konvertiere den Hexadezimalwert in einen Long
        temp_float = (temp_long * 50.0) - 2000.0 ; // Berechne die Temperatur #1 gemäß der gegebenen Formel
        temp_float = round(temp_float / 10.0) / 10.0;
        detectordiag.temp_1 = String(temp_float); // Speichere die Temperatur #1 als String
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Temperatur #1: ");
            Serial.println(temp_float); // Gebe die Temperatur #1 für Debug-Zwecke aus
        #endif
        // Extrahiere Temperatur #2
        temp_string = msg.substring(8, 10); // Extrahiere die Temperatur #2 von den Stellen 8 bis 9
        temp_long = strtol(temp_string.c_str(), &endptr, 16); // Konvertiere den Hexadezimalwert in einen Long
        temp_float = (temp_long * 50.0) - 2000.0 ; // Berechne die Temperatur #1 gemäß der gegebenen Formel
        temp_float = round(temp_float / 10.0) / 10.0;
        detectordiag.temp_2 = String(temp_float); // Speichere die Temperatur #1 als String
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Temperatur #2: ");
            Serial.println(temp_float); // Gebe die Temperatur #1 für Debug-Zwecke aus
        #endif
    } else if       (topic == "CB") {
        // Extrahiere den Rauchkammerwert
        temp_string = msg.substring(2, 6); // Extrahiere den Rauchkammerwert von den Stellen 2 bis 5
        temp_long = strtol(temp_string.c_str(), &endptr, 16); // Konvertiere den Hexadezimalwert in einen Long
        temp_long = temp_long - 100;
        detectordiag.optical_smoke = temp_long;
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Rauchkammer Wert: ");
            Serial.println(temp_long); // Gebe den Rauchkammerwert für Debug-Zwecke aus
        #endif
        // Extrahiere die Anzahl der Rauchalarme
        temp_string = msg.substring(6, 8); // Extrahiere die Anzahl der Rauchalarme von den Stellen 6 bis 7
        temp_long = strtol(temp_string.c_str(), &endptr, 16); // Konvertiere den Hexadezimalwert in einen Long
        detectordiag.nr_alarm_l_opti = temp_long;
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Anzahl der Rauchalarme: ");
            Serial.println(temp_long); // Gebe die Anzahl der Rauchalarme für Debug-Zwecke aus
        #endif
        // Extrahiere den Verschmutzungsgrad der Rauchkammer
        temp_string = msg.substring(8, 10); // Extrahiere den Verschmutzungsgrad der Rauchkammer von den Stellen 8 bis 9
        temp_long = strtol(temp_string.c_str(), &endptr, 16); // Konvertiere den Hexadezimalwert in einen Long
        detectordiag.optical_dirt = temp_long;
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Rauchkammer Verschmutzungsgrad: ");
            Serial.println(temp_long); // Gebe den Verschmutzungsgrad der Rauchkammer für Debug-Zwecke aus
        #endif
    } else if       (topic == "CD") {
        // Extrahiere die Anzahl der lokalen Temperaturalarme
        temp_string = msg.substring(2, 4); // Extrahiere die Anzahl der lokalen Temperaturalarme von den Stellen 2 bis 3
        temp_long = strtol(temp_string.c_str(), &endptr, 16); // Konvertiere den Hexadezimalwert in einen Long
        detectordiag.nr_alarm_l_temp = String(temp_long);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Anzahl der lokalen Temperatur Alarme: ");
            Serial.println(temp_long); // Gebe die Anzahl der lokalen Temperaturalarme für Debug-Zwecke aus
        #endif
        // Extrahiere die Anzahl der lokalen Testalarme
        temp_string = msg.substring(4, 6); // Extrahiere die Anzahl der lokalen Testalarme von den Stellen 4 bis 5
        temp_long = strtol(temp_string.c_str(), &endptr, 16); // Konvertiere den Hexadezimalwert in einen Long
        detectordiag.nr_talarm_l = String(temp_long);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Anzahl der lokalen Test Alarme: ");
            Serial.println(temp_long); // Gebe die Anzahl der lokalen Testalarme für Debug-Zwecke aus
        #endif
        // Extrahiere die Anzahl der Alarme über Kabel
        temp_string = msg.substring(6, 8); // Extrahiere die Anzahl der Alarme über Kabel von den Stellen 6 bis 7
        temp_long = strtol(temp_string.c_str(), &endptr, 16); // Konvertiere den Hexadezimalwert in einen Long
        detectordiag.nr_alarm_k = String(temp_long);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Anzahl der Alarme über Kabel: ");
            Serial.println(temp_long); // Gebe die Anzahl der Alarme über Kabel für Debug-Zwecke aus
        #endif
        // Extrahiere die Anzahl der Alarme über Funk
        temp_string = msg.substring(8, 10); // Extrahiere die Anzahl der Alarme über Funk von den Stellen 8 bis 9
        temp_long = strtol(temp_string.c_str(), &endptr, 16); // Konvertiere den Hexadezimalwert in einen Long
        detectordiag.nr_alarm_f = String(temp_long);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Anzahl der Alarme über Funk: ");
            Serial.println(temp_long); // Gebe die Anzahl der Alarme über Funk für Debug-Zwecke aus
        #endif
    } else if       (topic == "CE") {
        // Extrahiere die Anzahl der Testalarme über Kabel
        temp_string = msg.substring(2, 4); // Extrahiere die Anzahl der Testalarme über Kabel von den Stellen 2 bis 3
        temp_long = strtol(temp_string.c_str(), &endptr, 16); // Konvertiere den Hexadezimalwert in einen Long
        detectordiag.nr_talarm_k = String(temp_long);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Anzahl der Test Alarme über Kabel: ");
            Serial.println(temp_long); // Gebe die Anzahl der Testalarme über Kabel für Debug-Zwecke aus
        #endif
        // Extrahiere die Anzahl der Testalarme über Funk
        temp_string = msg.substring(4, 6); // Extrahiere die Anzahl der Testalarme über Funk von den Stellen 4 bis 5
        temp_long = strtol(temp_string.c_str(), &endptr, 16); // Konvertiere den Hexadezimalwert in einen Long
        detectordiag.nr_talarm_f = String(temp_long);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Anzahl der Test Alarme über Funk: ");
            Serial.println(temp_long); // Gebe die Anzahl der Testalarme über Funk für Debug-Zwecke aus
        #endif
    } else if       (topic == "C2" || topic == "82") {
        temp_string = msg.substring(2, 3);
        Serial.println(temp_string);
        if ( temp_string == "0" ) { detectordiag.ub_ext = "true"; } else { detectordiag.ub_ext = "false"; }
        
        // Erstes Byte
        temp_string = msg.substring(2, 4);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Byte 1 : ");
            Serial.println(temp_string);
        #endif

        // Zweites Byte
        temp_string = msg.substring(4, 6);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Byte 2 : ");
            Serial.println(temp_string);
        #endif

        // Drites Byte
        temp_string = msg.substring(6, 8);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Byte 3 : ");
            Serial.println(temp_string);
        #endif

        // Fiertes Byte
        temp_string = msg.substring(8, 10);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Byte 4 : ");
            Serial.println(temp_string);
        #endif

    }
   
    // Hier kannst du den Code einfügen, um die extrahierten Stellen zu verarbeiten
    // Zum Beispiel: Vergleiche mit einem anderen Wert

}

/*
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
            if (comserial.com_status > 0) //serial_send("030200", 1);
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
            //serial_send("09", 1);
            detectordiag.status ++;

            break;
        }
        // Statusabfrage
        case 2:
        if ( comserial.com_status == 0 ) 
        {
            //serial_send("02", 1);
            detectordiag.status ++;

            break;
        }
        // Seriennummer
        case 4:
        if ( comserial.com_status == 0 ) 
        {
            //serial_send("04", 1);
            detectordiag.status ++;

            break;
        }
        // Batterie + 2x Temperaturfühler 45
        case 6:
        if ( comserial.com_status == 0 ) 
        {
            //serial_send("0C", 1);
            detectordiag.status ++;

            break;
        }
        // Rauchkammer + Verschmutzung + Anzahl Optische Alarme 65
        case 8:
        if ( comserial.com_status == 0 ) 
        {
            //serial_send("0B", 1);
            detectordiag.status ++;

            break;
        }
        // Alarme - 1 85
        case 10:
        if ( comserial.com_status == 0 ) 
        {
            //serial_send("0D", 1);
            detectordiag.status ++;

            break;
        }
        // Alarme - 2 105
        case 12:
        if ( comserial.com_status == 0 ) 
        {
            //serial_send("0E", 1);
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
            //temp_json["Counter_Alarm_Optisch"]      = detectordiag.nr_alarm_l_opti;
            //temp_json["Counter_Alarm_Thermisch"]    = detectordiag.nr_alarm_l_temp;
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
*/