#include "SysHeaders.h"

WEBSERVER webserver;

const char notbetrieb_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="en">
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<meta charset="UTF-8">
</head>
<body>
<center>
<h1>! Notbetrieb !</h1>
<h3>Netzwerk Einstellungen</h3>
<form action="/" method="GET">
<table><tbody>
<tr><td>ESP- Name :</td><td><input class="setting" type="text" name="esp_name" placeholder="%textarea_network_esp%"></td></tr>
<tr><td>WiFi - SSID :</td><td><select name="wifi_ssid">%button_network_ssid_selected% %button_network_ssid_option% </select></td></tr>
<tr><td>WiFi - Passwort :</td><td><input class="setting" type="text" name="wifi_pw" placeholder="%textarea_network_pw%"></td></tr>
</tbody></table><br><input type="submit" value="Änderungen übernehmen" name="config_save"></form>
<br>
<h3>Datein Upload</h3>
<table><tbody>
<tr><td><form method="POST" action="/upload" enctype="multipart/form-data">
<input type="file" name="data"><input type="submit" name="upload" value="Upload" title="Upload File">
</form></td></tr></tbody></table>
<br>
<h3>Auf dem ESP vorhandene Daten :</h3>
<table><tbody>
%spiff_info_files%
</tbody></table>
</center>
</body>
</html>
)rawliteral";

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  // Überprüfe, ob das uploadPath-Feld vorhanden ist und lese den Wert aus
    Serial.print("Wert des uploadPath-Feldes FUNKTION : ");
    Serial.println(webserver.uploadFolder);
  
  #ifdef DEBUG_SERIAL_WEBSERVER
    Serial.println("Client:" + request->client()->remoteIP().toString() + " " + request->url());
  #endif
  if (!index) {
    // open the file on first call and store the file handle in the request object
    request->_tempFile = SPIFFS.open(webserver.uploadFolder + filename, "w");
    #ifdef DEBUG_SERIAL_WEBSERVER
      Serial.println("Upload Start: " + String(filename));
    #endif
  }
  if (len) {
    // stream the incoming chunk to the opened file
    request->_tempFile.write(data, len);
    #ifdef DEBUG_SERIAL_WEBSERVER
      Serial.println("Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len));
    #endif
  }

  if (final) {
    // close the file handle as the upload is now done
    request->_tempFile.close();
    #ifdef DEBUG_SERIAL_WEBSERVER
      Serial.println("Upload Complete: " + String(filename) + ",size: " + String(index + len));
    #endif
    if ( request->url() == "/System/upload" ){
      request->redirect("/System/");
    } else {
      request->redirect("/");
    }
  }
}


String web_request(const String &var)
{
  if ( webserver.sperre ) return String();
  #ifdef DEBUG_SERIAL_WEBSERVER
    Serial.print("* web_request mit : ");
    Serial.println(var);
  #endif
  String temp = "";
  // Überschrift
  if (var == "header_esp_name" )            return wifi.esp_name;
  if (var == "header_detector_location" )   return detector.location;
  if (var == "textarea_system_upload" )     return "<input type='text' id='uploadPath' name='uploadPath' value='" + webserver.uploadFolder + "'>";
  if (var == "display_div_menue" )          return (webserver.config)     ? ""
                                                                          : "style='display: none'";
  
  // Netzwerk
  temp = web_request_wifi(var);         if (temp != "") return temp; 
  temp = web_request_mqtt(var);         if (temp != "") return temp;
  
  // Detector
  temp = web_request_detector(var);     if (temp != "") return temp;

  // Sensor
  temp = web_request_sensor(var);    if (temp != "") return temp;

  // System
  temp = web_request_sys(var);          if (temp != "") return temp;
  temp = web_request_spiff(var);        if (temp != "") return temp;
 
  return String();
}

void web_response_GET(String name, String value)
{
  #ifdef DEBUG_SERIAL_WEBSERVER
    Serial.print("*web_response_GET* name : ");
    Serial.print(name);
    Serial.print(" value : ");
    Serial.println(value);
  #endif 
  if ( name == "config_save" && value == "Änderungen übernehmen" ) webserver.config = true;
  
  web_response_spiff(name,value);
  web_response_sys(name,value);
  web_response_wifi(name, value);
  web_response_mqtt(name, value);
  web_response_detector(name, value);
  web_response_sensor(name, value);

  if        (name == "navigation")                                      {    webserver.navigation = value; Serial.println("Variable - navigation = " + webserver.navigation);}
  
  else if   (name == "config_save" && value == "Änderungen übernehmen") {     spiffs_config_save();
                                                                            webserver.config = true; }
  else if   (name == "ESP-Neustart")                                        ESP.restart();
  else if   (name == "uploadPath")                                          webserver.uploadFolder = value;
  
  if (name == "reset_config") {
    if (value == "Werkseinstellungen laden !") {
      SPIFFS.remove(safefile);
      #ifdef DEBUG_SERIAL_WEBSERVER
        Serial.println("Savefile wurde gelöscht !");
        Serial.println();
        Serial.println("ESP wird in 6 Sekunden neugestartet !");
      #endif
      delay(6000);
      ESP.restart();
    }
  }
}

void webserver_setup(){
  String uploadFolder;
  #ifdef DEBUG_SERIAL_WEBSERVER
    Serial.print("Webserver wird konfiguriert : ");
    if ( webserver.notbetrieb ) { 
      Serial.println( " -> für Notbetrieb <-");
    } else {
      Serial.println( " -> für Normalbetrieb <-");
    }
  #endif
  server = new AsyncWebServer(80);
  
  // Routen festlegen
  server->on("/System/download", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Pfad zur herunterzuladenden Datei aus den Abfrageparametern erhalten
    String filePath = request->arg("filepath");  
    #ifdef DEBUG_SERIAL_WEBSERVER
      Serial.print("Downloadanfrage gesendet, der Pfad ist : ");
      Serial.println(filePath);
    #endif
    // Öffne die Datei zum Lesen
    File file = SPIFFS.open(filePath, "r");
    if (!file) {
        request->send(404, "text/plain", "Datei nicht gefunden");
        return;
    }
    #ifdef DEBUG_SERIAL_WEBSERVER
      Serial.println("Dateiinhalt:");
      while (file.available()) {
        Serial.write(file.read());
      }
    #endif
    // Bestimme die Dateigröße
    size_t fileSize = file.size();
    // Setze den Content-Type entsprechend des Dateityps
    String contentType = "application/octet-stream";
    if (filePath.endsWith(".html")) {
      contentType = "text/html";
    } else if (filePath.endsWith(".txt")) {
      contentType = "text/plain";
    } else if (filePath.endsWith(".jpg") || filePath.endsWith(".jpeg")) {
      contentType = "image/jpeg";
    } else if (filePath.endsWith(".png")) {
      contentType = "image/png";
    }
    // Sende die HTTP-Antwort mit der Datei
    request->send(SPIFFS, filePath, contentType, true); // Hier wurde der letzte Parameter auf "true" geändert
    // Datei schließen
    file.close();
  });
  
  server->on(
    "/System/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200);
    },
    handleUpload
  );

  server->on(
    "/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
        request->send(200);
    },
    handleUpload
  );
  
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->method() == HTTP_GET) {
      String art = "Get"; // Hier initialisieren
      String name = "";
      String value = "";
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter *p = request->getParam(i);
        name = p->name().c_str();
        value = p->value().c_str();
        #ifdef DEBUG_SERIAL_WEBSERVER
          String logMessage = "GET - " + request->url() + " [%s]: %s\n";
          Serial.printf(logMessage.c_str(), p->name().c_str(), p->value().c_str());
        #endif
        web_response_GET(name,value);
      }
    }      
    if (webserver.notbetrieb) {
      #ifdef DEBUG_SERIAL_WEBSERVER
        Serial.println("Notbetrieb");
      #endif
      // Logik für Notbetriebsseite
      request->send_P(200, "text/html", notbetrieb_html, web_request);
    } else {
      #ifdef DEBUG_SERIAL_WEBSERVER
        Serial.println("Normalbetrieb");
      #endif
      String temp = "";
      temp += loadFileContent("/html/head.html");
      temp += "";
      temp += loadFileContent("/html/bottom.html");
      size_t combinedContentLength = temp.length();

      // Speicher für den kombinierten Inhalt vorbereiten
      uint8_t *combinedContent = new uint8_t[combinedContentLength + 1]; // +1 für das Nullterminierungszeichen
      size_t index = 0;

      for (size_t i = 0; i < temp.length(); i++) {
        combinedContent[index++] = temp[i];
      }
      combinedContent[combinedContentLength] = '\0'; // Nullterminierungszeichen hinzufügen

      // Senden der Antwort
      request->send_P(200, "text/html", combinedContent, combinedContentLength, web_request);

      // Speicher freigeben
      delete[] combinedContent;
    } 
  });
  
  server->on("/Netzwerk", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->method() == HTTP_GET) {
      String art = "Get"; // Hier initialisieren
      String name = "";
      String value = "";
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter *p = request->getParam(i);
        name = p->name().c_str();
        value = p->value().c_str();
        #ifdef DEBUG_SERIAL_WEBSERVER
          String logMessage = "GET - " + request->url() + " [%s]: %s\n";
          Serial.printf(logMessage.c_str(), p->name().c_str(), p->value().c_str());
        #endif
        web_response_GET(name,value);
      }
    }
    String temp = "";
    temp += loadFileContent("/html/head.html");
    temp += loadFileContent("/html/network.html");
    temp += loadFileContent("/html/bottom.html");
    size_t combinedContentLength = temp.length();

    // Speicher für den kombinierten Inhalt vorbereiten
    uint8_t *combinedContent = new uint8_t[combinedContentLength + 1]; // +1 für das Nullterminierungszeichen
    size_t index = 0;

    for (size_t i = 0; i < temp.length(); i++) {
        combinedContent[index++] = temp[i];
    }
    combinedContent[combinedContentLength] = '\0'; // Nullterminierungszeichen hinzufügen

    // Senden der Antwort
    request->send_P(200, "text/html", combinedContent, combinedContentLength, web_request);

    // Speicher freigeben
    delete[] combinedContent;
  });
  
  server->on("/Rauchmelder", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->method() == HTTP_GET) {
      String art = "Get"; // Hier initialisieren
      String name = "";
      String value = "";
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter *p = request->getParam(i);
        name = p->name().c_str();
        value = p->value().c_str();
        #ifdef DEBUG_SERIAL_WEBSERVER
          String logMessage = "GET - " + request->url() + " [%s]: %s\n";
          Serial.printf(logMessage.c_str(), p->name().c_str(), p->value().c_str());
        #endif
        web_response_GET(name,value);
      }
    }
    String temp = "";
    temp += loadFileContent("/html/head.html");
    temp += loadFileContent("/html/detector.html");
    temp += loadFileContent("/html/bottom.html");
    size_t combinedContentLength = temp.length();
  
    // Speicher für den kombinierten Inhalt vorbereiten
    uint8_t *combinedContent = new uint8_t[combinedContentLength + 1]; // +1 für das Nullterminierungszeichen
    size_t index = 0;

    for (size_t i = 0; i < temp.length(); i++) {
        combinedContent[index++] = temp[i];
    }
    combinedContent[combinedContentLength] = '\0'; // Nullterminierungszeichen hinzufügen

    // Senden der Antwort
    request->send_P(200, "text/html", combinedContent, combinedContentLength, web_request);

    // Speicher freigeben
    delete[] combinedContent;
  });
  
  server->on("/Sensoren", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->method() == HTTP_GET) {
      String art = "Get"; // Hier initialisieren
      String name = "";
      String value = "";
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter *p = request->getParam(i);
        name = p->name().c_str();
        value = p->value().c_str();
        #ifdef DEBUG_SERIAL_WEBSERVER
          String logMessage = "GET - " + request->url() + " [%s]: %s\n";
          Serial.printf(logMessage.c_str(), p->name().c_str(), p->value().c_str());
        #endif
        web_response_GET(name,value);
      }
    }
    String temp = "";
    temp += loadFileContent("/html/head.html");
    temp += loadFileContent("/html/sensor.html");
    temp += loadFileContent("/html/bottom.html");
    size_t combinedContentLength = temp.length();

    // Speicher für den kombinierten Inhalt vorbereiten
    uint8_t *combinedContent = new uint8_t[combinedContentLength + 1]; // +1 für das Nullterminierungszeichen
    size_t index = 0;

    for (size_t i = 0; i < temp.length(); i++) {
        combinedContent[index++] = temp[i];
    }
    combinedContent[combinedContentLength] = '\0'; // Nullterminierungszeichen hinzufügen

    // Senden der Antwort
    request->send_P(200, "text/html", combinedContent, combinedContentLength, web_request);

    // Speicher freigeben
    delete[] combinedContent;
  });
  
  server->on("/System", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->method() == HTTP_GET) {
      String art = "Get"; // Hier initialisieren
      String name = "";
      String value = "";
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter *p = request->getParam(i);
        name = p->name().c_str();
        value = p->value().c_str();
        #ifdef DEBUG_SERIAL_WEBSERVER
          String logMessage = "GET - " + request->url() + " [%s]: %s\n";
          Serial.printf(logMessage.c_str(), p->name().c_str(), p->value().c_str());
        #endif
        web_response_GET(name,value);
      }
    }
    String temp = "";
    temp += loadFileContent("/html/head.html");
    temp += loadFileContent("/html/system.html");
    temp += loadFileContent("/html/bottom.html");
    size_t combinedContentLength = temp.length();

    // Speicher für den kombinierten Inhalt vorbereiten
    uint8_t *combinedContent = new uint8_t[combinedContentLength + 1]; // +1 für das Nullterminierungszeichen
    size_t index = 0;

    for (size_t i = 0; i < temp.length(); i++) {
        combinedContent[index++] = temp[i];
    }
    combinedContent[combinedContentLength] = '\0'; // Nullterminierungszeichen hinzufügen

    // Senden der Antwort
    request->send_P(200, "text/html", combinedContent, combinedContentLength, web_request);

    // Speicher freigeben
    delete[] combinedContent;
  });
  
  server->on("/Logging", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->method() == HTTP_GET) {
      String art = "Get"; // Hier initialisieren
      String name = "";
      String value = "";
      int params = request->params();
      for (int i = 0; i < params; i++) {
        AsyncWebParameter *p = request->getParam(i);
        name = p->name().c_str();
        value = p->value().c_str();
        #ifdef DEBUG_SERIAL_WEBSERVER
          String logMessage = "GET - " + request->url() + " [%s]: %s\n";
          Serial.printf(logMessage.c_str(), p->name().c_str(), p->value().c_str());
        #endif
        web_response_GET(name,value);
      }
    }
    String temp = "";
    temp += loadFileContent("/html/head.html");
    temp += loadFileContent("/html/logging.html");
    temp += loadFileContent("/html/bottom.html");
    size_t combinedContentLength = temp.length();

    // Speicher für den kombinierten Inhalt vorbereiten
    uint8_t *combinedContent = new uint8_t[combinedContentLength + 1]; // +1 für das Nullterminierungszeichen
    size_t index = 0;

    for (size_t i = 0; i < temp.length(); i++) {
        combinedContent[index++] = temp[i];
    }
    combinedContent[combinedContentLength] = '\0'; // Nullterminierungszeichen hinzufügen

    // Senden der Antwort
    request->send_P(200, "text/html", combinedContent, combinedContentLength, web_request);

    // Speicher freigeben
    delete[] combinedContent;
  }); 

  #ifdef DEBUG_SERIAL_WEBSERVER
    Serial.println("Starten des Webservers");
  #endif
  server->begin();
}

String loadFileContent(const char *filePath) {
    File file = SPIFFS.open(filePath, "r");
    if (!file) {
        Serial.println("Fehler beim Öffnen der Datei");
        return String();
    }

    String fileContent;
    while (file.available()) {
        fileContent += (char)file.read();
    }

    file.close();
    return fileContent;
}

bool check_files(){
  if (!WiFi.isConnected()){
    if ( !SPIFFS.exists("/html/network.html"))  return true;
    if ( !SPIFFS.exists("/html/head.html"))     return true;
    if ( !SPIFFS.exists("/html/bottom.html"))   return true;
  } else {
    int error = 0;
    if ( !SPIFFS.exists("/html/network.html"))   if (!file_download("/data/html/network.html", "/html/network.html"))   ++error ;
    if ( !SPIFFS.exists("/html/detector.html"))  if (file_download("/data/html/detector.html", "/html/detector.html"))  ++error ;
    if ( !SPIFFS.exists("/html/sensor.html"))    if (file_download("/data/html/sensor.html", "/html/sensor.html"))      ++error ;
    if ( !SPIFFS.exists("/html/system.html"))    if (file_download("/data/html/system.html", "/html/system.html"))      ++error ;
    if ( !SPIFFS.exists("/html/logging.html"))   if (file_download("/data/html/logging.html", "/html/logging.html"))    ++error ;
    if ( !SPIFFS.exists("/html/head.html"))      if (file_download("/data/html/head.html", "/html/head.html"))          ++error ;
    if ( !SPIFFS.exists("/html/bottom.html"))    if (file_download("/data/html/bottom.html", "/html/bottom.html"))      ++error ;
    if ( error == 0 ) {
      return false;
    } else {
      return true;
    }
    return true;
  }
  return true;
}