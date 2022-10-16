#pragma once

struct WEBSERVER
{
    String navigation;
    String wifi_ssid;
};
extern WEBSERVER webserver;


void webserver_art();
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
String humanReadableSize(const size_t bytes);
String listFiles(bool ishtml);
void webserver_notbetrieb();
void webserver_normalbetrieb();

String webserver_call(const String &var);
void webserver_triger(String name, String msg);

void webserver_config();
