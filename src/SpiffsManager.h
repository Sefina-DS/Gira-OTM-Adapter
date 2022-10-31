#pragma once

#define safefilea "/config_a.json"
#define safefileb "/config_b.json"
#define safefile_alt "/config.json"

void spiffs_starten();
void spiffs_format();
void spiffs_scan();
void spiffs_config_save_part_a();
void spiffs_config_save_part_b();
void spiffs_config_load_part_a();
void spiffs_config_load_part_b();
void spiffs_config_read_part_a();
void spiffs_config_read_part_b();

String webserver_call_spiffs(const String &var);
void webserver_triger_spiffs(String name, String msg);

