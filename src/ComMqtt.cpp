#include "SysHeaders.h"

MQTT mqtt;

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

  #ifdef DEBUG_SERIAL_OUTPUT
  	Serial.print("Ankomende Nachricht / das Topic : ");
    Serial.print(topicTemp);
    Serial.print(" || die Message : ");
    Serial.println(messageTemp);
  #endif

  // Subscriber übergeben
  mqtt_mqtt_sub_read(topicTemp, messageTemp);
  mqtt_detector_sub_read(topicTemp, messageTemp);

}

///         MQTT einrichten
void mqtt_config()
{
  //    Serververbindungen
  #ifdef DEBUG_SERIAL_OUTPUT
    Serial.print("MQTT Verbindung mit : ");
    Serial.print(mqtt.ip);
    Serial.print(" : ");
    Serial.println(mqtt.port);
  #endif
  client.setServer(mqtt.ip.c_str(), mqtt.port.toInt());
  client.setCallback(mqtt_read);
  mqtt_connect();
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
    #ifdef DEBUG_SERIAL_OUTPUT
      Serial.print("MQTT verbinden");
    #endif
    String temp_topic = mqtt.topic_base + "/" + mqtt.topic_define + "/" + "ESP/";
    ///         Connect mit LastWill Message
    if (client.connect(wifi.esp_name.c_str(), (temp_topic + "Online").c_str(), 1, true, "false"))
    {
      mqtt.false_number = 0;
      mqtt.reconnect_counter ++;
      ///         Online Status setzen
      // timer_bluetooth = millis();
      mqtt_publish(temp_topic + "Online", "true", "mqtt_connect");
      mqtt_publish(temp_topic + "Firmwareupdate", "false", "mqtt_connect");
      
      mqtt_esp_status();
      
      #ifdef DEBUG_SERIAL_OUTPUT
        Serial.println(" / erfolgreich / mit Topic : " + mqtt.topic_base + "/" + mqtt.topic_define + "/");
        Serial.println("");
      #endif
      
      // Subscriben
      mqtt_mqtt_sub_register();
      mqtt_detector_sub_register();

      //detector_mqtt_config();

      
    }
    else
    {
      mqtt.false_number ++;
      #ifdef DEBUG_SERIAL_OUTPUT
        Serial.print(" / nicht erfolgreich / neuer Versuch in 5 Sekunden / False-Counter : ");
        Serial.println(mqtt.false_number);
      #endif
      if (mqtt.false_number == 12)
      {
        ESP.restart();
      }
    }
  }
}

void mqtt_publish(String topic, String msg, String funktion)
{
  if (!client.connected()) return;
  int topic_size = topic.length();
  int msg_size = msg.length();
  /*char temp_topic[topic_size];
  char temp_msg[msg_size];
  topic.toCharArray(temp_topic, topic_size + 1);
  msg.toCharArray(temp_msg, msg_size + 1);*/

  const char* temp_topic =  topic.c_str();
  const char* temp_msg =  msg.c_str();
  
  #ifdef DEBUG_SERIAL_OUTPUT
    Serial.print("MQTT Send - Funktion : ");
    Serial.print(funktion);
    Serial.print(" || Topic (");
    Serial.print(topic_size);
    Serial.print(") : ");
    Serial.print(temp_topic);
    Serial.print(" / ");
    Serial.print(topic);
    Serial.print(" || Msg (");
    Serial.print(msg_size);
    Serial.print(") : ");
    Serial.println(temp_msg);
  #endif

  client.publish(temp_topic, temp_msg);
}

String web_request_mqtt(const String &var) {
    if        ( var == "ph_mqtt_aktivdisplay" )     { return ( !mqtt.aktiv )    ? "display: none; "
                                                                                : "";
    } else if ( var == "ph_mqtt_aktiv")            { return ( mqtt.aktiv )      ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if ( var == "ph_mqtt_ip")                { return  mqtt.ip;
    } else if ( var == "ph_mqtt_port")              { return  mqtt.port;
    } else if ( var == "ph_mqtt_base")              { return  mqtt.topic_base;
    } else if ( var == "ph_mqtt_define")            { return  mqtt.topic_define;
    }

    return String();
}

void webserver_triger_mqtt(String name, String msg)
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
    #ifdef DEBUG_SERIAL_OUTPUT
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
    #ifdef DEBUG_SERIAL_OUTPUT
      Serial.println("... MQTT- Variablen ...");
    #endif
    doc["mqtt"] = mqtt.aktiv;
    doc["mqtt_ip"] = mqtt.ip;
    doc["mqtt_port"] = mqtt.port;
    doc["mqtt_topic_base"] = mqtt.topic_base;
    doc["mqtt_topic_define"] = mqtt.topic_define;

    return doc;
}

void mqtt_mqtt_sub_register()
{
  String temp = mqtt.topic_base + "/" + mqtt.topic_define + "/ESP/";
  client.subscribe((temp + "Neustart-ESP").c_str());
  mqtt_publish(temp + "Neustart-ESP", "false", "mqtt_mqtt_sub_read");
  client.subscribe((temp + "Firmwareupdate").c_str());
  mqtt_publish(temp + "Firmwareupdate", "false", "mqtt_mqtt_sub_read");

}

void mqtt_mqtt_sub_read(String topic, String msg)
{
  String temp;
  temp = mqtt.topic_base + "/" + mqtt.topic_define + "/ESP/";
  
  if ( topic == temp + "Neustart-ESP" && msg == "true" )
  {
    mqtt_publish(temp + "Neustart-ESP", "false", "mqtt_mqtt_sub_read");
    delay(3000);
    ESP.restart();
  }
  if ( topic == temp + "Firmwareupdate" && msg == "true" )
  {
    delay(1000);
    firmwareupdate_http();
  }

}

void mqtt_esp_status()
{
  StaticJsonDocument<1024> temp_json;
  String temp_string;
  
  temp_json["ESP-ID"]              = wifi.esp_name;
  temp_json["Version-Alt"]         = system_funktion.version_old;
  temp_json["updatebar"]           = system_funktion.new_version;
  temp_json["ESP-ID"]              = wifi.esp_name;
  temp_json["Mac-Adresse"]         = WiFi.macAddress();
  temp_json["IP-Adresse"]          = WiFi.localIP().toString();
  temp_json["Reconnect"]           = String(mqtt.reconnect_counter);
  temp_json["Betriebsstunden"]     = String(((millis() / 1000) / 60) / 60);

  serializeJson(temp_json, temp_string);
  mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + "ESP" + "/" + "Status", temp_string, "mqtt_esp_status");

  mqtt.timer = millis() + 300000 ;
}
