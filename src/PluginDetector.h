#pragma once

#define detector_diagnose "Melder-Diagnose/"
#define detector_status "Melder-Status/"
#define detector_control "Melder-Steuern/"
#define group_control "-Gruppen-Steuerung-/"

struct DETECTOR
{
    int group;
    String alarm_group_safe;
    int alarm_group[99];
    int alarm_group_size;
    String location;
    unsigned long timer = 0;
};
extern DETECTOR detector;

//void detector_mqtt_config();
void alarm_group_diagnose(String msg);

//String web_server_detector(const String &var);
//void web_server_detector_get(String name, String msg);

String web_request_detector(const String &var);
void webserver_triger_detector(String name, String msg);

void load_conf_detector(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_detector(StaticJsonDocument<1024> doc);

//void mqtt_detector_sub_register();
//void mqtt_detector_sub_read(String topic, String msg);



void mqtt_subscribe_detector();
void mqtt_incoming_msg_detector(String topic, String msg);