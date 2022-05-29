#pragma once
#include "x-header.hpp"

//extern WiFiClient espClient;
extern PubSubClient client;

void mqtt_read(char *topic, byte *message, unsigned int length);
void mqtt_config();
void mqtt_connect();
void mqtt_publish(String topic, String msg);
void mqtt_publish_int(String topic, int msg);
