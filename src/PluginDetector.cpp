#include "SysHeaders.h"

DETECTOR detector;

std::vector<int> groups;

void load_conf_detector(StaticJsonDocument<1024> doc)
{
    #ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("... Detector- Variablen ...");
    #endif
    String temp;
    
    detector.group = doc["detector_group"] | 0;
    temp = doc["detector_alarm_group"] | "0";
    detector.location = doc["detector_location"] | "";

    diagnose_groups(temp);
}

StaticJsonDocument<1024> safe_conf_detector(StaticJsonDocument<1024> doc)
{
    #ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("... Detector- Variablen ...");
    #endif
    doc["detector_group"] = detector.group;
    doc["detector_alarm_group"] = detector.alarm_group_safe;
    doc["detector_location"] = detector.location;

    return doc;
}

void diagnose_groups(String group_string) {
    std::string input = group_string.c_str();
    groups.push_back(0); // Füge die Null am Anfang hinzu

    std::string currentNumber;

    for (char c : input) {
        if (std::isdigit(c)) {
            currentNumber += c;
        } else if (!currentNumber.empty()) {
            groups.push_back(std::stoi(currentNumber));
            currentNumber.clear();
        }
    }
    if (!currentNumber.empty()) {
        groups.push_back(std::stoi(currentNumber));
    }
    // Sortieren der Zahlen
    std::sort(groups.begin(), groups.end());

    // Entfernen von Duplikaten
    std::set<int> uniqueNumbers(groups.begin(), groups.end());
    groups.assign(uniqueNumbers.begin(), uniqueNumbers.end());

    String temp = "";
    for (int group : groups) {
        temp += group;
        temp += " ";
    }
    Serial.print("Auf folgenden Gruppen wird gehört : ");
    Serial.println(temp);
}

void alarm_group_diagnose(String msg)
{
    String temp_string;
    String temp_safe = "";
    boolean temp_grp = false;
    boolean temp_0 = false;
    int temp_size = 0;
    for (int i = 0; msg[i] != 0 ; i++)
    {
        if (msg[i] > 47 &&
            msg[i] < 58)
        {
            temp_string += msg[i];
        }
        else 
        {
            if ( temp_string != "")
            {
                detector.alarm_group[temp_size] = temp_string.toInt();
                if ( 0 == temp_string.toInt())              temp_0 = true;
                if ( detector.group == temp_string.toInt()) temp_grp = true;
                temp_size++;
                temp_string = "";
            }
        }
    }
    if ( temp_string != "")
    {
        detector.alarm_group[temp_size] = temp_string.toInt();
        temp_size++;
        temp_string = "";
    }
    if (temp_0 == false)
    {
        detector.alarm_group[temp_size] = 0;
        temp_size++;
        if (detector.group == 0) temp_grp = true ;
    }
    if (temp_grp == false)
    {
        detector.alarm_group[temp_size] = detector.group;
        temp_size++;
    }
    detector.alarm_group_size = temp_size;
    for (int i = 0; i < temp_size; i++)
    {
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.print("Gruppe ");
            Serial.print(i);
            Serial.print(" : ");
            Serial.println(detector.alarm_group[i]);
        #endif
        temp_safe += detector.alarm_group[i];
        temp_safe += " ";
    }
    detector.alarm_group_safe = temp_safe;
}

String web_request_detector(const String &var) {
    if          ( var == "ph_detector_aktiv")       { return ( comserial.aktiv )    ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                    : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_detector_location")    { return  detector.location;
    } else if   ( var == "ph_detector_grp")         { return  String(detector.group);
    } else if   ( var == "ph_detector_alarmgrp")    { return  detector.alarm_group_safe;
    }

    return String();
}

void webserver_triger_detector(String name, String msg)
{
    if (name == "detector_komunikation"   && msg == "aktiviert")      comserial.aktiv = true;
    if (name == "detector_komunikation"   && msg == "deaktiviert")    comserial.aktiv = false;
    if (name == "detector_location" && msg != "")       detector.location = msg;
    if (name == "detector_group" && msg != "")          detector.group = msg.toInt();
    if (name == "detector_alarm_group" && msg != "")    alarm_group_diagnose(msg);
}
/*
void mqtt_detector_sub_register()
{
    String local_topic_detector = mqtt.topic_base + "/" + mqtt.topic_define + "/Melder-Steuern/" ;
    String local_topic_group = mqtt.topic_base + "/-Gruppen-Steuerung-/";

    client.subscribe((local_topic_detector + "Seriele_Nachricht").c_str());
    //mqtt_publish(local_topic_detector + "Seriele_Nachricht", "", "mqtt_detector_sub_register");
  
    client.subscribe((local_topic_detector + "Melder_Finden").c_str());
    client.subscribe((local_topic_detector + "Reset_Test|Funk_Alarme").c_str());
    client.subscribe((local_topic_detector + "Testalarm_Funk").c_str());
    client.subscribe((local_topic_detector + "Alarm_Funk").c_str());
    
    client.subscribe((local_topic_group + detector.group + "/" + "Melder_Finden").c_str());
    
    for (int i = 0; i < detector.alarm_group_size; i++)
    {
        client.subscribe((local_topic_group + detector.alarm_group[i] + "/" + "Testalarm_Funk").c_str());
        client.subscribe((local_topic_group + detector.alarm_group[i] + "/" + "Alarm_Funk").c_str());
    }    
}

void mqtt_detector_sub_read(String topic, String msg)
{
    String local_topic_detector = mqtt.topic_base + "/" + mqtt.topic_define + "/Melder-Steuern/" ;
    String local_topic_group = mqtt.topic_base + "/-Gruppen-Steuerung-/";
    String temp_topic = "";
    boolean msg_bool;

    if (msg == "false") msg_bool = false;
    if (msg == "true")  msg_bool = true;

    //      MELDER FINDEN
    if (topic == local_topic_detector +  "Melder_Finden" ||
        topic == local_topic_group + detector.group + "/" + "Melder_Finden")
    {
        if (topic == local_topic_detector +  "Melder_Finden" && msg == "")                          mqtt_publish(local_topic_detector + "Melder_Finden", "false", "mqtt_detector_sub_read");
        if (topic == local_topic_group + detector.group + "/" + "Melder_Finden" && msg == "")       mqtt_publish(local_topic_group + detector.group + "/" + "Melder_Finden", "false", "mqtt_detector_sub_read");
        
        if (comserial.com_status == 0)
        {
            if (msg_bool == true)
            {
                //serial_send("070020", 1); // Melder- Finden An
            }
            else
            {
                //serial_send("070040", 1); // Melder- Finden Aus
            }
        }
    }

    //      TESTALARM ÜBER ESP
    if (topic == local_topic_detector + "Testalarm_Funk")
    {
        if (topic == local_topic_detector +  "Testalarm_Funk" && msg == "")                          mqtt_publish(local_topic_detector + "Testalarm_Funk", "false", "mqtt_detector_sub_read");
        if (comserial.com_status == 0)
        {
            if (msg_bool == true)
            {
                //serial_send("030080", 1); // Test- Alarm
            }
            else
            {
                //serial_send("030200", 1); // Reset Funk-Alarm + Test-Alarm
            }
        }
    }

    //      RESET TEST / UND FUNK _ ALARME
    if (topic == local_topic_detector + "Reset_Test|Funk_Alarme")
    {
        if (topic == local_topic_detector +  "Reset_Test|Funk_Alarme" && msg == "")                          mqtt_publish(local_topic_detector + "Reset_Test|Funk_Alarme", "false", "mqtt_detector_sub_read");
        if (comserial.com_status == 0)
        {
            if (msg_bool == true)
            {
                //serial_send("030000", 1); // Test- Alarm- Reset
                mqtt_publish(local_topic_detector + "Reset_Test|Funk_Alarme", "false", "mqtt_detector_sub_read");
            }
        }
        else
        {
            mqtt_publish(local_topic_detector + "Reset_Test|Funk_Alarme", "false", "mqtt_detector_sub_read");
        }
    }

    //      SERIELE TESTNACHRICHT
    if (topic == local_topic_detector + "Seriele_Nachricht")
    {
        if (comserial.com_status == 0 && msg != "" )
        {
            //serial_send(msg, 1); // Testnachricht (Als String)
            //mqtt_publish(local_topic_detector + "Seriele_Nachricht", "", "mqtt_detector_sub_read");
        }
        else
        {
            //mqtt_publish(local_topic_detector + "Seriele_Nachricht", "", "mqtt_detector_sub_read");
        }
    }

    //      FUNK- ALARM
    if (topic == local_topic_detector + "Alarm_Funk")
    {
        if (topic == local_topic_detector +  "Alarm_Funk" && msg == "")                          mqtt_publish(local_topic_detector + "Alarm_Funk", "false", "mqtt_detector_sub_read");
        if (comserial.com_status == 0)
        {
            if (msg_bool == true)
            {
                //serial_send("030210", 1); // Alarm Funk
            }
        }
    }

    //      ALARMIERUNGSGRUPPEN
    for (int i = 0; i < detector.alarm_group_size; i++)
    {
        // Feueralarm
        if (topic == local_topic_group + detector.alarm_group[i] + "/Alarm_Funk")
        {
            if (topic == local_topic_group + detector.alarm_group[i] + "/Alarm_Funk" && msg == "")  mqtt_publish(local_topic_group + detector.alarm_group[i] + "/Alarm_Funk", "false", "mqtt_detector_sub_read");
            if (comserial.com_status == 0)
            {
                if (msg_bool == true)
                {
                    if (comserial.timer_alarm < millis())
                    {
                        comserial.timer_alarm = millis() + 300000;
                        //serial_send("030210", 1); // Alarm Funk
                    }
                }
                else
                {
                    //serial_send("030200", 1); // Reset Funk-Alarm + Test-Alarm
                }
            }
        }
        // Testalarm
        if (topic == local_topic_group + detector.alarm_group[i] + "/Testalarm_Funk")
        {
            if (topic == local_topic_group + detector.alarm_group[i] + "/Testalarm_Funk" && msg == "")  mqtt_publish(local_topic_group + detector.alarm_group[i] + "/Testalarm_Funk", "false", "mqtt_detector_sub_read");
            if (comserial.com_status == 0)
            {
                if (msg_bool == true)
                {
                    //serial_send("030080", 1); // Test- Alarm
                }
                else
                {
                    //serial_send("030200", 1); // Reset Funk-Alarm + Test-Alarm
                }
            }
        }
    }
    
}
*/
void mqtt_subscribe_detector() {
    mqtt_publish("Detector-Steuerung/Melder_Finden","false");
    mqtt_subscribe("Detector-Steuerung/Melder_Finden");
    mqtt_publish("Detector-Steuerung/Alarm","false");
    mqtt_subscribe("Detector-Steuerung/Alarm");
    mqtt_publish("Detector-Steuerung/Test-Alarm","false");
    mqtt_subscribe("Detector-Steuerung/Test-Alarm");
    mqtt_publish("Detector-Steuerung/Serial_Send","frei");
    mqtt_subscribe("Detector-Steuerung/Serial_Send");
    mqtt_publish("Detector-Steuerung/TestString","frei");
    mqtt_subscribe("Detector-Steuerung/TestString");
    for (int group : groups) {
        mqtt_subscribe_group(group, "Melder_Finden");
        mqtt_subscribe_group(group, "Alarm");
        mqtt_subscribe_group(group, "Test-Alarm");
    }
}

void mqtt_incoming_msg_detector(String topic, String msg){
  if ( topic == mqtt.topic + "Detector-Steuerung/Melder_Finden"     && msg == "true")      serial_transceive( "070020" );
  if ( topic == mqtt.topic + "Detector-Steuerung/Melder_Finden"     && msg == "false")     serial_transceive( "070040" );
  if ( topic == mqtt.topic + "Detector-Steuerung/Alarm"             && msg == "true")      serial_transceive( "030210" );
  if ( topic == mqtt.topic + "Detector-Steuerung/Alarm"             && msg == "false")     serial_transceive( "030200" );
  if ( topic == mqtt.topic + "Detector-Steuerung/Test-Alarm"        && msg == "true")      serial_transceive( "030080" );
  if ( topic == mqtt.topic + "Detector-Steuerung/Test-Alarm"        && msg == "false")     serial_transceive( "030000" );
  if ( topic == mqtt.topic + "Detector-Steuerung/Serial_Send"       && msg != "frei" ) {          
        serial_transceive( msg );
        mqtt_publish("Detector-Steuerung/Serial_Send", "frei");
  }
  if ( topic == mqtt.topic + "Detector-Steuerung/TestString"       && msg != "frei" ) {          
        diagnose_groups( msg );
        mqtt_publish("Detector-Steuerung/TestString", "frei");
  }
  for (int group : groups){
    if ( topic == mqtt.topic_base + "/0-Gruppen-Steuerung-0/" + group + "/Melder_Finden"    && msg == "true" )  serial_transceive( "070020" );
    if ( topic == mqtt.topic_base + "/0-Gruppen-Steuerung-0/" + group + "/Melder_Finden"    && msg == "false" ) serial_transceive( "070040" );
    if ( topic == mqtt.topic_base + "/0-Gruppen-Steuerung-0/" + group + "/Alarm"            && msg == "true" )  serial_transceive( "030210" );
    if ( topic == mqtt.topic_base + "/0-Gruppen-Steuerung-0/" + group + "/Alarm"            && msg == "false" ) serial_transceive( "030200" );
    if ( topic == mqtt.topic_base + "/0-Gruppen-Steuerung-0/" + group + "/Test-Alarm"       && msg == "true" )  serial_transceive( "030080" );
    if ( topic == mqtt.topic_base + "/0-Gruppen-Steuerung-0/" + group + "/Test-Alarm"       && msg == "false" ) serial_transceive( "030000" );
  }
  
  
  
  
}
