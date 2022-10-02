#include "SysHeaders.h"

DETECTOR detector;

void detector_mqtt_config()
{
    // subscriben 
    client.subscribe((mqtt.topic_base + "/" + mqtt.topic_define + "/" + detector_control + "Melder_Finden").c_str());
    client.subscribe((mqtt.topic_base + "/" + mqtt.topic_define + "/" + detector_status + "Komunikation").c_str());
    client.subscribe((mqtt.topic_base + "/" + mqtt.topic_define + "/" + detector_control + "Reset_Test|Funk_Alarme").c_str());
    client.subscribe((mqtt.topic_base + "/" + mqtt.topic_define + "/" + detector_control + "Testalarm-Funk").c_str());
    client.subscribe((mqtt.topic_base + "/" + mqtt.topic_define + "/" + detector_control + "Alarm-Funk").c_str());
    client.subscribe((mqtt.topic_base + "/" + mqtt.topic_define + "/" + detector_control + "Das_ist_ein_Test").c_str());
    client.subscribe((mqtt.topic_base + "/" + group_control + config.detector_group + "/" + "Melder_Finden").c_str());
    client.subscribe((mqtt.topic_base + "/" + group_control + config.detector_group + "/" + "Testalarm").c_str());

    // Subscribe Alarmierungsgruppen
    String topic_temp = "";
    for (int i = 0; i < config.detector_alarm_group_size + 1; i++)
    {
        topic_temp = mqtt.topic_base + "/" + group_control + config.detector_alarm_group_int[i] + "/" + "Alarm";
        client.subscribe(topic_temp.c_str());
    }
}

String web_server_sensor(const String &var)
{
    String temp = "";
    if (var == "nav-sen-bme")
    {
        if (!sensor.bme280)
        {
            return "<br/><div class='bme280' style=' display : none;'>";
        }
        else
        {
            return "<div class='bme280'>";
        }
    }
    if (var == "place_sensor_bme")
    {
        if (sensor.bme280)
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
        if (sensor.bme280_temperature)
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
        if (sensor.bme280_humidity)
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
        if (sensor.bme280_pressure)
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
        if (sensor.bme280_high)
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
        if (sensor.light)
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
        if (sensor.ubext)
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

void web_server_sensor_get(String name, String msg)
{
    if (name == "bme")
    {
        if (msg == "aktiviert")
        {
            sensor.bme280 = true;
        }
        else
        {
            sensor.bme280 = false;
        }
    }
    if (name == "bme-temperature")
    {
        if (msg == "aktiviert")
        {
            sensor.bme280_temperature = true;
        }
        else
        {
            sensor.bme280_temperature = false;
        }
    }
    if (name == "bme-humidity")
    {
        if (msg == "aktiviert")
        {
            sensor.bme280_humidity = true;
        }
        else
        {
            sensor.bme280_humidity = false;
        }
    }
    if (name == "bme-temperature")
    {
        if (msg == "aktiviert")
        {
            sensor.bme280_pressure = true;
        }
        else
        {
            sensor.bme280_pressure = false;
        }
    }
    if (name == "bme-high")
    {
        if (msg == "aktiviert")
        {
            sensor.bme280_high = true;
        }
        else
        {
            sensor.bme280_high = false;
        }
    }
    if (name == "light")
    {
        if (msg == "aktiviert")
        {
            sensor.light = true;
        }
        else
        {
            sensor.light = false;
        }
    }
    if (name == "ubext")
    {
        if (msg == "aktiviert")
        {
            sensor.ubext = true;
        }
        else
        {
            sensor.ubext = false;
        }
    }
}

void load_conf_detector(StaticJsonDocument<1024> doc)
{
    Serial.println("... Detector- Variablen ...");

    String temp;
    
    detector.group = doc["detector_group"] | "0";
    temp = doc["detector_alarm_group"] | "0";
    detector.location = doc["detector_location"] | "";

    alarm_group_diagnose(temp);
}

StaticJsonDocument<1024> safe_conf_detector(StaticJsonDocument<1024> doc)
{
    Serial.println("... Detector- Variablen ...");
    
    String temp;
    
    for (int i = 0; i < detector.alarm_group_size; i++)
    {
        temp += detector.alarm_group[i];
        temp += ";";
    }
    temp += detector.alarm_group[detector.alarm_group_size];
    
    doc["detector_group"] = detector.group;
    doc["detector_alarm_group"] = temp;
    doc["detector_location"] = detector.location;

    return doc;
}

void alarm_group_diagnose(String msg)
{
    String number_str = "";
    int number = 999999;
    int size = 0;
    boolean mg = false;

    for (int i = 0; msg[i] != 0; i++)
    {
        if (msg[i] > 47 &&
            msg[i] < 58)
        {
            if (number == 999999)
            {
                number_str = String(msg[i] - 48);
                number = msg[i] - 48;
            }
            else
            {
                number_str += String(msg[i] - 48);
            }
        }
        else
        {
            if (number >= 0 &&
                number != 999999)
            {
                if (msg[i + 1] != 0)
                {
                    detector.alarm_group[size] = number_str.toInt();
                    if (detector.group == number_str)
                    {
                        mg = true;
                    }
                    size++;
                    number = 999999;
                }
            }
        }
    }
    detector.alarm_group[size] = number_str.toInt();
    if (mg == false &&
        detector.group != number_str)
    {
        size++;
        detector.alarm_group[size] = detector.group;
    }
    detector.alarm_group_size = size;
    
    Serial.print("Alarmierungsgruppen : ");
    for (int i = 0; i < detector.alarm_group_size + 1; i++)
    {
        Serial.print(detector.alarm_group[i]);
        Serial.print(", ");
    }
}