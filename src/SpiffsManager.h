#pragma once

#define safefile "/config.json"

void spiffs_starten();
void spiffs_format();
void spiffs_scan();
void spiffs_config_save();
void spiffs_config_load();
void spiffs_config_read();

String webserver_call_spiffs(const String &var);

