#include "SysHeaders.h"

// Spiffs Starten
void spiffs_starten()
{
    Serial.println(F("Starten von dem Filesystem... "));
    if (SPIFFS.begin())
    {
        Serial.println(F("SPIFFS wurde erfolgreich gemountet ..."));
        spiffs_config_load();
    }
    else
    {
        Serial.println(F("Fehler beim Mounten von SPIFFS !"));
        spiffs_format();
    }
}
// Spiffs Formatieren
void spiffs_format()
{
    Serial.println("");
    Serial.println(F("Formatieren wird durchgeführt... "));
    bool formatted = SPIFFS.format();
    if (formatted)
    {
        Serial.println("\n\nSuccess formatting");
        spiffs_starten();
    }
    else
    {
        Serial.println("\n\nError formatting");
        spiffs_format();
    }
}

void spiffs_scan()
{
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    Serial.println("Spiffs wird nach Datein durchsucht :");

    while (file)
    {

        Serial.print("FILE: ");
        Serial.println(file.name());

        file = root.openNextFile();
    }
    Serial.println("");
}

void spiffs_config_save()
{
    SPIFFS.remove(safefile);
    File fileTemp = SPIFFS.open(safefile, FILE_WRITE);
    if (!fileTemp)
    {
        Serial.println("config.json konnte nicht erstellt werden !");
        return;
    }
    StaticJsonDocument<1024> doc;
    String msg_temp = "";
    for (int i = 0; i < config.detector_alarm_group_size; i++)
    {
        msg_temp += config.detector_alarm_group_int[i];
        msg_temp += ";";
    }
    msg_temp += config.detector_alarm_group_int[config.detector_alarm_group_size];

    // Funktionen - SAFE

    doc = safe_conf_wifi(doc);
    doc = safe_conf_mqtt(doc);
    doc = safe_conf_sensor(doc);
    doc = safe_conf_bluetooth(doc);
    
    // Variablen werden gelesen
    /*
    doc["esp_name"] = config.esp_name;
    doc["wifi_ssid"] = config.wifi_ssid;
    doc["wifi_pw"] = config.wifi_pw;
    doc["wifi_dhcp"] = config.wifi_dhcp;
    doc["wifi_ip"] = config.wifi_ip;
    doc["wifi_gw"] = config.wifi_gw;
    doc["wifi_subnet"] = config.wifi_subnet;
    doc["wifi_dns"] = config.wifi_dns;
    */
    /*
    doc["mqtt"] = config.mqtt;
    doc["mqtt_ip"] = config.mqtt_ip;
    doc["mqtt_port"] = config.mqtt_port;
    doc["mqtt_topic_base"] = config.mqtt_topic_base;
    doc["mqtt_topic_define"] = config.mqtt_topic_define;
    */
    doc["seriel"] = config.seriel;
    doc["detector_group"] = config.detector_group;
    doc["detector_alarm_group"] = msg_temp;
    doc["detector_location"] = config.detector_location;
    

    if (serializeJson(doc, fileTemp) == 0)
    {
        Serial.println("Speichern der config.json Fehlgeschlagen !");
    }
    else
    {
        Serial.println("Speichern der config.json Erfolgreich !");
    }
    Serial.println();
    fileTemp.close();
}

void spiffs_config_load()
{
    Serial.println("Config- wird in Variablen geschrieben ...");
    
    File fileTemp = SPIFFS.open(safefile);

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, fileTemp);

    String msg_temp = "";

    if (error)
        Serial.println("Config- Lesefehler || Standart wird genutzt !");

    // Funktionen - LOAD
    
    load_conf_sensor(doc);
    load_conf_bluetooth(doc);
    load_conf_wifi(doc);
    load_conf_mqtt(doc);
    
    // Variablen werden beschrieben
    /*
    config.esp_name = doc["esp_name"] | "";
    config.wifi_ssid = doc["wifi_ssid"] | "-";
    config.wifi_pw = doc["wifi_pw"] | "";
    config.wifi_dhcp = doc["wifi_dhcp"] | true;
    config.wifi_ip = doc["wifi_ip"] | "0.0.0.0";
    config.wifi_gw = doc["wifi_gw"] | "0.0.0.0";
    config.wifi_subnet = doc["wifi_subnet"] | "0.0.0.0";
    config.wifi_dns = doc["wifi_dns"] | "0.0.0.0";
    */
    /*
    config.mqtt = doc["mqtt"] | false;
    config.mqtt_ip = doc["mqtt_ip"] | "x-x-x-x";
    config.mqtt_port = doc["mqtt_port"] | "1883";
    config.mqtt_topic_base = doc["mqtt_topic_base"] | "Rauchmelder";
    config.mqtt_topic_define = doc["mqtt_topic_define"] | "";
    */
    config.seriel = doc["seriel"] | false;
    config.detector_group = doc["detector_group"] | "0";
    msg_temp = doc["detector_alarm_group"] | "0";
    alarm_group_diagnose(msg_temp);
    config.detector_location = doc["detector_location"] | "";
    

    fileTemp.close();
}

void spiffs_config_read()
{ // lesen der config.json
    Serial.println("Anzeige der config - Datei !");
    File fileTemp = SPIFFS.open(safefile);
    if (!fileTemp)
    {
        Serial.println("die config.json konnte nicht gelesen werden");
        return;
    }
    while (fileTemp.available())
    {
        Serial.print((char)fileTemp.read());
    }
    Serial.println();
    fileTemp.close();
}
