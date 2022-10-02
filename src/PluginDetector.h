#pragma once

struct DETECTOR
{
    int group;
    String alarm_group_safe;
    int alarm_group[99];
    int alarm_group_size;
    String location;
    
};
extern DETECTOR detector;

void detector_mqtt_config();
void alarm_group_diagnose(String msg);

String web_server_detector(const String &var);
void web_server_detector_get(String name, String msg);

void load_conf_detector(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_detector(StaticJsonDocument<1024> doc);