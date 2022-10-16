#pragma once

struct MQTT
{
    boolean aktiv;
    String ip;
    String port;
    String topic_base;
    String topic_define;
    
};
extern MQTT mqtt;
extern PubSubClient client;

void mqtt_read(char *topic, byte *message, unsigned int length);
void mqtt_config();
void mqtt_connect();
void mqtt_publish(String topic, String msg);
void mqtt_publish_int(String topic, int msg);

String webserver_call_mqtt(const String &var);
void webserver_triger_mqtt(String name, String msg);

void load_conf_mqtt(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_mqtt(StaticJsonDocument<1024> doc);



