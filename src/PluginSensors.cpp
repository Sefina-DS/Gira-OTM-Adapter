#include "SysHeaders.h"

SENSOR sensor;
Adafruit_BME280 bme;
bool bme_run;

void bme_refresh()
{
    bme_run = bme.begin(0x76);
    if (!bme_run)
    {
        client.publish((mqtt.topic_base + "/" + mqtt.topic_define + "/" + extension + ext_sensor + "Status").c_str(), "Störung");
        Serial.println("BME-240 ist in Störung");
    }
    else
    {
        float temp_float;
        char temp_msg[8];
        client.publish((mqtt.topic_base + "/" + mqtt.topic_define + "/" + extension + ext_sensor + "Status").c_str(), "Läuft");
        if (sensor.bme280_temperature)
        {
            temp_float = bme.readTemperature();
            mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + extension + ext_sensor + "Temperatur", String(temp_float));
        }
        if (sensor.bme280_pressure)
        {
            temp_float = bme.readPressure() / 100.0F;
            mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + extension + ext_sensor + "Druck", String(temp_float));
        }
        if (sensor.bme280_humidity)
        {
            temp_float = bme.readHumidity();
            mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + extension + ext_sensor + "Feuchte", String(temp_float));
        }
        if (sensor.bme280_high)
        {
            temp_float = bme.readAltitude(SEALEVELPRESSURE_HPA);
            mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + extension + ext_sensor + "Meter über Meeresspiegel", String(temp_float));
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
        mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + extension + ext_light + "Lichtzahl", String(temp_light));
        temp = temp_light - 4095;
        if (temp != 0)
        {
            temp = -temp;
        }
        temp = (temp * 100) / 4095;
        mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + extension + ext_light + "Lichtwert", String(temp));
    }
}

void ubext_refresh()
{
    int R1 = 10490;
    int R2 = 2140;
    int faktor = 1026;
    float temp_roh = analogRead(input_ubext);
    float temp_ubext = ((temp_roh * 100000) / faktor) / ((100000 * R2) / (R1 + R2));
    mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + extension + ext_ubext + "Spannungsversorgung Extern", String(temp_ubext));
}

String webserver_call_sensor(const String &var)
{
    String temp = "";
    if (var == "web_sensor_sensor")
    {
        temp += F("<div class='sensor'><br/><div class='box'>");
        temp += F("<h3>Senso Einstellungen</h3>");
        temp += F("<form action='/get'>");
        temp += F("<table>");
        // BME280 Aktiv/Deaktiv
        temp += F("<tr><td>BME 280 :</td>");
        temp += F("<td><select name='bme'><option value='");
        if (sensor.bme280)
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
        // Verdeckte Einstellungen (BME 280 Aktiv)
        temp += F("<table ");
        if (!sensor.bme280)
        {
            temp += F("style='display: none'>");
        }
        else
        {
            temp += F(">");
        }
            // Temperatur
            temp += F("<tr><td>Temperatur :</td>");
            temp += F("<td><select name='bme-temperature'><option value='");
            if (sensor.bme280_temperature)
            {
                temp += F("aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert");
            }
            else
            {
                temp += F("deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert");
            }
            temp += F("</select>");
            temp += F("</td></tr>");
            // Feuchtigkeit
            temp += F("<tr><td>Feuchtigkeit :</td>");
            temp += F("<td><select name='bme-humidity'><option value='");
            if (sensor.bme280_humidity)
            {
                temp += F("aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert");
            }
            else
            {
                temp += F("deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert");
            }
            temp += F("</select>");
            temp += F("</td></tr>");
            // Druck
            temp += F("<tr><td>Druck :</td>");
            temp += F("<td><select name='bme-pressure'><option value='");
            if (sensor.bme280_pressure)
            {
                temp += F("aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert");
            }
            else
            {
                temp += F("deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert");
            }
            temp += F("</select>");
            temp += F("</td></tr>");
            // Höhe
            temp += F("<tr><td>Höhe :</td>");
            temp += F("<td><select name='bme-high'><option value='");
            if (sensor.bme280_high)
            {
                temp += F("aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert");
            }
            else
            {
                temp += F("deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert");
            }
            temp += F("</select>");
            temp += F("</td></tr>");
            temp += F("</table><br/>");
        // Light Aktiv/Deaktiv
        temp += F("<table>");
        temp += F("<tr><td>Lichtsensor :</td>");
        temp += F("<td><select name='light'><option value='");
        if (sensor.light)
        {
            temp += F("aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert");
        }
        else
        {
            temp += F("deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert");
        }
        temp += F("</select>");
        temp += F("</td></tr>");
        temp += F("</table><br/>");
        // Spannung Aktiv/Deaktiv
        temp += F("<table>");
        temp += F("<tr><td>Versorgungsspannung :</td>");
        temp += F("<td><select name='ubext'><option value='");
        if (sensor.ubext)
        {
            temp += F("aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert");
        }
        else
        {
            temp += F("deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert");
        }
        temp += F("</select>");
        temp += F("</td></tr>");
        temp += F("</table><br/>");
        temp += F("<input type='submit' value='Submit' />");
        temp += F("</form></div></div>");
        return temp;
    }
    
    return String();
    
}

void webserver_triger_sensor(String name, String msg)
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
    if (name == "bme-pressure")
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