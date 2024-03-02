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
    doc["detector_alarm_group"] = detector.groups_safestring;
    doc["detector_location"] = detector.location;

    return doc;
}

void diagnose_groups(String group_string) {
    std::string input = group_string.c_str();
    groups.push_back(0);                // Füge die Null am Anfang hinzu
    groups.push_back(detector.group);   // Füge die eigene Meldegruppe hinzu

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
        temp += ";";
    }
    detector.groups_safestring = temp;
    Serial.print("Auf folgenden Gruppen wird gehört : ");
    Serial.println(temp);
}
/*
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
*/
String web_request_detector(const String &var) {
    if          (var == "button_detector")              { return (comserial.aktiv)  ? "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'>deaktiviert</option>"
                                                                                    : "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'>aktiviert</option>";
    } else if   (var == "textarea_detector_location")   { return detector.location;
    } else if   (var == "textarea_detector_grp")        { return String(detector.group);
    } else if   (var == "textarea_detector_alarmgrp")   { return detector.groups_safestring;
    }

    return String();
}

void web_response_detector(String name, String msg)
{
    if (name == "detector_komunikation"   && msg == "aktiviert")        comserial.aktiv = true;
    if (name == "detector_komunikation"   && msg == "deaktiviert")      comserial.aktiv = false;
    if (name == "detector_location" && msg != "")                       detector.location = msg;
    if (name == "detector_group" && msg != "")                          detector.group = msg.toInt();
    if (name == "detector_alarm_group" && msg != "")                    diagnose_groups(msg);
    if (name == "mqtt_einrichten" && msg == "MQTT Topics erstellen") {
        for (int group : groups) {
            mqtt_publish_group(group, "Melder_Finden", "false");
            mqtt_publish_group(group, "Alarm", "false");
            mqtt_publish_group(group, "Test-Alarm", "false");
        }
    }
}

void mqtt_subscribe_detector() {
    mqtt_publish("Detector-Steuerung/Melder_Finden","false");
    mqtt_subscribe("Detector-Steuerung/Melder_Finden");
    mqtt_publish("Detector-Steuerung/Alarm","false");
    mqtt_subscribe("Detector-Steuerung/Alarm");
    mqtt_publish("Detector-Steuerung/Test-Alarm","false");
    mqtt_subscribe("Detector-Steuerung/Test-Alarm");
    mqtt_publish("Detector-Steuerung/Serial_Send","frei");
    mqtt_subscribe("Detector-Steuerung/Serial_Send");
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
  for (int group : groups){
    if ( topic == mqtt.topic_base + "/0-Gruppen-Steuerung-0/" + group + "/Melder_Finden"    && msg == "true" )  serial_transceive( "070020" );
    if ( topic == mqtt.topic_base + "/0-Gruppen-Steuerung-0/" + group + "/Melder_Finden"    && msg == "false" ) serial_transceive( "070040" );
    if ( topic == mqtt.topic_base + "/0-Gruppen-Steuerung-0/" + group + "/Alarm"            && msg == "true" )  {
                                if ( detector.timer <= millis() ) {
                                    serial_transceive( "030210" );
                                    detector.remote = true; 
                                    detector.remote_grp = group; } }
    if ( topic == mqtt.topic_base + "/0-Gruppen-Steuerung-0/" + group + "/Alarm"            && msg == "false" ) {
                                detector.timer = millis() + 60000;
                                serial_transceive( "030200" );
                                detector.remote = false;
                                detector.remote_grp = -1; }
    if ( topic == mqtt.topic_base + "/0-Gruppen-Steuerung-0/" + group + "/Test-Alarm"       && msg == "true" )  serial_transceive( "030080" );
    if ( topic == mqtt.topic_base + "/0-Gruppen-Steuerung-0/" + group + "/Test-Alarm"       && msg == "false" ) serial_transceive( "030000" );
  }
  
  
  
  
}
