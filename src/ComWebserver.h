#pragma once

struct WEBSERVER
{
    String navigation;
    String wifi_ssid;
    boolean sperre = false;
    boolean config = false;
    unsigned long timer = 0;
    bool notbetrieb = true;
};
extern WEBSERVER webserver;

extern AsyncWebServer *server;
extern const char *PARAM_MESSAGE;
extern const char notbetrieb_html[] PROGMEM;
extern String processor(const String& var);


void webserver_art();
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
String humanReadableSize(const size_t bytes);
String listFiles(bool ishtml);
void webserver_notbetrieb();
void webserver_normalbetrieb();
String loadFileContent(const char *filePath);

String web_request(const String &var);
void web_response_GET(String name, String value);

void webserver_config();


void webserver_setup();
void webserver_file(AsyncWebServerRequest *request, String path, String contentType);
