#include "SysHeaders.h"

MQTT mqtt;

void mqtt_publish(String topic, String msg, String funktion)
{
}

void mqtt_incoming_msg(String topic, String msg){
  if ( topic == mqtt.topic + "ESP/Neustart-ESP"         && msg == "true")     ESP.restart();
  if ( topic == mqtt.topic + "ESP/Firmwareupdate"       && msg == "true")     firmwareupdate_http();
  
  
}

void mqtt_setup(){
  #ifdef DEBUG_SERIAL_MQTT
    Serial.print("MQTT Verbindung mit : ");
    Serial.print(mqtt.ip);
    Serial.print(" : ");
    Serial.println(mqtt.port);
  #endif
  IPAddress server_ip;
  server_ip.fromString(mqtt.ip);
  int server_port = mqtt.port.toInt();
  client.setServer(server_ip, server_port);
  client.setCallback(mqtt_callback);
  mqtt.configured = true ;
}

void mqtt_reconnect() {
  static int fail_mqtt = 0;
  if ( !client.connected() ) {
    if ( millis() >= mqtt.timer ) {
      #ifdef DEBUG_SERIAL_MQTT
      Serial.print("Aufbauende MQTT verbindung ... ");
      #endif
      const char* mqtt_client = wifi.esp_name.c_str();
      mqtt.topic = mqtt.topic_base + "/" + mqtt.topic_define + "/";
      String mqtt_topic_str = mqtt.topic + "ESP/Online" ;
      const char* mqtt_topic = mqtt_topic_str.c_str();
      int mqtt_qos = 1;
      bool mqtt_retain = true;
      const char* mqtt_msg = "false";
      if (client.connect(mqtt_client, mqtt_topic, mqtt_qos, mqtt_retain, mqtt_msg)) {
        #ifdef DEBUG_SERIAL_MQTT
          Serial.println("connected (" + wifi.esp_name + ")" );
        #endif
        mqtt_publish("ESP/Online", "true");
        mqtt_publish("ESP/Neustart-ESP", "false");
        mqtt_publish("ESP/Firmwareupdate", "false");
        mqtt_subscribe_list ();
        mqtt.reconnect_counter ++;
      } else {
        #ifdef DEBUG_SERIAL_MQTT
          Serial.print("failed, rc=");
          Serial.print(client.state());
          Serial.println(" try again in 5 seconds");
        #endif
        fail_mqtt ++;
        mqtt.timer = millis() + 5000;
        if ( fail_mqtt == 6 ) ESP.restart();
      }
    }
  } else {
    client.loop();
    fail_mqtt = 0;
    if ( millis() >= mqtt.timer_status ) {
      StaticJsonDocument<1024> temp_json;
      String temp_string;
      temp_json["ESP-ID"]              = wifi.esp_id;
      temp_json["Version-Alt"]         = system_funktion.version_old;
      temp_json["updatebar"]           = system_funktion.new_version;
      temp_json["ESP-Name"]            = wifi.esp_name;
      temp_json["Mac-Adresse"]         = WiFi.macAddress();
      temp_json["IP-Adresse"]          = WiFi.localIP().toString();
      temp_json["Reconnect"]           = String(mqtt.reconnect_counter);
      temp_json["Betriebsstunden"]     = String(((millis() / 1000) / 60) / 60);
      serializeJson(temp_json, temp_string);
      mqtt_publish("ESP/Status", temp_string);
      mqtt.timer_status = millis() + 300000;
    }
  }
}

bool mqtt_publish (String topic, String msg){
  if (!client.connected()) {
    return false;
  } else {
    String temp_topic_str = mqtt.topic + topic;
    const char* temp_topic =  temp_topic_str.c_str();
    const char* temp_msg =  msg.c_str();
    #ifdef DEBUG_SERIAL_MQTT
      Serial.print("Mqtt-publish ... Topic : ");
      Serial.print(temp_topic);
      Serial.print(" ... Message : ");
      Serial.println(temp_msg);
    #endif
    client.publish(temp_topic, temp_msg);
    return true;
  }
}

bool mqtt_publish_group (int group, String topic, String msg){
  if (!client.connected()) {
    return false;
  } else {
    String temp_topic_str = mqtt.topic_base + "/0-Gruppen-Steuerung-0/" + group + "/" + topic;
    const char* temp_topic =  temp_topic_str.c_str();
    const char* temp_msg =  msg.c_str();
    #ifdef DEBUG_SERIAL_MQTT
      Serial.print("Mqtt-publish ... Topic : ");
      Serial.print(temp_topic);
      Serial.print(" ... Message : ");
      Serial.println(temp_msg);
    #endif
    client.publish(temp_topic, temp_msg);
    return true;
  }
}

void mqtt_subscribe_list (){
  if (client.connected()) {
    mqtt_subscribe("ESP/Neustart-ESP");
    mqtt_subscribe("ESP/Firmwareupdate");

    if ( comserial.aktiv ) mqtt_subscribe_detector();
  }
}

bool mqtt_subscribe (String topic){
  if (client.connected()) {
    String temp_topic = mqtt.topic + topic;
    client.subscribe(temp_topic.c_str());
    #ifdef DEBUG_SERIAL_MQTT
      Serial.print("Mqtt-subscribe ... Topic : ");
      Serial.println(temp_topic);
    #endif
    return true;
  } else {
    return false;
  }
}

bool mqtt_subscribe_group (int group, String topic){
  if (client.connected()) {
    String temp_topic = mqtt.topic_base + "/0-Gruppen-Steuerung-0/" + group + "/" + topic;
    client.subscribe(temp_topic.c_str());
    #ifdef DEBUG_SERIAL_MQTT
      Serial.print("Mqtt-subscribe ... Topic : ");
      Serial.println(temp_topic);
    #endif
    return true;
  } else {
    return false;
  }
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String temp_topic = topic ;
  String temp_msg;
  for (int i = 0; i < length; i++) {
    temp_msg += (char)payload[i];
  }
  #ifdef DEBUG_SERIAL_MQTT
      Serial.print("Mqtt-incoming ... Topic : ");
      Serial.print(temp_topic);
      Serial.print(" ... Message : ");
      Serial.println(temp_msg);
  #endif
  mqtt_incoming_msg(temp_topic, temp_msg);
  if (comserial.aktiv) mqtt_incoming_msg_detector(temp_topic, temp_msg);
}

String web_request_mqtt(const String &var) {
  if          (var == "button_mqtt")       { return (mqtt.aktiv)            ? "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'>deaktiviert</option>"
                                                                            : "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'>aktiviert</option>";
  } else if   (var == "display_mqtt")      { return (mqtt.aktiv)            ? ""
                                                                            : "style='display: none'";
  } else if   ( var == "textarea_mqtt_ip")                { return  mqtt.ip;
  } else if   ( var == "textarea_mqtt_port")              { return  mqtt.port;
  } else if   ( var == "textarea_mqtt_base")              { return  mqtt.topic_base;
  } else if   ( var == "textarea_mqtt_define")            { return  mqtt.topic_define;
  }

    return String();
}

void web_response_mqtt(String name, String msg)
{
  if (name == "mqtt" && msg == "aktiviert")     mqtt.aktiv = true;
  if (name == "mqtt" && msg == "deaktiviert")   mqtt.aktiv = false;
  if (name == "mqtt_ip" && msg != "" )          mqtt.ip = msg;
  if (name == "mqtt_port" && msg != "" )        mqtt.port = msg.toInt();
  if (name == "mqtt_base" && msg != "" )        mqtt.topic_base = msg;
  if (name == "mqtt_define" && msg != "" )      mqtt.topic_define = msg;

}

void load_conf_mqtt(StaticJsonDocument<1024> doc)
{
    #ifdef DEBUG_SERIAL_MQTT
      Serial.println("... MQTT- Variablen ...");
    #endif
    mqtt.aktiv = doc["mqtt"] | false;
    mqtt.ip = doc["mqtt_ip"] | "x-x-x-x";
    mqtt.port = doc["mqtt_port"] | "1883";
    mqtt.topic_base = doc["mqtt_topic_base"] | "Rauchmelder";
    mqtt.topic_define = doc["mqtt_topic_define"] | "";
}

StaticJsonDocument<1024> safe_conf_mqtt(StaticJsonDocument<1024> doc)
{
    #ifdef DEBUG_SERIAL_MQTT
      Serial.println("... MQTT- Variablen ...");
    #endif
    doc["mqtt"] = mqtt.aktiv;
    doc["mqtt_ip"] = mqtt.ip;
    doc["mqtt_port"] = mqtt.port;
    doc["mqtt_topic_base"] = mqtt.topic_base;
    doc["mqtt_topic_define"] = mqtt.topic_define;

    return doc;
}
