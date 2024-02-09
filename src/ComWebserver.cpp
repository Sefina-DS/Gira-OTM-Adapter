#include "SysHeaders.h"

WEBSERVER webserver;

void webserver_art()
{
  server = new AsyncWebServer(80);
  if (SPIFFS.exists("/config.html"))
  {
    #ifdef DEBUG_SERIAL_OUTPUT
      Serial.println("config.html ist vorhanden = Normalbetrieb");
    #endif
    webserver_normalbetrieb();
  }
  else
  {
    #ifdef DEBUG_SERIAL_OUTPUT
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
<tr><td>ESP- Name :</td><td><input class="setting" type="text" name="esp_name" placeholder="%ph_wifi_esp%"></td></tr>
<tr><td>WiFi - SSID :</td><td><select name="wifi_ssid">%ph_wifi_ssid% %ph_wifi_ssiddisplay%</select></td></tr>
<tr><td>WiFi - Passwort :</td><td><input class="setting" type="text" name="wifi_pw" placeholder="%ph_wifi_pw%"></td></tr>
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
  #ifdef DEBUG_SERIAL_OUTPUT
    Serial.println("Client:" + request->client()->remoteIP().toString() + " " + request->url());
  #endif

  if (!index)
  {
    // open the file on first call and store the file handle in the request object
    request->_tempFile = SPIFFS.open("/" + filename, "w");
    #ifdef DEBUG_SERIAL_OUTPUT
      Serial.println("Upload Start: " + String(filename));
    #endif
  }

  if (len)
  {
    // stream the incoming chunk to the opened file
    request->_tempFile.write(data, len);
    #ifdef DEBUG_SERIAL_OUTPUT
      Serial.println("Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len));
    #endif
  }

  if (final)
  {
    // close the file handle as the upload is now done
    request->_tempFile.close();
    #ifdef DEBUG_SERIAL_OUTPUT
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
  #ifdef DEBUG_SERIAL_OUTPUT
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
    #ifdef DEBUG_SERIAL_OUTPUT
      Serial.println("Client:" + request->client()->remoteIP().toString() + + " " + request->url());
    #endif
    request->send_P(200, "text/html", notbetrieb_html, webserver_call); });

  webserver_config();
}

void webserver_normalbetrieb()
{
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
             { request->send(SPIFFS, "/config.html", String(), false, webserver_call); });
  

  server->on("/config.css", HTTP_GET, [](AsyncWebServerRequest *request)
             { request->send(SPIFFS, "/config.css", "text/css"); });

  webserver_config();
}

String webserver_call(const String &var)
{
  if ( webserver.sperre ) return String();
  #ifdef DEBUG_SERIAL_OUTPUT
    Serial.print("Web-Server call mit : ");
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
 
  return String();
}

void webserver_triger(String name, String msg)
{
  web_response_sys(name,msg);
  webserver_triger_wifi(name, msg);
  webserver_triger_mqtt(name, msg);
  webserver_triger_detector(name, msg);
  webserver_triger_sensor(name, msg);

  if        (name == "navigation")                                          webserver.navigation = msg;
  else if   (name == "config_save" && msg == "Änderungen übernehmen") {     spiffs_config_save();
                                                                            webserver.config = true; }
  else if   (name == "ESP-Neustart")                                        ESP.restart();
  
  if (name == "reset_config") {
    if (msg == "Werkseinstellungen laden !") {
      SPIFFS.remove(safefile);
      #ifdef DEBUG_SERIAL_OUTPUT
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
                    #ifdef DEBUG_SERIAL_OUTPUT
                      Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
                    #endif
                }
                else if (p->isPost())
                {
                    art = "Post";
                    id = p->name().c_str();
                    msg = p->value().c_str();
                    #ifdef DEBUG_SERIAL_OUTPUT
                      Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
                    #endif
                }
                else
                {
                    art = "Get";
                    id = p->name().c_str();
                    msg = p->value().c_str();
                    #ifdef DEBUG_SERIAL_OUTPUT
                      Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
                    #endif
                }
                webserver_triger(id,msg);

                #ifdef DEBUG_SERIAL_OUTPUT
                  Serial.print("MSG : ");
                  Serial.print(art);
                  Serial.print(" | ");
                  Serial.print(id);
                  Serial.print(" | ");
                  Serial.println(msg);
                #endif
            }
            request->send(SPIFFS, "/config.html", String(), false, webserver_call); });
}