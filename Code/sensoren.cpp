#include "sensoren.hpp"

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
        if (config.bme_280_temperature)
        {
            temp_float = bme.readTemperature();
            mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + extension + ext_sensor + "Temperatur", String(temp_float));
        }
        if (config.bme_280_pressure)
        {
            temp_float = bme.readPressure() / 100.0F;
            mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + extension + ext_sensor + "Druck", String(temp_float));
        }
        if (config.bme_280_humidity)
        {
            temp_float = bme.readHumidity();
            mqtt_publish(config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + extension + ext_sensor + "Feuchte", String(temp_float));
        }
        if (config.bme_280_high)
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