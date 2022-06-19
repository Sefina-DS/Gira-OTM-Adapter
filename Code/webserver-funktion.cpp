#include "webserver-funktion.hpp"

String web_server_variablen(const String &var)
{
    String temp = "";
    Serial.println(var);
    if (var == "FILELIST")
    {
        return listFiles(true);
    }
    if (var == "FREESPIFFS")
    {
        return humanReadableSize((SPIFFS.totalBytes() - SPIFFS.usedBytes()));
    }

    if (var == "USEDSPIFFS")
    {
        return humanReadableSize(SPIFFS.usedBytes());
    }

    if (var == "TOTALSPIFFS")
    {
        return humanReadableSize(SPIFFS.totalBytes());
    }
    if (var == "esp_name")
    {
        return config.esp_name;
    }
    if (var == "nav-net")
    {
        if (webserver.navigation == "Netzwerk")
        {
            return "";
        }
        else
        {
            return "display: none;";
        }
    }
    if (var == "nav-net-dhcp")
    {
        if (config.wifi_dhcp)
        {
            return "<br/><div class='dhcp' style=' display : none;'>";
        }
        else
        {
            return "<div class='dhcp'>";
        }
    }
    if (var == "nav-net-mqtt")
    {
        if (!config.mqtt)
        {
            return "<br/><div class='mqtt' style=' display : none;'>";
        }
        else
        {
            return "<div class='mqtt'>";
        }
    }
    if (var == "nav-det")
    {
        if (webserver.navigation == "Rauchmelder")
        {
            return "";
        }
        else
        {
            return "display: none;";
        }
    }
    if (var == "nav-sen")
    {
        if (webserver.navigation == "Sensoren")
        {
            return "";
        }
        else
        {
            return "display: none;";
        }
    }
    if (var == "nav-sen-bme")
    {
        if (!config.bme_280)
        {
            return "<br/><div class='bme280' style=' display : none;'>";
        }
        else
        {
            return "<div class='bme280'>";
        }
    }
    if (var == "nav-sys")
    {
        if (webserver.navigation == "System")
        {
            return "";
        }
        else
        {
            return "display: none;";
        }
    }
    if (var == "place_esp_name")
    {
        temp = "placeholder = '" + config.esp_name + "'";
        return temp;
    }
    if (var == "place_wifi_ssid")
    {
        temp = F("<select name='wifi_ssid'>");
        if (config.wifi_ssid != "")
        {
            temp += F("<option value='");
            temp += config.wifi_ssid;
            temp += F("' selected>");
            temp += config.wifi_ssid;
            temp += F("</option>");
        }
        else
        {
            temp += F("<option selected>keins ausgewählt</option>");
        }
        temp += webserver.wifi_ssid;
        temp += F("</select>");
        return temp;
    }
    if (var == "place_wifi_pw")
    {
        if (config.wifi_pw != "")
        {
            temp = "placeholder = '---FFF---FFF---'";
        }
        else
        {
            temp = "placeholder = 'Bitte eintragen !'";
        }

        return temp;
    }
    if (var == "place_wifi_typ")
    {
        if (config.wifi_dhcp)
        {
            temp = "<option value='dynamisch' selected>dynamisch</option><option value='statisch'</option>statisch";
        }
        else
        {
            temp = "<option value='statisch' selected>statisch</option><option value='dynamisch'</option>dynamisch";
        }

        return temp;
    }
    if (var == "place_wifi_ip")
    {
        temp = "placeholder = '" + config.wifi_ip + "'";
        return temp;
    }
    if (var == "place_wifi_subnet")
    {
        temp = "placeholder = '" + config.wifi_subnet + "'";
        return temp;
    }
    if (var == "place_wifi_gw")
    {
        temp = "placeholder = '" + config.wifi_gw + "'";
        return temp;
    }
    if (var == "place_wifi_dns")
    {
        temp = "placeholder = '" + config.wifi_dns + "'";
        return temp;
    }
    if (var == "place_mqtt")
    {
        if (config.mqtt)
        {
            temp = "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert";
        }
        else
        {
            temp = "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
        }

        return temp;
    }
    if (var == "place_mqtt_ip")
    {
        temp = "placeholder = '" + config.mqtt_ip + "'";
        return temp;
    }
    if (var == "place_mqtt_port")
    {
        temp = "placeholder = '" + config.mqtt_port + "'";
        return temp;
    }
    if (var == "place_mqtt_base")
    {
        temp = "placeholder = '" + config.mqtt_topic_base + "'";
        return temp;
    }
    if (var == "place_mqtt_define")
    {
        temp = "placeholder = '" + config.mqtt_topic_define + "'";
        return temp;
    }
    if (var == "place_detector_location")
    {
        temp = "placeholder = '" + config.detector_location + "'";
        return temp;
    }
    if (var == "place_detector_group")
    {
        temp = "placeholder = '" + config.detector_group + "'";
        return temp;
    }
    if (var == "place_detector_alarm_group")
    {
        String msg_temp = "";
        for (int i = 0; i < config.detector_alarm_group_size; i++)
        {
            msg_temp += config.detector_alarm_group_int[i];
            msg_temp += ";";
        }
        msg_temp += config.detector_alarm_group_int[config.detector_alarm_group_size];
        temp = "placeholder='" + msg_temp + "'";
        return temp;
    }
    if (var == "place_sensor_bme")
    {
        if (config.bme_280)
        {
            temp = "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert";
        }
        else
        {
            temp = "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
        }
        return temp;
    }
    if (var == "place_sensor_bme_temperature")
    {
        if (config.bme_280_temperature)
        {
            temp = "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert";
        }
        else
        {
            temp = "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
        }
        return temp;
    }
    if (var == "place_sensor_bme_humidity")
    {
        if (config.bme_280_humidity)
        {
            temp = "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert";
        }
        else
        {
            temp = "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
        }
        return temp;
    }
    if (var == "place_sensor_bme_pressure")
    {
        if (config.bme_280_pressure)
        {
            temp = "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert";
        }
        else
        {
            temp = "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
        }
        return temp;
    }
    if (var == "place_sensor_bme_high")
    {
        if (config.bme_280_high)
        {
            temp = "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert";
        }
        else
        {
            temp = "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
        }
        return temp;
    }
    if (var == "place_sensor_light")
    {
        if (config.light)
        {
            temp = auswahl_aktiv;
        }
        else
        {
            temp = auswahl_deaktiv;
        }
        return temp;
    }
    if (var == "place_sensor_ubext")
    {
        if (config.ubext)
        {
            temp = auswahl_aktiv;
        }
        else
        {
            temp = auswahl_deaktiv;
        }
        return temp;
    }

    return String();
}

// ------------------------------------------------------------ //

void web_server_get_analyse(String name, String msg)
{
    if (name == "navigation")
    {
        webserver.navigation = msg;
        Serial.println(webserver.navigation);
    }
    if (name == "esp_name")
    {
        if (msg != "")
        {
            config.esp_name = msg;
        }
    }
    if (name == "config_save_restart")
    {
        if (msg == "geänderte Config übertragen und Modul neustarten !")
        {
            spiffs_config_save();
            Serial.println("ESP wird in 3 Sekunden neugestartet !");
            delay(3000);
            ESP.restart();
        }
    }
    if (name == "reset_config")
    {
        if (msg == "Werkseinstellungen laden !")
        {
            SPIFFS.remove(safefile);
            Serial.println("Savefile wurde gelöscht !");
            Serial.println();
            Serial.println("ESP wird in 3 Sekunden neugestartet !");
            delay(3000);
            ESP.restart();
        }
    }
    if (name == "wifi_ssid")
    {
        if (msg != "")
        {
            config.wifi_ssid = msg;
        }
    }
    if (name == "wifi_pw")
    {
        if (msg != "")
        {
            config.wifi_pw = msg;
        }
    }
    if (name == "wifi_dhcp")
    {
        if (msg == "dynamisch")
        {
            config.wifi_dhcp = true;
        }
        else
        {
            config.wifi_dhcp = false;
        }
    }
    if (name == "wifi_ip")
    {
        if (msg != "")
        {
            config.wifi_ip = msg;
        }
    }
    if (name == "wifi_gw")
    {
        if (msg != "")
        {
            config.wifi_gw = msg;
        }
    }
    if (name == "wifi_subnet")
    {
        if (msg != "")
        {
            config.wifi_subnet = msg;
        }
    }
    if (name == "wifi_dns")
    {
        if (msg != "")
        {
            config.wifi_dns = msg;
        }
    }
    if (name == "mqtt")
    {
        if (msg == "aktiviert")
        {
            config.mqtt = true;
        }
        else
        {
            config.mqtt = false;
        }
    }
    if (name == "mqtt_ip")
    {
        if (msg != "")
        {
            config.mqtt_ip = msg;
        }
    }
    if (name == "mqtt_port")
    {
        if (msg != "")
        {
            config.mqtt_port = msg.toInt();
        }
    }
    if (name == "mqtt_base")
    {
        if (msg != "")
        {
            config.mqtt_topic_base = msg;
        }
    }
    if (name == "mqtt_define")
    {
        if (msg != "")
        {
            config.mqtt_topic_define = msg;
        }
    }
    if (name == "detector_location")
    {
        if (msg != "")
        {
            config.detector_location = msg;
        }
    }
    if (name == "detector_group")
    {
        if (msg != "")
        {
            config.detector_group = msg;
            Serial.println("Meldegruppe");
            /*if (msg.length() == 1)
            {
                config.detector_group = "00" + msg;
            }
            if (msg.length() == 2)
            {
                config.detector_group = "0" + msg;
            }*/
        }
    }
    if (name == "detector_alarm_group")
    {
        if (msg != "")
        {
            alarm_group_diagnose(msg);
        }
    }
    if (name == "bme")
    {
        if (msg == "aktiviert")
        {
            config.bme_280 = true;
        }
        else
        {
            config.bme_280 = false;
        }
    }
    if (name == "bme-temperature")
    {
        if (msg == "aktiviert")
        {
            config.bme_280_temperature = true;
        }
        else
        {
            config.bme_280_temperature = false;
        }
    }
    if (name == "bme-humidity")
    {
        if (msg == "aktiviert")
        {
            config.bme_280_humidity = true;
        }
        else
        {
            config.bme_280_humidity = false;
        }
    }
    if (name == "bme-temperature")
    {
        if (msg == "aktiviert")
        {
            config.bme_280_pressure = true;
        }
        else
        {
            config.bme_280_pressure = false;
        }
    }
    if (name == "bme-high")
    {
        if (msg == "aktiviert")
        {
            config.bme_280_high = true;
        }
        else
        {
            config.bme_280_high = false;
        }
    }
    if (name == "light")
    {
        if (msg == "aktiviert")
        {
            config.light = true;
        }
        else
        {
            config.light = false;
        }
    }
    if (name == "ubext")
    {
        if (msg == "aktiviert")
        {
            config.ubext = true;
        }
        else
        {
            config.ubext = false;
        }
    }
}

void webserver_config()
{
    // run handleUpload function when any file is uploaded
    server->on(
        "/upload", HTTP_POST, [](AsyncWebServerRequest *request)
        { request->send(200); },
        handleUpload);

    server->on(
        "/post", HTTP_POST, [](AsyncWebServerRequest *request)
        { request->send(200, "text/plain", "Post route"); });

    server->on(
        "/get", HTTP_GET, [](AsyncWebServerRequest *request)
        {   scan_wifi_ssid();
            String art;
            String id;
            String msg;
            int params = request->params();
            for (int i = 0; i < params; i++)
            {
                AsyncWebParameter *p = request->getParam(i);
                if (p->isPost())
                { 
                    art = "File";
                    id = p->name().c_str();
                    msg = p->value().c_str();
                    Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
                }
                else if (p->isPost())
                {
                    art = "Post";
                    id = p->name().c_str();
                    msg = p->value().c_str();
                    Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
                }
                else
                {
                    art = "Get";
                    id = p->name().c_str();
                    msg = p->value().c_str();
                    Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
                }
                web_server_get_analyse(id,msg);

                Serial.print("MSG : ");
                Serial.print(art);
                Serial.print(" | ");
                Serial.print(id);
                Serial.print(" | ");
                Serial.println(msg);
            }
            request->send(SPIFFS, "/config.html", String(), false, web_server_variablen); });
}