#include "SysHeaders.h"

WEBSERVER webserver;

void webserver_art()
{
  server = new AsyncWebServer(80);
  if (SPIFFS.exists("/config.html"))
  {
    #ifdef DEBUG_SERIAL_WEBSERVER
      Serial.println("config.html ist vorhanden = Normalbetrieb");
    #endif
    webserver_normalbetrieb();
  }
  else
  {
    #ifdef DEBUG_SERIAL_WEBSERVER
      Serial.println("config.html ist nicht vorhanden = Notbetrieb");
    #endif
    webserver.notbetrieb=true;
    webserver_notbetrieb();
  }
}

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
<form action="/get"><table><tbody>
<tr><td>ESP- Name :</td><td><input class="setting" type="text" name="esp_name" placeholder="%textarea_network_esp%"></td></tr>
<tr><td>WiFi - SSID :</td><td>
<select name="wifi_ssid">
%button_network_ssid_selected%
%button_network_ssid_option%
</select></td></tr>
<tr><td>WiFi - Passwort :</td><td><input class="setting" type="text" name="wifi_pw" placeholder="%textarea_network_pw%"></td></tr>
</tbody></table><br><input type="submit" value="Änderungen übernehmen" name="config_save"></form>
<br>
<h3>Datein Upload</h3>
<tr><td align=center height=50>Free Storage: %FREESPIFFS% | Used Storage: %USEDSPIFFS% | Total Storage: %TOTALSPIFFS%</td></tr>
<tr><td align=center height=25><form method="POST" action="/upload" enctype="multipart/form-data"><input type="file" name="data"/></td></tr>
<tr><td align=center height=25><input type="submit" name="upload" value="Upload" title="Upload File"></form></td></tr>
<br>
<tr><td align=center height=50>Auf dem ESP vorhandene Daten :</td></tr>
<tr><td align=center>%FILELIST%</td></tr>
</table>
</center>
</body>
</html>
)rawliteral";

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  #ifdef DEBUG_SERIAL_WEBSERVER
    Serial.println("Client:" + request->client()->remoteIP().toString() + " " + request->url());
  #endif

  if (!index)
  {
    // open the file on first call and store the file handle in the request object
    request->_tempFile = SPIFFS.open("/" + filename, "w");
    #ifdef DEBUG_SERIAL_WEBSERVER
      Serial.println("Upload Start: " + String(filename));
    #endif
  }

  if (len)
  {
    // stream the incoming chunk to the opened file
    request->_tempFile.write(data, len);
    #ifdef DEBUG_SERIAL_WEBSERVER
      Serial.println("Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len));
    #endif
  }

  if (final)
  {
    // close the file handle as the upload is now done
    request->_tempFile.close();
    #ifdef DEBUG_SERIAL_WEBSERVER
      Serial.println("Upload Complete: " + String(filename) + ",size: " + String(index + len));
    #endif
    request->redirect("/");
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

String listFiles(bool ishtml)
{
  String returnText = "";
  #ifdef DEBUG_SERIAL_WEBSERVER
    Serial.println("Listing files stored on SPIFFS");
  #endif
  File root = SPIFFS.open("/");
  File foundfile = root.openNextFile();
  if (ishtml)
  {
    returnText += "<table class='upload-50b'><tr><th align=' left '>Name</th><th align=' left'>Size</th></tr>";
  }
  while (foundfile)
  {
    if (ishtml)
    {
      returnText += "<tr align='left'><td>" + String(foundfile.name()) + "</td><td>" + humanReadableSize(foundfile.size()) + "</td></tr>";
    }
    else
    {
      returnText += "File: " + String(foundfile.name()) + "\n";
    }
    foundfile = root.openNextFile();
  }
  if (ishtml)
  {
    returnText += "</table>";
  }
  root.close();
  foundfile.close();
  return returnText;
}

void webserver_notbetrieb()
{
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
             {
    #ifdef DEBUG_SERIAL_WEBSERVER
      Serial.println("Client:" + request->client()->remoteIP().toString() + + " " + request->url());
    #endif
    request->send_P(200, "text/html", notbetrieb_html, web_request); });

  webserver_config();
}

void webserver_normalbetrieb()
{
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
             { request->send(SPIFFS, "/config.html", String(), false, web_request); });
  

  server->on("/config.css", HTTP_GET, [](AsyncWebServerRequest *request)
             { request->send(SPIFFS, "/config.css", "text/css"); });

  webserver_config();
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
  if (var == "header_esp_name" ) return wifi.esp_name;
  if (var == "header_detector_location" ) return detector.location;
  
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
  
  if ( var == "ph_web_config" ) { return (!webserver.config)   ? "display: none; " 
                                                              : "";
  }
  
  if          (var == "display_network")    { return ( webserver.navigation == "Netzwerk")      ? ""
                                                                                                : "style= 'display: none'";
  } else if   (var == "display_detector")   { return ( webserver.navigation == "Rauchmelder")   ? ""
                                                                                                : "style= 'display: none'";
  } else if   (var == "display_sensors")    { return ( webserver.navigation == "Sensoren")      ? ""
                                                                                                : "style= 'display: none'";
  } else if   (var == "display_system")     { return ( webserver.navigation == "System")        ? ""
                                                                                                : "style= 'display: none'";
  } else if   (var == "display_logging")    { return ( webserver.navigation == "Logging")       ? ""
                                                                                                : "style= 'display: none'";
  }
  
  
  Serial.println("Variable - navigation = " + webserver.navigation);
  if (var == "navigation-network" &&
      webserver.navigation != "Netzwerk")
  {
    return "display: none;";
  }
  if (var == "navigation-detector" &&
      webserver.navigation != "Rauchmelder")
  {
    return "display: none;";
  }
  if (var == "navigation-sensor" &&
      webserver.navigation != "Sensoren")
  {
    return "display: none;";
  }
  if (var == "navigation-system" &&
      webserver.navigation != "System")
  {
    return "display: none;";
  }
  if (var == "navigation-log" &&
      webserver.navigation != "Logging")
  {
    return "display: none;";
  }
 
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
  
  web_response_sys(name,value);
  webserver_triger_wifi(name, value);
  web_response_mqtt(name, value);
  webserver_triger_detector(name, value);
  web_response_sensor(name, value);

  if        (name == "navigation")                                      {    webserver.navigation = value; Serial.println("Variable - navigation = " + webserver.navigation);}
  
  else if   (name == "config_save" && value == "Änderungen übernehmen") {     spiffs_config_save();
                                                                            webserver.config = true; }
  else if   (name == "ESP-Neustart")                                        ESP.restart();
  
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

void webserver_config()
{
    // run handleUpload function when any file is uploaded
    server->on(
        "/upload", HTTP_POST, [](AsyncWebServerRequest *request)
        { request->send(200); },
        handleUpload);

    server->on(
        "/post", HTTP_POST, [](AsyncWebServerRequest *request)
        { request->send(200, "text/plain", "Post route"); });

    server->on(
        "/get", HTTP_GET, [](AsyncWebServerRequest *request)
        {   scan_wifi_ssid();
            String art;
            String id;
            String msg;
            int params = request->params();
            for (int i = 0; i < params; i++)
            {
                AsyncWebParameter *p = request->getParam(i);
                if (p->isPost())
                { 
                    art = "File";
                    id = p->name().c_str();
                    msg = p->value().c_str();
                    #ifdef DEBUG_SERIAL_WEBSERVER
                      Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
                    #endif
                }
                else if (p->isPost())
                {
                    art = "Post";
                    id = p->name().c_str();
                    msg = p->value().c_str();
                    #ifdef DEBUG_SERIAL_WEBSERVER
                      Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
                    #endif
                }
                else
                {
                    art = "Get";
                    id = p->name().c_str();
                    msg = p->value().c_str();
                    #ifdef DEBUG_SERIAL_WEBSERVER
                      Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
                    #endif
                }
                //web_response(id,msg);

                #ifdef DEBUG_SERIAL_WEBSERVER
                  Serial.print("MSG : ");
                  Serial.print(art);
                  Serial.print(" | ");
                  Serial.print(id);
                  Serial.print(" | ");
                  Serial.println(msg);
                #endif
            }
            request->send(SPIFFS, "/config.html", String(), false, web_request); });
}

void webserver_setup(){
  #ifdef DEBUG_SERIAL_WEBSERVER
    Serial.println("Webserver wird konfiguriert");
  #endif
  server = new AsyncWebServer(80);
  
  // Routen festlegen
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String art;
    String name;
    String value;
    int params = request->params();
    #ifdef DEBUG_SERIAL_WEBSERVER
      Serial.print("URL der Anfrage: ");
      Serial.println(request->url());
      Serial.print("HTTP-Methode der Anfrage: ");
      Serial.println(request->methodToString());
      Serial.print("IP-Adresse des Clients: ");
      Serial.println(request->client()->remoteIP().toString());
    #endif
    // Behandlung von GET- und POST-Anfragen
    if (request->method() == HTTP_GET) {
      #ifdef DEBUG_SERIAL_WEBSERVER
        Serial.printf("-> Get Funktion <-");
      #endif
      art = "Get"; // Hier initialisieren
      name = "";
      value = "";
      for (int i = 0; i < params; i++) {
        AsyncWebParameter *p = request->getParam(i);
        name = p->name().c_str();
        value = p->value().c_str();
        web_response_GET(name,value);
        #ifdef DEBUG_SERIAL_WEBSERVER
          Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
        #endif
            // Hier können Sie die GET-Parameter verarbeiten
            // z.B. p->name() und p->value()
      }
        // Logik für GET-Anfragen
        // Hier können Sie die GET-Parameter verarbeiten
        // und entsprechend reagieren
    } /*else if (request->method() == HTTP_POST) {
      #ifdef DEBUG_SERIAL_WEBSERVER
        Serial.printf("-> Post Funktion <-");
      #endif
      art = "Post"; // Hier initialisieren
      id = "";
      msg = "";
      for (int i = 0; i < params; i++) {
        AsyncWebParameter *p = request->getParam(i);
        id = p->name().c_str();
        msg = p->value().c_str();
        #ifdef DEBUG_SERIAL_WEBSERVER
          Serial.println("WIR TESTEN EIN POST");
          Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        #endif
            // Hier können Sie die POST-Daten verarbeiten
            // z.B. p->name() und p->value()
      }
        // Logik für POST-Anfragen
        // Hier können Sie die POST-Daten verarbeiten
        // und entsprechend reagieren
    }*/
    
    if (webserver.notbetrieb) {
      #ifdef DEBUG_SERIAL_WEBSERVER
        Serial.println("Notbetrieb");
      #endif
      // Logik für Notbetriebsseite
      request->send_P(200, "text/html", notbetrieb_html);
    } else {
      #ifdef DEBUG_SERIAL_WEBSERVER
        Serial.println("Normalbetrieb");
        Serial.print("*Request- Methode : ");
        Serial.println(request->method());
      #endif
      // Seite aus dem SPIFFS laden
      request->send(SPIFFS, "/config.html", String(), false, web_request);
      //request->send(SPIFFS, "/config.css", "text/css");
    }
  
    
  
  });
  server->on("/Netzwerk", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/html/network.html", String(), false, web_request);
  });
  server->on("/Rauchmelder", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/html/detector.html", String(), false, web_request);
  });
  server->on("/Sensoren", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/html/sensor.html", String(), false, web_request);
  });
  server->on("/System", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/html/system.html", String(), false, web_request);
  });
  server->on("/Logging", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/html/logging.html", String(), false, web_request);
  }); 
  /*  
    File file = SPIFFS.open("/network_settings.html", "r");
    if (!file) {
        request->send(404, "text/plain", "Datei nicht gefunden");
        return;
    }
    String fileContent = file.readString();
    file.close();

    // Verarbeite die Platzhalter in fileContent mit dem Callback web_request
    fileContent = web_request(fileContent);

    request->send(200, "text/html", fileContent);
});*/
  
  // Start des Servers
  #ifdef DEBUG_SERIAL_WEBSERVER
    Serial.println("Starten des Webservers");
  #endif
  server->begin();
}

void webserver_file(AsyncWebServerRequest *request, String path, String contentType) {
  #ifdef DEBUG_SERIAL_WEBSERVER
    Serial.println("das funktioniert auch");
  #endif
  #ifdef DEBUG_SERIAL_WEBSERVER
    Serial.println("Daten von den File laden : " + path );
  #endif
  if (SPIFFS.exists(path)) {
    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, path, contentType);
    request->send(response);
  } else {
    request->send(404, "text/plain", "Datei nicht gefunden");
  }
}
