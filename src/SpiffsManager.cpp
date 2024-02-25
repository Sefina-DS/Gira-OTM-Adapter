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

void log_write(String msg) {
    String temp = wifi.ntp_date + " ; " + timeClient->getFormattedTime() + " ;; " + msg + '\n';
    String templine;
    #ifdef DEBUG_SERIAL_SPIFFS
        Serial.print("LOGG-FILE-WRITE : ");
        Serial.println(temp);
    #endif

    // lesen der log datei
    if (SPIFFS.exists(LOG_FILE_PATH)) {
        File loggfile = SPIFFS.open( LOG_FILE_PATH, "r");
        int line = 0;
        while ( loggfile.available() && line < 19 ) {
            templine = loggfile.readStringUntil('\n'); // lesen bis Zeilenumbruch..
            templine += '\n';
            temp += templine;
            line ++;
        }
        loggfile.close();
        loggfile = SPIFFS.open( LOG_FILE_PATH, "w");
        loggfile.print(temp);
        loggfile.close();
    } else {
        #ifdef DEBUG_SERIAL_SPIFFS
            Serial.print("Loggdatei ist nicht vorhanden und muss erstellt werden");
        #endif
        File loggfile = SPIFFS.open( LOG_FILE_PATH, "w");
        loggfile.print(temp);
        loggfile.close();
    }
}

String web_request_spiff(const String &var)
{
    String temp = "";
    if          (var == "spiff_info_files")     { return web_spiffs_analyse();
    } else if   (var == "spiff_info_free")      { return humanReadableSize((SPIFFS.totalBytes() - SPIFFS.usedBytes())); 
    } else if   (var == "spiff_info_use")       { return humanReadableSize(SPIFFS.usedBytes());
    } else if   (var == "spiff_info_size")      { return humanReadableSize(SPIFFS.totalBytes());
    } else if   (var == "display_loggfile")      { 
                        if ( !SPIFFS.exists(LOG_FILE_PATH) ) return String();
                        String tempContent;
                        File file = SPIFFS.open(LOG_FILE_PATH, "r");
                        while (file.available()) {
                            tempContent += "<tr><td>";
                            tempContent += file.readStringUntil('\n');
                            tempContent += "</tr></td>";
                        }
                        file.close();
                        return tempContent;
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
        size_t fileSize = file.size();
        float fileSizeKB = (float)fileSize / 1024.0;
        msg += "<tr><td>";
        msg += file.name();
        msg += "</td><td>";
        msg += file.path();
        msg += "</td><td>";
        msg += fileSizeKB;
        msg += " KB</td>";
        if (!webserver.notbetrieb) {
            msg += "<td>";
            msg += "<form action='/System/download' method='GET'><input type='hidden' name='filepath' value='";
            msg += file.path();
            msg += "'><input type='submit' value='Download'></form>";
            msg += "</td><td>";
            msg += "<form action='/System' method='GET'><input type='submit' name='";
            msg += file.path();
            msg += "' value='Löschen'></form>";
            msg += "</td>";
        }
        msg += "</tr>";
        temp += msg;

        file = root.openNextFile();
    }
    return temp;
}

void web_response_spiff(String name, String value){
    #ifdef DEBUG_SERIAL_SPIFFS
        Serial.print("Spiff - Response - Get // name : ");
        Serial.print(name);
        Serial.print(" || value : ");
        Serial.print(value);
    #endif
    if          ( value == "Löschen") {
        if (name != "" ) SPIFFS.remove(name);
    } else if   ( value == "Download") {

    }
}

String humanReadableSize(const size_t bytes)
{
  if (bytes < 1024)
    return String(bytes) + " B";
  else if (bytes < (1024 * 1024))
    return String(bytes / 1024.0) + " KB";
  else if (bytes < (1024 * 1024 * 1024))
    return String(bytes / 1024.0 / 1024.0) + " MB";
  else
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}