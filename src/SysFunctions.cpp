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
        HTTPClient http;
        http.begin( "https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/" + system_funktion.fw_art + + "firmware/version.txt" );
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

void file_download(String filePath) {
    #ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("Fieledownload URL : https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/" + system_funktion.fw_art + filePath);
    #endif
  
    // HTTP-Anfrage senden
    http.begin("https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/" + system_funktion.fw_art + "/data" + filePath);
  
    // Überprüfen der Antwort
    int httpCode = http.GET();
    if(httpCode == HTTP_CODE_OK) {
        // Datei öffnen, um im SPIFFS zu speichern
        File file = SPIFFS.open(filePath, "w");
        if(!file) {
            #ifdef DEBUG_SERIAL_OUTPUT
                Serial.println("File kann nicht erstellt werden");
            #endif
            return;
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
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.println("File " + filePath + " erfolgreich gedownloadet");
        #endif
    } else {
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.printf("File download fehlgeschlagen mit : : %d\n", httpCode);
        #endif
    }
  
    // HTTP-Verbindung schließen
    http.end();
}

void folder_download(const String& folderPath) {
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
                        File file = SPIFFS.open(folderPath + "/" + filename, "w");
                        if (file) {
                            String fileContent = fileHTTP.getString();
                            file.write((const uint8_t*)fileContent.c_str(), fileContent.length());
                            file.close();
                            #ifdef DEBUG_SERIAL_OUTPUT
                                Serial.println("Datei heruntergeladen: " + folderPath + "/" + filename);
                            #endif
                        } else {
                            #ifdef DEBUG_SERIAL_OUTPUT
                                Serial.println("Fehler beim Öffnen der Datei: " + folderPath + "/" + filename);
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

void firmwareupdate_http() {
    if (WiFi.isConnected() && system_funktion.new_version) {
        system_funktion.new_version = false;
        webserver.sperre = true;
        file_download("/config.html");
        file_download("/config.css");

        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.println("Firmware-Update wird gestartet...");
            Serial.println("Url ist : https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/" + system_funktion.fw_art + "/firmware/firmware.bin");
        #endif

        //String firmwareURL = "https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/" + system_funktion.fw_art + "/firmware/firmware.bin";
        t_httpUpdate_return ret = ESPhttpUpdate.update("https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/" + system_funktion.fw_art + "/firmware/firmware.bin");
                
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
}

StaticJsonDocument<1024> safe_conf_sys(StaticJsonDocument<1024> doc)
{
    #ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("... Sys- Variablen ...");
    #endif
    doc["fw"] = system_funktion.fw_art;

    return doc;
}


String web_request_sys(const String &var) {
    if          (var == "ph_sys_fwtyp") {
        return (system_funktion.fw_art == "main" )  ? "'main' selected='main'>main</option><option value='beta'>beta" 
                                                    : "'beta' selected='beta'>beta</option><option value='main'>main";
    } else if   (var == "ph_sys_fwold") {
        return system_funktion.version_old ;
    } else if   (var == "ph_sys_fwnew") {
        return system_funktion.version_new ;
    }
    return String();
}
void web_response_sys(String name, String msg) {
    if (msg != "")
    {
        if (name == "fw_build")         { system_funktion.fw_art = msg; version_check(); }
        if (name == "Firmwareupdate")   { firmwareupdate_http(); }
    }
}