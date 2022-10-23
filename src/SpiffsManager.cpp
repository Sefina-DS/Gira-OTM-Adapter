#include "SysHeaders.h"

// Spiffs Starten
void spiffs_starten()
{
    Serial.println(F("Starten von dem Filesystem... "));
    if (SPIFFS.begin())
    {
        Serial.println(F("SPIFFS wurde erfolgreich gemountet ..."));
        spiffs_config_load_part_a();
        spiffs_config_load_part_b();
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

void spiffs_config_save_part_a()
{
    SPIFFS.remove(safefilea);
    File fileTemp = SPIFFS.open(safefilea, FILE_WRITE);
    if (!fileTemp)
    {
        Serial.println("config_a.json konnte nicht erstellt werden !");
        return;
    }
    StaticJsonDocument<1024> doc;
    String msg_temp = "";
    

    // Funktionen - SAFE

    doc = safe_conf_wifi(doc);
    doc = safe_conf_mqtt(doc);
    doc = safe_conf_sensor(doc);
    doc = safe_conf_detector(doc);
    
    // Variablen werden gelesen
    
    doc["seriel"] = config.seriel;

    

    if (serializeJson(doc, fileTemp) == 0)
    {
        Serial.println("Speichern der config_a.json Fehlgeschlagen !");
    }
    else
    {
        Serial.println("Speichern der config_a.json Erfolgreich !");
    }
    Serial.println();
    fileTemp.close();
}
void spiffs_config_save_part_b()
{
    SPIFFS.remove(safefileb);
    File fileTemp = SPIFFS.open(safefileb, FILE_WRITE);
    if (!fileTemp)
    {
        Serial.println("config_b.json konnte nicht erstellt werden !");
        return;
    }
    StaticJsonDocument<1024> doc;
    String msg_temp = "";
    

    // Funktionen - SAFE

    doc = safe_conf_bluetooth(doc);

    

    if (serializeJson(doc, fileTemp) == 0)
    {
        Serial.println("Speichern der config_b.json Fehlgeschlagen !");
    }
    else
    {
        Serial.println("Speichern der config_b.json Erfolgreich !");
    }
    Serial.println();
    fileTemp.close();
    SPIFFS.remove(safefile_alt);
}

void spiffs_config_load_part_a()
{
    Serial.println("Config A - wird in Variablen geschrieben ...");
    
    File fileTemp = SPIFFS.open(safefilea);

    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, fileTemp);

    String msg_temp = "";

    if (error)
        Serial.println("Config A - Lesefehler || Standart wird genutzt !");

    // Funktionen - LOAD
    
    load_conf_sensor(doc);
    load_conf_detector(doc);
    load_conf_wifi(doc);
    load_conf_mqtt(doc);
    
    config.seriel = doc["seriel"] | false;

    

    fileTemp.close();
}
void spiffs_config_load_part_b()
{
    Serial.println("Config B - wird in Variablen geschrieben ...");
    
    File fileTemp = SPIFFS.open(safefileb);

    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, fileTemp);

    String msg_temp = "";

    if (error)
        Serial.println("Config B- Lesefehler || Standart wird genutzt !");

    // Funktionen - LOAD
    
    load_conf_bluetooth(doc);

    

    fileTemp.close();
}

void spiffs_config_read_part_a()
{ // lesen der config.json
    Serial.println("Anzeige der config A - Datei !");
    File fileTemp = SPIFFS.open(safefilea);
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
void spiffs_config_read_part_b()
{ // lesen der config.json
    Serial.println("Anzeige der config B - Datei !");
    File fileTemp = SPIFFS.open(safefileb);
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

String webserver_call_spiffs(const String &var)
{
    String temp = "";
    if (var == "FILELIST")
    {
        return listFiles(true);
    }
    if (var == "FREESPIFFS")
    {
        return humanReadableSize((SPIFFS.totalBytes() - SPIFFS.usedBytes()));
    }
    if (var == "USEDSPIFFS")
    {
        return humanReadableSize(SPIFFS.usedBytes());
    }
    if (var == "TOTALSPIFFS")
    {
        return humanReadableSize(SPIFFS.totalBytes());
    }
  
    return String();
}

void webserver_triger_spiffs(String name, String msg)
{

}