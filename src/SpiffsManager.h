#pragma once

#define safefile "/config.json"
#define LOG_FILE_PATH "/log.txt"
#define MAX_LINES 20

void spiffs_starten();
void spiffs_format();
void spiffs_scan();
void spiffs_config_save();
void spiffs_config_load();
void spiffs_config_read();

String web_request_spiff(const String &var);

void log_write(String msg);
