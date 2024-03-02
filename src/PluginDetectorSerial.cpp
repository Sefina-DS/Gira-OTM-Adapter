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
            temp_json["Sensor_Verschmutzung"]       = detectordiag.optical_dirt;
            temp_json["Sensor_Rauchwert"]           = detectordiag.optical_smoke;
            temp_json["Temperatur_1"]               = detectordiag.temp_1;
            temp_json["Temperatur_2"]               = detectordiag.temp_2;
            temp_json["UB_Extern"]                  = detectordiag.ub_ext;
            temp_json["UB_Batterie"]                = detectordiag.ub_batterie;
            temp_json["Seriennummer"]               = detectordiag.serial_nr;
            temp_json["Betriebsstunden"]            = detectordiag.time;
            serializeJson(temp_json, temp_string);
            if ( mqtt_publish ( "Detector-Status/0-Diagnose-0", temp_string ) ) {
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
        int error = 0;
        // -> Bit 1
        temp_string = msg.substring(2, 3);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Bit 1 : ");
            Serial.println(temp_string);
        #endif
        /*      UB-Ext-use  Temp-Alarm     
            0       X           -               
            1       X           X
            2       -           -
            3       -           X                                 
             
        */
        if ( temp_string == "0" || temp_string == "1" ) {   detectordiag.ub_ext = "true";       }
        else                                            {   detectordiag.ub_ext = "false";      }
        if ( temp_string == "1" || temp_string == "3" ) {   mqtt_publish ( "Detector-Status/Alarm-Temperatur", "true");
                                                            detector.alarm = true;
                                                            mqtt_publish_group ( detector.group, "Alarm", "true");     
                                                            if ( detector.alarm ) {
                                                                detector.alarm = false;
                                                                mqtt_publish_group ( detector.group, "Alarm", "false");}    }
        else                                            {   mqtt_publish ( "Detector-Status/Alarm-Temperatur", "false");    }
        //////
        if ( temp_string != "0" && temp_string != "1" && temp_string != "2" && temp_string != "3" ) ++error;
        
        // -> Bit 2
        temp_string = msg.substring(3, 4);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Bit 2 : ");
            Serial.println(temp_string);
        #endif
        if ( temp_string == "0" )                       {   mqtt_publish ( "Detector-Status/Taster", "false");      }
        else                                            {   mqtt_publish ( "Detector-Status/Taster", "true");
                                                            if ( detector.remote ) {
                                                                mqtt_publish_group ( detector.remote_grp, "Alarm", "false");}    }
                                                                
        //////
        if ( temp_string != "0" && temp_string != "8" ) ++error;
        
        // -> Bit 3
        temp_string = msg.substring(4, 5);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Bit 3 : ");
            Serial.println(temp_string);
        #endif
        /*      Test-Local  Test-Remote     Alarm-Remote
            0       -           -               -
            1       -           -               X
            2       X           -               -
            3       X           -               X
            8       -           X               -
            A       X           X               -
            Wenn Alarm-Remote ankommt wird der Test-Remote überschrieben !
        */
        if ( temp_string == "2" || temp_string == "3" || temp_string == "A" )   {   mqtt_publish ( "Detector-Status/Test-Local", "true");     }
        else                                                                    {   mqtt_publish ( "Detector-Status/Test-Local", "false");    }
        if ( temp_string == "8" || temp_string == "A" )                         {   mqtt_publish ( "Detector-Status/Test-Remote", "true");     }
        else                                                                    {   mqtt_publish ( "Detector-Status/Test-Remote", "false");    }
        if ( temp_string == "1" || temp_string == "3" )                         {   mqtt_publish ( "Detector-Status/Alarm-Remote", "true");     }
        else                                                                    {   mqtt_publish ( "Detector-Status/Alarm-Remote", "false");    }
        //////
        if ( temp_string != "0" && temp_string != "1" && temp_string != "2" && temp_string != "3" && temp_string != "8" && temp_string != "A" ) ++error;
        
        // -> Bit 4
        temp_string = msg.substring(5, 6);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Bit 4 : ");
            Serial.println(temp_string);
        #endif
        /*       Batterie    Optisch    Alarm-Kabel
            0       -           -           -
            1       X           -           -
            4       -           X           -
            5       X           X           -
            8       -           -           X
            9       X           -           X
        */
        if ( temp_string == "1" || temp_string == "5" || temp_string == "9" )   {   mqtt_publish ( "Detector-Status/Störung-Batterie", "true");     }
        else                                                                    {   mqtt_publish ( "Detector-Status/Störung-Batterie", "false");    }
        if ( temp_string == "4" || temp_string == "5" )                         {   mqtt_publish ( "Detector-Status/Alarm-Optisch", "true");
                                                                                    detector.alarm = true;
                                                                                    mqtt_publish_group ( detector.group, "Alarm", "true");     }
        else                                                                    {   mqtt_publish ( "Detector-Status/Alarm-Optisch", "false");
                                                                                    if ( detector.alarm ) {
                                                                                        detector.alarm = false;
                                                                                        mqtt_publish_group ( detector.group, "Alarm", "false");}    }
        if ( temp_string == "8" || temp_string == "9" )                         {   mqtt_publish ( "Detector-Status/Alarm-Kabel", "true");     }
        else                                                                    {   mqtt_publish ( "Detector-Status/Alarm-Kabel", "false");    }
        //////
        if ( temp_string != "0" && temp_string != "1" && temp_string != "4" && temp_string != "5" && temp_string != "8" && temp_string != "9" ) ++error;
        
        // -> Bit 5
        temp_string = msg.substring(6, 7);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Bit 5 : ");
            Serial.println(temp_string);
        #endif
        if      ( temp_string != "0" )  ++error;
        
        // -> Bit 6
        temp_string = msg.substring(7, 8);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Bit 6 : ");
            Serial.println(temp_string);
        #endif
        if      ( temp_string != "0" )  ++error;

        // -> Bit 7
        temp_string = msg.substring(8, 9);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Bit 7 : ");
            Serial.println(temp_string);
        #endif
        if      ( temp_string != "0" )  ++error;

        // -> Bit 8
        temp_string = msg.substring(9, 10);
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.print("Bit 8 : ");
            Serial.println(temp_string);
        #endif
        if      ( temp_string != "0" )  ++error;

        if ( error != 0 ) log_write("Rauchmelder-Übertragungsfehler (" + String(error) + "): " + msg );
    }
}
