#include "SysHeaders.h"

int mqtt_counter = 0;

///         MQTT Nachrichten Empfangen
void mqtt_read(char *topic, byte *message, unsigned int length)
{
  String messageTemp;
  String topicTemp;

  for (int i = 0; i < length; i++)
  {
    messageTemp += (char)message[i];
  }

  // Subscribe auswerten !
  topicTemp = String(topic);
  mqtt_filter(topicTemp, messageTemp);
}

///         MQTT einrichten
void mqtt_config()
{
  //    Serververbindungen
  Serial.print("MQTT Verbindung mit : ");
  Serial.print(config.mqtt_ip);
  Serial.print(" : ");
  Serial.println(config.mqtt_port);
  client.setServer(config.mqtt_ip.c_str(), config.mqtt_port.toInt());
  client.setCallback(mqtt_read);
}

///         MQTT verbinden
void mqtt_connect()
{
  if (config.mqtt_topic_base == "")
  {
    config.mqtt_topic_base = "Rauchmelder";
  }
  if (config.mqtt_topic_define == "")
  {
    config.mqtt_topic_define = config.esp_name;
  }
  if (WiFi.isConnected() == true &&
      !client.connected())

  {
    // timer_bluetooth = 0;
    led_flash_timer(250, 150, 3);
    Serial.print("MQTT verbinden");
    ///         Connect mit LastWill Message
    if (client.connect(config.esp_name.c_str(), (config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + esp_status + "Online").c_str(), 1, true, "false"))
    {
      mqtt_counter = 0;
      ///         Online Status setzen
      // timer_bluetooth = millis();
      client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + esp_status + "Online").c_str(), "true");
      mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Einbauort", config.detector_location);
      mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Meldegruppe", config.detector_group);
      mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + esp_status + "ESP- ID", config.esp_name);
      mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + esp_status + "Mac Adresse", WiFi.macAddress());
      mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + esp_status + "IP Adresse", WiFi.localIP().toString());

      Serial.println(" / erfolgreich / mit Topic : " + config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/");
      Serial.println("");
      // Subscribe Melder
      // delay(1000);
      client.subscribe((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_control + "Melder_Finden").c_str());
      client.subscribe((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_status + "Komunikation").c_str());
      client.subscribe((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_control + "Reset_Test|Funk_Alarme").c_str());
      client.subscribe((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_control + "Testalarm-Funk").c_str());
      client.subscribe((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_control + "Alarm-Funk").c_str());
      client.subscribe((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_control + "Das_ist_ein_Test").c_str());
      // Subscribe Gruppe
      // delay(1000);
      client.subscribe((config.mqtt_topic_base + "/" + group_control + config.detector_group + "/" + "Melder_Finden").c_str());
      client.subscribe((config.mqtt_topic_base + "/" + group_control + config.detector_group + "/" + "Testalarm").c_str());

      // Subscribe Alarmierungsgruppen
      String topic_temp = "";
      for (int i = 0; i < config.detector_alarm_group_size + 1; i++)
      {
        topic_temp = config.mqtt_topic_base + "/" + group_control + config.detector_alarm_group_int[i] + "/" + "Alarm";
        client.subscribe(topic_temp.c_str());
      }
    }
    else
    {
      mqtt_counter ++;
      Serial.println(" / nicht erfolgreich / neuer Versuch in 5 Sekunden");
      if (mqtt_counter == 12)
      {
        ESP.restart();
      }
    }
  }
}

void mqtt_publish(String topic, String msg)
{
  int topic_size = topic.length();
  int msg_size = msg.length();
  char temp_topic[topic_size];
  char temp_msg[msg_size];
  topic.toCharArray(temp_topic, topic_size + 1);
  msg.toCharArray(temp_msg, msg_size + 1);

  client.publish(temp_topic, temp_msg);
}
