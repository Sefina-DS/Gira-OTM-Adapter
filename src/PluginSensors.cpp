#include "SysHeaders.h"

SENSOR sensor;
Adafruit_BME280 bme;
bool bme_run;

void bme_refresh()
{
    bme_run = bme.begin(0x76);
    if (!bme_run)
    {
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + extension + ext_sensor + "Status").c_str(), "Störung");
        Serial.println("BME-240 ist in Störung");
    }
    else
    {
        float temp_float;
        char temp_msg[8];
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + extension + ext_sensor + "Status").c_str(), "Läuft");
        if (sensor.bme280_temperature)
        {
            temp_float = bme.readTemperature();
            mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + extension + ext_sensor + "Temperatur", String(temp_float));
        }
        if (sensor.bme280_pressure)
        {
            temp_float = bme.readPressure() / 100.0F;
            mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + extension + ext_sensor + "Druck", String(temp_float));
        }
        if (sensor.bme280_humidity)
        {
            temp_float = bme.readHumidity();
            mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + extension + ext_sensor + "Feuchte", String(temp_float));
        }
        if (sensor.bme280_high)
        {
            temp_float = bme.readAltitude(SEALEVELPRESSURE_HPA);
            mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + extension + ext_sensor + "Meter über Meeresspiegel", String(temp_float));
        }
    }
}

void light_refresh()
{
    String topic;
    String msg;
    int temp;
    static int temp_light = 0;
    temp = analogRead(input_light);
    if (temp < temp_light)
    {
        temp = temp_light - temp;
    }
    else
    {
        temp = temp - temp_light;
    }
    if (temp < 0)
    {
        temp = -temp;
    }
    if (temp >= 200)
    {
        temp_light = analogRead(input_light);
        mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + extension + ext_light + "Lichtzahl", String(temp_light));
        temp = temp_light - 4095;
        if (temp != 0)
        {
            temp = -temp;
        }
        temp = (temp * 100) / 4095;
        mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + extension + ext_light + "Lichtwert", String(temp));
    }
}

void ubext_refresh()
{
    int R1 = 10490;
    int R2 = 2140;
    int faktor = 1026;
    float temp_roh = analogRead(input_ubext);
    float temp_ubext = ((temp_roh * 100000) / faktor) / ((100000 * R2) / (R1 + R2));
    mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + extension + ext_ubext + "Spannungsversorgung Extern", String(temp_ubext));
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

void load_conf_sensor(StaticJsonDocument<1024> doc)
{
    Serial.println("... Sensor- Variablen ...");
    
    sensor.bme280 = doc["bme_280"] | false;
    sensor.bme280_temperature = doc["bme_280_temperature"] | false;
    sensor.bme280_humidity = doc["bme_280_humidity"] | false;
    sensor.bme280_pressure = doc["bme_280_pressure"] | false;
    sensor.bme280_high = doc["bme_280_high"] | false;
    sensor.light = doc["light"] | false;
    sensor.ubext = doc["ubext"] | false;
}

StaticJsonDocument<1024> safe_conf_sensor(StaticJsonDocument<1024> doc)
{
    Serial.println("... Sensor- Variablen ...");
    
    doc["bme_280"] = sensor.bme280;
    doc["bme_280_temperature"] = sensor.bme280_temperature;
    doc["bme_280_humidity"] = sensor.bme280_humidity;
    doc["bme_280_pressure"] = sensor.bme280_pressure;
    doc["bme_280_high"] = sensor.bme280_high;
    doc["light"] = sensor.light;
    doc["ubext"] = sensor.ubext;

    return doc;
}