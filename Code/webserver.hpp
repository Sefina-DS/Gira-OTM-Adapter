#pragma once
#include "x-header.hpp"

void webserver_art();
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);
String humanReadableSize(const size_t bytes);
String listFiles(bool ishtml);
void webserver_notbetrieb();
void webserver_normalbetrieb();
