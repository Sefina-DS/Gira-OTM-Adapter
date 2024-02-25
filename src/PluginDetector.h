#pragma once

#define detector_diagnose "Melder-Diagnose/"
#define detector_status "Melder-Status/"
#define detector_control "Melder-Steuern/"
#define group_control "-Gruppen-Steuerung-/"

struct DETECTOR
{
    int group;
    String groups_safestring = "0";
    int alarm_group[99];
    int alarm_group_size;
    String location;
    unsigned long timer = 0;
};
extern DETECTOR detector;

extern std::vector<int> groups;

//void detector_mqtt_config();
void alarm_group_diagnose(String msg); // ALT !

//String web_server_detector(const String &var);
//void web_server_detector_get(String name, String msg);

String web_request_detector(const String &var);
void web_response_detector(String name, String msg);

void load_conf_detector(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_detector(StaticJsonDocument<1024> doc);
void diagnose_groups(String group_string);

void mqtt_subscribe_detector();
void mqtt_incoming_msg_detector(String topic, String msg);

void test(String testtest);
std::vector<int> extractAndSortNumbers(const std::string& input);