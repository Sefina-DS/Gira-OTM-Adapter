#pragma once

#define SEALEVELPRESSURE_HPA (1013.25)

struct SENSOR
{
    boolean bme280;
    boolean bme280_temperature;
    boolean bme280_humidity;
    boolean bme280_pressure;
    boolean bme280_high;
    boolean light;
    boolean ubext;
    
};
extern SENSOR sensor;

void bme_refresh();
void light_refresh();
void ubext_refresh();

String web_server_sensor(const String &var);
void web_server_sensor_get(String name, String msg);

void load_conf_sensor(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_sensor(StaticJsonDocument<1024> doc);