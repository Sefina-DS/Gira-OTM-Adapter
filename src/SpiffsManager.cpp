#include "SysHeaders.h"

// Spiffs Starten
void spiffs_starten()
{
    #ifdef DEBUG_SERIAL_SPIFFS
        Serial.println();
        Serial.println(F("Starten von dem Filesystem... "));
    #endif
    if (SPIFFS.begin())
    {
        #ifdef DEBUG_SERIAL_SPIFFS
            Serial.println(F("SPIFFS wurde erfolgreich gemountet ..."));
        #endif
    }
    else
    {
        #ifdef DEBUG_SERIAL_SPIFFS
            Serial.println(F("Fehler beim Mounten von SPIFFS !"));
        #endif
        spiffs_format();
    }
}

// Spiffs Formatieren
void spiffs_format()
{
    #ifdef DEBUG_SERIAL_SPIFFS
        Serial.println("");
        Serial.println(F("Formatieren wird durchgeführt... "));
    #endif
    bool formatted = SPIFFS.format();
    if (formatted)
    {
        #ifdef DEBUG_SERIAL_SPIFFS
            Serial.println("\n\nSuccess formatting");
        #endif
        spiffs_starten();
    }
    else
    {
        #ifdef DEBUG_SERIAL_SPIFFS
            Serial.println("\n\nError formatting");
        #endif
        spiffs_format();
    }
}

void spiffs_scan()
{
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    #ifdef DEBUG_SERIAL_SPIFFS
        Serial.println("Spiffs wird nach Datein durchsucht :");
    #endif

    while (file)
    {
        #ifdef DEBUG_SERIAL_SPIFFS
            Serial.print("FILE: ");
            Serial.print(file.name());
            Serial.print(" // ");
            Serial.println(file.size());
        #endif
        file = root.openNextFile();
    }
    #ifdef DEBUG_SERIAL_SPIFFS
        Serial.println("");
    #endif
}

void spiffs_config_save()
{
    SPIFFS.remove(safefile);
    File fileTemp = SPIFFS.open(safefile, FILE_WRITE);
    if (!fileTemp)
    {
        #ifdef DEBUG_SERIAL_SPIFFS
            Serial.println("config.json konnte nicht erstellt werden !");
        #endif
        return;
    }
    StaticJsonDocument<1024> doc;
    String msg_temp = "";

    // Funktionen - SAFE

    doc = safe_conf_sys(doc);
    doc = safe_conf_wifi(doc);
    doc = safe_conf_mqtt(doc);
    doc = safe_conf_sensor(doc);
    doc = safe_conf_detector(doc);
    doc = safe_conf_serial(doc);

    // Variablen werden gelesen

    //doc["seriel"] = config.seriel;

    #ifdef DEBUG_SERIAL_SPIFFS
        if (serializeJson(doc, fileTemp) == 0) {
            Serial.println("Speichern der config.json Fehlgeschlagen !");
        } else {
            Serial.println("Speichern der config.json Erfolgreich !");
        }
        Serial.println();
    #endif
    fileTemp.close();
}

void spiffs_config_load()
{
    #ifdef DEBUG_SERIAL_SPIFFS
        Serial.println("Config - wird in Variablen geschrieben ...");
    #endif
    File fileTemp = SPIFFS.open(safefile);

    StaticJsonDocument<4096> doc;
    DeserializationError error = deserializeJson(doc, fileTemp);

    String msg_temp = "";

    #ifdef DEBUG_SERIAL_SPIFFS
        if (error)  Serial.println("Config - Lesefehler || Standart wird genutzt !");
    #endif
    // Funktionen - LOAD
    
    load_conf_sys(doc);
    load_conf_sensor(doc);
    load_conf_detector(doc);
    load_conf_wifi(doc);
    load_conf_mqtt(doc);
    load_conf_serial(doc);
    
    fileTemp.close();
}

void spiffs_config_read()
{ // lesen der config.json
    #ifdef DEBUG_SERIAL_SPIFFS
        Serial.println("Anzeige der config - Datei !");
    #endif
    File fileTemp = SPIFFS.open(safefile);
    if (!fileTemp) {
        #ifdef DEBUG_SERIAL_SPIFFS
            Serial.println("die config.json konnte nicht gelesen werden");
        #endif
        return;
    }
    #ifdef DEBUG_SERIAL_SPIFFS
        while (fileTemp.available()) {
            Serial.print((char)fileTemp.read());
        }
        Serial.println();
    #endif
    fileTemp.close();
}

void log_write(String msg){
    String temp = wifi.ntp_date + ";" + timeClient->getFormattedTime() + " ;; " + msg;
    #ifdef DEBUG_SERIAL_SPIFFS
        Serial.print( "LOGG-FILE-WRITE : ");
        Serial.println(temp);
    #endif

    File file = SPIFFS.open(LOG_FILE_PATH, "r+");
    if (!file) {
        file = SPIFFS.open(LOG_FILE_PATH, "w+");
    }

    if (file) {
        int lineCount = 0;
        while (file.available()) {
            if (file.read() == '\n') {
                lineCount++;
            }
        }

        while (lineCount >= MAX_LINES) {
            file.seek(0);
            while (file.available()) {
                char c = file.read();
                if (c == '\n') {
                    break;
                }
            }
            lineCount--;
        }

        file.seek(0);

        String existingContent = file.readString();
        
        file.seek(0);
        file.print(temp);
        file.print('\n');
        file.print(existingContent);

        file.close();
        #ifdef DEBUG_SERIAL_SPIFFS
            Serial.println("Logeintragung erfolgreich");
        #endif
    } else {
        #ifdef DEBUG_SERIAL_SPIFFS
            Serial.println("Fehler beim Öffnen der Log-Datei!");
        #endif
    }
}

String web_request_spiff(const String &var)
{
    String temp = "";
    if          (var == "spiff_info_files")     { return web_spiffs_analyse();
    } else if   (var == "spiff_info_free")      { return humanReadableSize((SPIFFS.totalBytes() - SPIFFS.usedBytes())); 
    } else if   (var == "spiff_info_use")       { return humanReadableSize(SPIFFS.usedBytes());
    } else if   (var == "spiff_info_size")      { return humanReadableSize(SPIFFS.totalBytes());
    }
    return String();
}

String web_spiffs_analyse(){
    String temp = "";
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    #ifdef DEBUG_SERIAL_SPIFFS
        Serial.println("Spiffs wird nach Datein durchsucht :");
    #endif

    while (file)
    {
        String msg = "";
        msg += "<tr><td>";
        msg += file.name();
        msg += "</td><td>";
        msg += file.path();
        msg += "</td><td>";
        msg += file.size();
        msg += "</td></tr>";
        temp += msg;
        
        #ifdef DEBUG_SERIAL_SPIFFS
            Serial.print("FILE: ");
            Serial.print(file.name());
            Serial.print(" // ");
            Serial.print(file.path());
            Serial.print(" // ");
            Serial.println(file.size());
        #endif
        file = root.openNextFile();
    }
    #ifdef DEBUG_SERIAL_SPIFFS
        Serial.println("");
    #endif
    Serial.println(temp);
    return temp;
}
/*
File root = SPIFFS.open("/");
    File file = root.openNextFile();
    #ifdef DEBUG_SERIAL_SPIFFS
        Serial.println("Spiffs wird nach Datein durchsucht :");
    #endif

    while (file)
    {
        #ifdef DEBUG_SERIAL_SPIFFS
            Serial.print("FILE: ");
            Serial.print(file.name());
            Serial.print(" // ");
            Serial.println(file.size());
        #endif
        file = root.openNextFile();
    }
    #ifdef DEBUG_SERIAL_SPIFFS
        Serial.println("");
    #endif
*/
void web_response_spiff(String name, String value){

}
