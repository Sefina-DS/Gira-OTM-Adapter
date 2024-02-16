#pragma once

struct MQTT
{
    boolean aktiv;
    bool configured = false;
    bool subscribe = false;
    String ip;
    String port;
    String topic_base;
    String topic_define;
    String topic;
    int false_number = 0;
    int reconnect_counter = -1;
    unsigned long timer = 0;
    unsigned long timer_status = 0;
    
};
extern MQTT mqtt;
extern PubSubClient client;

void mqtt_publish(String topic, String msg, String funktion);


void mqtt_setup();
void mqtt_reconnect();
bool mqtt_publish (String topic, String msg);
bool mqtt_publish_group (int group, String topic, String msg);
void mqtt_subscribe_list ();
bool mqtt_subscribe (String topic);
bool mqtt_subscribe_group (int group, String topic);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void mqtt_incoming_msg(String topic, String msg);

String web_request_mqtt(const String &var);
void web_response_mqtt(String name, String msg);

void load_conf_mqtt(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_mqtt(StaticJsonDocument<1024> doc);
