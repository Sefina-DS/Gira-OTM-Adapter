#include "SysHeaders.h"

SYSTEM_FUNKTION system_funktion;

void system_timer()
{
    system_funktion.timer = millis() + 10000 ;

    if (!WiFi.isConnected() && !AP_Mode ) wlan_connect();
        
    led_flash_timer(100 ,0 ,1);
}

void version_check()
{
    if (WiFi.isConnected() == true )
    {
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.println("Version- Update wird geprüfft ... ");
        #endif
        String url = "https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/" + system_funktion.fw_art + "/" + "firmware/version.txt";
        HTTPClient http;
        http.begin( url );
        int httpCode = http.GET();
        if(httpCode > 0) 
        {
            if(httpCode == HTTP_CODE_OK) 
            {
                system_funktion.version_new = http.getString() ;
            }
        } else {
            system_funktion.version_new = system_funktion.version_old ;
        }
        http.end();
        if ( system_funktion.version_old != system_funktion.version_new )
        {
            system_funktion.new_version = true;
        } else {
            system_funktion.new_version = false;
        }
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.print("Frimwareversion von : ");
            Serial.println(url);
            Serial.print("alte Version : ");
            Serial.print(system_funktion.version_old);
            Serial.print(" // neue Version : ");
            Serial.print(system_funktion.version_new);
            Serial.println();
            if ( system_funktion.new_version ) {
                Serial.println("Eine neue Version steht zum download bereit.");
            } else {
                Serial.println("Kein Update vorhanden.");
            }
        #endif
    }
}

bool file_download(String download_file_path, String spiff_file_path) {
    #ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("Fieledownload URL : https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/" + system_funktion.fw_art + download_file_path);
    #endif
  
    // HTTP-Anfrage senden
    http.begin("https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/" + system_funktion.fw_art + download_file_path );
  
    // Überprüfen der Antwort
    int httpCode = http.GET();
    if(httpCode == HTTP_CODE_OK) {
        // Datei öffnen, um im SPIFFS zu speichern
        File file = SPIFFS.open(spiff_file_path, "w");
        if(!file) {
            #ifdef DEBUG_SERIAL_OUTPUT
                Serial.println("File kann nicht erstellt werden");
            #endif
            return false;
        }
    
        // Dateiinhalt von der HTTP-Antwort lesen und im SPIFFS speichern
        Stream* stream = http.getStreamPtr();
        if(stream) {
            while(stream->available()) {
                file.write(stream->read());
            }
        }
    
        // Datei schließen
        file.close();
        return true;
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.println("File " + spiff_file_path + " erfolgreich gedownloadet");
        #endif
    } else {
        return false;
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.printf("File download fehlgeschlagen mit : : %d\n", httpCode);
        #endif
    }
  
    // HTTP-Verbindung schließen
    http.end();
}
/*
void folder_download(const String& download_folder_path, const String& spiffs_folder_path) {
    HTTPClient http;
    
    // HTTP-Anfrage senden, um den Inhalt des Ordners abzurufen
    String folderURL = "https://api.github.com/repos/Sefina-DS/Gira-OTM-Adapter/contents/" + download_folder_path + "?ref=beta";
    Serial.println("Folder URL, Git HUB API : " + folderURL);
    http.begin(folderURL);
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
        // Lesen des Inhalts der Antwort
        String folderContent = http.getString();
        
        // JSON-Daten analysieren
        DynamicJsonDocument doc(2048);
        DeserializationError error = deserializeJson(doc, folderContent);
        if (error) {
            Serial.println("Fehler beim Parsen des JSON-Dokuments");
            return;
        }
        
        // Überprüfen, ob das JSON-Dokument ein Array ist
        if (doc.is<JsonArray>()) {
            // Durchlaufen aller Objekte im Array
            for (JsonObject obj : doc.as<JsonArray>()) {
                // Extrahieren des Dateinamens
                String filename = obj["name"].as<String>();
                
                // Herunterladen der Datei
                String fileURL = obj["download_url"].as<String>();
                HTTPClient fileHTTP;
                fileHTTP.begin(fileURL);
                int fileHTTPCode = fileHTTP.GET();
                if (fileHTTPCode == HTTP_CODE_OK) {
                    // Öffnen und Schreiben der heruntergeladenen Datei im SPIFFS
                    File file = SPIFFS.open("/" + spiffs_folder_path + "/" + filename, "w");
                    if (file) {
                        String fileContent = fileHTTP.getString();
                        file.write((const uint8_t*)fileContent.c_str(), fileContent.length());
                        file.close();
                        #ifdef DEBUG_SERIAL_OUTPUT
                            Serial.println("Datei heruntergeladen: /" + spiffs_folder_path + "/" + filename);
                        #endif
                    } else {
                        #ifdef DEBUG_SERIAL_OUTPUT
                            Serial.println("Fehler beim Öffnen der Datei: /" + spiffs_folder_path + "/" + filename);
                        #endif
                    }
                } else {
                    #ifdef DEBUG_SERIAL_OUTPUT
                        Serial.println("Fehler beim Herunterladen der Datei: " + fileURL);
                    #endif
                }
                fileHTTP.end();
            }
        } else {
            Serial.println("Ungültiges JSON-Array");
        }
    } else {
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.println("Fehler beim Abrufen des Ordnerinhalts: " + folderURL);
        #endif
    }

    http.end();
}
*/
/*
void folder_download(const String& download_folder_path, const String& spiffs_folder_path) {
    HTTPClient http;
    
    // HTTP-Anfrage senden, um den Inhalt des Ordners abzurufen
    String folderURL = "https://api.github.com/repos/Sefina-DS/Gira-OTM-Adapter/contents/" + download_folder_path + "?ref=beta";
    Serial.println("Folder URL, Git HUB API : " + folderURL);
    http.begin(folderURL);
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
        // Prozessieren des Streams, um das JSON-Dokument schrittweise zu parsen
        processJsonStream(http.getStream(), spiffs_folder_path);
    } else {
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.println("Fehler beim Abrufen des Ordnerinhalts: " + folderURL);
        #endif
    }

    http.end();
}
*/
/*
void processJsonStream(WiFiClient& client, const String& spiffs_folder_path) {
    DynamicJsonDocument doc(2048); // Größe des JSON-Dokuments anpassen

    DeserializationError error;

    // JSON-Parsing-Flag
    bool parsingObject = false;
    String currentKey;

    // Buffer für JSON-Ereignisse
    const size_t bufferSize = 2048;
    char buffer[bufferSize];
    size_t bufferIndex = 0;

    while (client.connected() || client.available()) {
        // Daten vom Stream lesen
        while (client.available()) {
            char c = client.read();
            // JSON-Ereignisse im Buffer speichern
            buffer[bufferIndex++] = c;
            // Prüfen auf Objektbeginn
            if (c == '{') {
                parsingObject = true;
            }
            // Prüfen auf Objektende
            if (c == '}' && parsingObject) {
                // Nullterminierung des Buffers für die Deserialisierung
                buffer[bufferIndex] = '\0';
                // Deserialisierung des JSON-Objekts
                error = deserializeJson(doc, buffer);
                if (error) {
                    Serial.print("Fehler beim Parsen des JSON-Dokuments: ");
                    Serial.println(error.c_str());
                } else {
                    // Überprüfen, ob das JSON-Objekt ein Array ist
                    if (doc.is<JsonArray>()) {
                        JsonArray array = doc.as<JsonArray>();
                        // Durchlaufen aller Objekte im Array
                        for (JsonObject obj : array) {
                            String filename = obj["name"].as<String>();
                            String fileURL = obj["download_url"].as<String>();

                            // Herunterladen der Datei
                            HTTPClient fileHTTP;
                            fileHTTP.begin(fileURL);
                            int fileHTTPCode = fileHTTP.GET();
                            if (fileHTTPCode == HTTP_CODE_OK) {
                                // Öffnen und Schreiben der heruntergeladenen Datei im SPIFFS
                                File file = SPIFFS.open("/" + spiffs_folder_path + "/" + filename, "w");
                                if (file) {
                                    String fileContent = fileHTTP.getString();
                                    file.write((const uint8_t*)fileContent.c_str(), fileContent.length());
                                    file.close();
                                    #ifdef DEBUG_SERIAL_OUTPUT
                                        Serial.println("Datei heruntergeladen: /" + spiffs_folder_path + "/" + filename);
                                    #endif
                                } else {
                                    #ifdef DEBUG_SERIAL_OUTPUT
                                        Serial.println("Fehler beim Öffnen der Datei: /" + spiffs_folder_path + "/" + filename);
                                    #endif
                                }
                            } else {
                                #ifdef DEBUG_SERIAL_OUTPUT
                                    Serial.println("Fehler beim Herunterladen der Datei: " + fileURL);
                                #endif
                            }
                            fileHTTP.end();
                        }
                    } else {
                        Serial.println("Ungültiges JSON-Array");
                    }
                }
                // Buffer zurücksetzen
                bufferIndex = 0;
                // Parsing beenden
                parsingObject = false;
            }
        }
    }
}
*/
/*
void processJsonArray(const String& jsonContent, const String& spiffs_folder_path) {
    // JSON-Daten analysieren
    StaticJsonDocument<2048> doc;
    DeserializationError error = deserializeJson(doc, jsonContent);
    if (error) {
        Serial.println("Fehler beim Parsen des JSON-Dokuments");
        return;
    }
    
    // Überprüfen, ob das JSON-Dokument ein Array ist
    if (doc.is<JsonArray>()) {
        // Durchlaufen aller Objekte im Array
        for (JsonObject obj : doc.as<JsonArray>()) {
            // Extrahieren des Dateinamens und der Download-URL
            String filename = obj["name"].as<String>();
            String fileURL = obj["download_url"].as<String>();

            // Herunterladen der Datei
            HTTPClient fileHTTP;
            fileHTTP.begin(fileURL);
            int fileHTTPCode = fileHTTP.GET();
            if (fileHTTPCode == HTTP_CODE_OK) {
                // Öffnen und Schreiben der heruntergeladenen Datei im SPIFFS
                File file = SPIFFS.open("/" + spiffs_folder_path + "/" + filename, "w");
                if (file) {
                    String fileContent = fileHTTP.getString();
                    file.write((const uint8_t*)fileContent.c_str(), fileContent.length());
                    file.close();
                    #ifdef DEBUG_SERIAL_OUTPUT
                        Serial.println("Datei heruntergeladen: /" + spiffs_folder_path + "/" + filename);
                    #endif
                } else {
                    #ifdef DEBUG_SERIAL_OUTPUT
                        Serial.println("Fehler beim Öffnen der Datei: /" + spiffs_folder_path + "/" + filename);
                    #endif
                }
            } else {
                #ifdef DEBUG_SERIAL_OUTPUT
                    Serial.println("Fehler beim Herunterladen der Datei: " + fileURL);
                #endif
            }
            fileHTTP.end();
        }
    } else {
        Serial.println("Ungültiges JSON-Array");
    }
}
*/
/*
void downloadFile(const String& url, const String& filePath) {
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        File file = SPIFFS.open(filePath, "w");
        if (file) {
            file.write((const uint8_t*)http.getString().c_str(), http.getSize());
            file.close();
        }
    }
    http.end();
}
*/
/*
void test(const String& folderPath) {
    HTTPClient http;
    
    // HTTP-Anfrage senden, um den Inhalt des Ordners abzurufen
    String folderURL = "https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/" + system_funktion.fw_art + "/data" + folderPath;
    http.begin(folderURL);
    int httpCode = http.GET();
    
    if (httpCode == HTTP_CODE_OK) {
        // Lesen des Inhalts der Antwort, der die Dateinamen im Ordner enthält
        String folderContent = http.getString();

        // Aufteilen des Ordnerinhalts in einzelne Dateinamen
        int startPos = 0;
        int endPos = 0;
        while ((startPos = folderContent.indexOf("href=\"", startPos)) != -1) {
            startPos += 6; // Länge von "href=\""
            endPos = folderContent.indexOf("\"", startPos);
            if (endPos != -1) {
                String filename = folderContent.substring(startPos, endPos);
                
                // Ignorieren von Links auf übergeordnete Ordner und Verzeichnisse
                if (!filename.endsWith("/")) {
                    // Herunterladen der Datei
                    String fileURL = folderURL + "/" + filename;
                    HTTPClient fileHTTP;
                    fileHTTP.begin(fileURL);
                    int fileHTTPCode = fileHTTP.GET();
                    if (fileHTTPCode == HTTP_CODE_OK) {
                        // Öffnen und Schreiben der heruntergeladenen Datei im SPIFFS
                        File file = SPIFFS.open("/html" + folderPath + "/" + filename, "w");
                        if (file) {
                            String fileContent = fileHTTP.getString();
                            file.write((const uint8_t*)fileContent.c_str(), fileContent.length());
                            file.close();
                            #ifdef DEBUG_SERIAL_OUTPUT
                                Serial.println("Datei heruntergeladen: /html" + folderPath + "/" + filename);
                            #endif
                        } else {
                            #ifdef DEBUG_SERIAL_OUTPUT
                                Serial.println("Fehler beim Öffnen der Datei: /html" + folderPath + "/" + filename);
                            #endif
                        }
                    } else {
                        #ifdef DEBUG_SERIAL_OUTPUT
                            Serial.println("Fehler beim Herunterladen der Datei: " + fileURL);
                        #endif
                    }
                    fileHTTP.end();
                }
            }
        }
    } else {
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.println("Fehler beim Abrufen des Ordnerinhalts: " + folderURL);
        #endif
    }

    http.end();
}
*/
void firmwareupdate_http() {
    if (WiFi.isConnected() && system_funktion.new_version) {
        String url = "https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/" + system_funktion.fw_art + "/firmware/firmware.bin";
        system_funktion.new_version = false;
        webserver.sperre = true;
        //file_download("/config.html");
        //file_download("/config.css");

        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.println("Firmware-Update wird gestartet...");
            Serial.println("Url ist : " + url );
        #endif

        t_httpUpdate_return ret = ESPhttpUpdate.update(url);
                
        switch (ret) {
            case HTTP_UPDATE_FAILED:
                #ifdef DEBUG_SERIAL_OUTPUT
                    Serial.printf("Firmware-Update fehlgeschlagen. Fehler (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                #endif
                    webserver.sperre = false;
                break;
            case HTTP_UPDATE_NO_UPDATES:
                #ifdef DEBUG_SERIAL_OUTPUT
                    Serial.println("Keine neuen Firmware-Updates verfügbar.");
                #endif
                break;
            case HTTP_UPDATE_OK:
                #ifdef DEBUG_SERIAL_OUTPUT
                    Serial.println("Firmware-Update erfolgreich.");
                #endif
                break;
        }
    } else {
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.println("Firmware-Update abgebrochen da vorgaben nicht erfüllt ...");
        #endif
        mqtt_publish( mqtt.topic_base + "/" + mqtt.topic_define + "/ESP/Firmwareupdate", "false", "mqtt_mqtt_sub_read");
    }
}

void led_flash_timer(int timer_on,int timer_off, int number)
{
    for (int i = 0; i < number; i++)
    {
        digitalWrite(output_led_detector, HIGH);
        digitalWrite(output_led_esp, HIGH);
        delay(timer_on);
        digitalWrite(output_led_detector, LOW);
        digitalWrite(output_led_esp, LOW);
        delay(timer_off);
    }
}

void load_conf_sys(StaticJsonDocument<1024> doc)
{
    #ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("... Sys- Variablen ...");
    #endif
    system_funktion.fw_art = doc["fw"] | "main";
    system_funktion.ota = doc["ota"] | false;
    system_funktion.ota_pw = doc["ota-pw"] | "";
}

StaticJsonDocument<1024> safe_conf_sys(StaticJsonDocument<1024> doc)
{
    #ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("... Sys- Variablen ...");
    #endif
    doc["fw"] = system_funktion.fw_art;
    doc["ota"] = system_funktion.ota;
    doc["ota-pw"] = system_funktion.ota_pw;

    return doc;
}


String web_request_sys(const String &var) {
    if              (var == "button_firmware")      { 
        if          ( system_funktion.fw_art == "main" ) {   return "<option value='main' selected>main</option><option value='beta'>beta</option>" ; 
        }else if    ( system_funktion.fw_art == "beta" ) {   return "<option value='beta' selected>beta</option><option value='main'>main</option>";
        }                                                                                     
    } else if       (var == "text_firmware_old")    { return system_funktion.version_old;
    } else if       (var == "text_firmware_new")    { return system_funktion.version_new;
    } else if       (var == "button_ota")           { return ( system_funktion.ota )                ? "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert</option>"
                                                                                                    : "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert</option>";
    } else if       (var == "textarea_fw_pw")       { return (system_funktion.ota_pw        != "")  ? "---FFF---FFF---" 
                                                                                                    : "Bitte eintragen !"; 
    }
    
    return String();
}
void web_response_sys(String name, String msg) {
    if (msg != "")
    {
        if (name == "fw_build")         { system_funktion.fw_art = msg; version_check(); }
        if (name == "Firmwareupdate")   { firmwareupdate_http(); }
        if (name == "fw_ota")           { if (msg == "aktiviert")   system_funktion.ota = true; }
        if (name == "fw_ota")           { if (msg == "deaktiviert") system_funktion.ota = false; }
        if (name == "ota_pw")   {
                                        if          ( msg == "") {                      system_funktion.ota_pw = "";
                                        } else if   ( msg == "Bitte eintragen !") {     system_funktion.ota_pw = "";
                                        } else if   ( msg == "---FFF---FFF---") {       return;
                                        } else if   ( msg != "") {                      system_funktion.ota_pw = msg;
                                        }
        }
    }
}