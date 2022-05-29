#include "webserver.hpp"

void webserver_art()
{
  if (SPIFFS.exists("/config.html"))
  {
    Serial.println("config.html ist vorhanden = Normalbetrieb");
    webserver_normalbetrieb();
  }
  else
  {
    Serial.println("config.html ist nicht vorhanden = Notbetrieb");
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
<table>
<tr><th><h1>! Notbetrieb !</h1><br>! File upload !</th></tr>
<tr><td align=center height=50>nur zum upload der config.html + config.css nutzen !</td></tr>
<tr><td align=center height=50>Free Storage: %FREESPIFFS% | Used Storage: %USEDSPIFFS% | Total Storage: %TOTALSPIFFS%</td></tr>
<tr><td align=center height=25><form method="POST" action="/upload" enctype="multipart/form-data"><input type="file" name="data"/></td></tr>
<tr><td align=center height=25><input type="submit" name="upload" value="Upload" title="Upload File"></form></td></tr>
<tr><td align=center height=50>Auf dem ESP vorhandene Daten :</td></tr>
<tr><td align=center>%FILELIST%</td></tr>
</table>
</center>
</body>
</html>
)rawliteral";

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
  Serial.println(logmessage);

  if (!index)
  {
    logmessage = "Upload Start: " + String(filename);
    // open the file on first call and store the file handle in the request object
    request->_tempFile = SPIFFS.open("/" + filename, "w");
    Serial.println(logmessage);
  }

  if (len)
  {
    // stream the incoming chunk to the opened file
    request->_tempFile.write(data, len);
    logmessage = "Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len);
    Serial.println(logmessage);
  }

  if (final)
  {
    logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
    // close the file handle as the upload is now done
    request->_tempFile.close();
    Serial.println(logmessage);
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
  Serial.println("Listing files stored on SPIFFS");
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
    String logmessage = "Client:" + request->client()->remoteIP().toString() + + " " + request->url();
    Serial.println(logmessage);
    request->send_P(200, "text/html", notbetrieb_html, web_server_variablen); });

  webserver_config();
}

void webserver_normalbetrieb()
{
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
             { request->send(SPIFFS, "/config.html", String(), false, web_server_variablen); });

  server->on("/config.css", HTTP_GET, [](AsyncWebServerRequest *request)
             { request->send(SPIFFS, "/config.css", "text/css"); });

  webserver_config();
}