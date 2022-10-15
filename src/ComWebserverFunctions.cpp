#include "SysHeaders.h"

String web_server_variablen(const String &var)
{
    String temp = "";
    Serial.println(var);

    // Bluetooth
    temp = web_server_bluetooth(var);
    if (temp != "")
    {
        return temp;
    }
    
    // Sensoren
    temp = web_server_sensor(var);
    if (temp != "")
    {
        return temp;
    }
    
    // WIFI
    temp = web_server_wifi(var);
    if (temp != "")
    {
        return temp;
    }
    
    // MQTT
    temp = web_server_mqtt(var);
    if (temp != "")
    {
        return temp;
    }


    
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

    return String();
}

// ------------------------------------------------------------ //

void web_server_get_analyse(String name, String msg)
{
    web_server_bluetooth_get(name, msg);
    web_server_sensor_get(name, msg);
    web_server_wifi_get(name, msg);

    if (name == "navigation")
    {
        webserver.navigation = msg;
        Serial.println(webserver.navigation);
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