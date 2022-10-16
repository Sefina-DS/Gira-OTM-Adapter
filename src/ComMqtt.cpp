#include "SysHeaders.h"

MQTT mqtt;

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
  Serial.print(mqtt.ip);
  Serial.print(" : ");
  Serial.println(mqtt.port);
  client.setServer(mqtt.ip.c_str(), mqtt.port.toInt());
  client.setCallback(mqtt_read);
}

///         MQTT verbinden
void mqtt_connect()
{
  if (mqtt.topic_base == "")
  {
    mqtt.topic_base = "Rauchmelder";
  }
  if (mqtt.topic_define == "")
  {
    mqtt.topic_define = wifi.esp_name;
  }
  if (WiFi.isConnected() == true &&
      !client.connected())

  {
    // timer_bluetooth = 0;
    led_flash_timer(250, 150, 3);
    Serial.print("MQTT verbinden");
    ///         Connect mit LastWill Message
    if (client.connect(wifi.esp_name.c_str(), (mqtt.topic_base + "/" + mqtt.topic_define + "/" + esp_status + "Online").c_str(), 1, true, "false"))
    {
      mqtt_counter = 0;
      ///         Online Status setzen
      // timer_bluetooth = millis();
      client.publish((mqtt.topic_base + "/" + mqtt.topic_define + "/" + esp_status + "Online").c_str(), "true");
      mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + detector_status + "Einbauort", config.detector_location);
      mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + detector_status + "Meldegruppe", config.detector_group);
      mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + esp_status + "ESP- ID", wifi.esp_name);
      mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + esp_status + "Mac Adresse", WiFi.macAddress());
      mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + esp_status + "IP Adresse", WiFi.localIP().toString());

      Serial.println(" / erfolgreich / mit Topic : " + mqtt.topic_base + "/" + mqtt.topic_define + "/");
      Serial.println("");
      
      // Subscriben

      detector_mqtt_config();
      
      /*
      
      // Subscribe Melder
      // delay(1000);
      client.subscribe((mqtt.topic_base + "/" + mqtt.topic_define + "/" + detector_control + "Melder_Finden").c_str());
      client.subscribe((mqtt.topic_base + "/" + mqtt.topic_define + "/" + detector_status + "Komunikation").c_str());
      client.subscribe((mqtt.topic_base + "/" + mqtt.topic_define + "/" + detector_control + "Reset_Test|Funk_Alarme").c_str());
      client.subscribe((mqtt.topic_base + "/" + mqtt.topic_define + "/" + detector_control + "Testalarm-Funk").c_str());
      client.subscribe((mqtt.topic_base + "/" + mqtt.topic_define + "/" + detector_control + "Alarm-Funk").c_str());
      client.subscribe((mqtt.topic_base + "/" + mqtt.topic_define + "/" + detector_control + "Das_ist_ein_Test").c_str());
      // Subscribe Gruppe
      // delay(1000);
      client.subscribe((mqtt.topic_base + "/" + group_control + config.detector_group + "/" + "Melder_Finden").c_str());
      client.subscribe((mqtt.topic_base + "/" + group_control + config.detector_group + "/" + "Testalarm").c_str());

      // Subscribe Alarmierungsgruppen
      String topic_temp = "";
      for (int i = 0; i < config.detector_alarm_group_size + 1; i++)
      {
        topic_temp = mqtt.topic_base + "/" + group_control + config.detector_alarm_group_int[i] + "/" + "Alarm";
        client.subscribe(topic_temp.c_str());
      }
      */
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

String webserver_call_mqtt(const String &var)
{
  String temp = "";
  if (var == "web_network_mqtt")
  {
    temp += F("<br/><div class='network'>");
    temp += F("<h3>MQTT Einstellungen</h3>");
    temp += F("<form action='/get'>");
    temp += F("<table>");
    // MQTT Aktiv/Deaktiv
    temp += F("<tr><td>MQTT :</td>");
    temp += F("<td><select name='mqtt'><option value='");
    if (mqtt.aktiv)
    {
      temp += F("aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert");
    }
    else
    {
      temp += F("deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert");
    }
    temp += F("</select>");
    temp += F("</td></tr>");
    temp += F("</table>");
    // Verdeckte Einstellungen (MQTT Aktiv)
    temp += F("<table ");
    if (!mqtt.aktiv)
    {
      temp += F("style='display: none'>");
    }
    else
    {
      temp += F(">");
    }
      // MQTT Broker
      temp += F("<tr><td>IP Broker :</td>");
      temp += F("<td><input class='setting' type='text' name='mqtt_ip' placeholder='");
      temp +=     mqtt.ip;
      temp += F("'></td></tr>");
      // Port Broker
      temp += F("<tr><td>Port Broker :</td>");
      temp += F("<td><input class='setting' type='text' name='mqtt_port' placeholder='");
      temp +=     mqtt.port;
      temp += F("'></td></tr>");
      // Erklärung Topic
      temp += F("<tr><td colspan='2'> MQTT - Topic <br>");
      temp += F("Achtung : Grund- Topic / Melder- Topic / <br>");
      temp += F("der Aufbau darf nicht geändert werden ! <br></tr></td>");
      // Grund Topic
      temp += F("<tr><td>Grund- Topic :</td>");
      temp += F("<td><input class='setting' type='text' name='mqtt_base' placeholder='");
      temp +=     mqtt.topic_base;
      temp += F("'></td></tr>");
      // Melder Topic
      temp += F("<tr><td>Melder- Topic :</td>");
      temp += F("<td><input class='setting' type='text' name='mqtt_define' placeholder='");
      temp +=     mqtt.topic_define;
      temp += F("'></td></tr>");
    temp += F("</table><br/>");
    temp += F("<input type='submit' value='Submit' />");
    temp += F("</form></div>");
    return temp;
  }
  
  return String();
}

void webserver_triger_mqtt(String name, String msg)
{
if (name == "mqtt")
    {
        if (msg == "aktiviert")
        {
            mqtt.aktiv = true;
        }
        else
        {
            mqtt.aktiv = false;
        }
    }
    if (name == "mqtt_ip")
    {
        if (msg != "")
        {
            mqtt.ip = msg;
        }
    }
    if (name == "mqtt_port")
    {
        if (msg != "")
        {
            mqtt.port = msg.toInt();
        }
    }
    if (name == "mqtt_base")
    {
        if (msg != "")
        {
            mqtt.topic_base = msg;
        }
    }
    if (name == "mqtt_define")
    {
        if (msg != "")
        {
            mqtt.topic_define = msg;
        }
    }
}

void load_conf_mqtt(StaticJsonDocument<1024> doc)
{
    Serial.println("... MQTT- Variablen ...");
    
    mqtt.aktiv = doc["mqtt"] | false;
    mqtt.ip = doc["mqtt_ip"] | "x-x-x-x";
    mqtt.port = doc["mqtt_port"] | "1883";
    mqtt.topic_base = doc["mqtt_topic_base"] | "Rauchmelder";
    mqtt.topic_define = doc["mqtt_topic_define"] | "";
}

StaticJsonDocument<1024> safe_conf_mqtt(StaticJsonDocument<1024> doc)
{
    Serial.println("... MQTT- Variablen ...");
    
    doc["mqtt"] = mqtt.aktiv;
    doc["mqtt_ip"] = mqtt.ip;
    doc["mqtt_port"] = mqtt.port;
    doc["mqtt_topic_base"] = mqtt.topic_base;
    doc["mqtt_topic_define"] = mqtt.topic_define;

    return doc;
}