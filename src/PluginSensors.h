#pragma once

#define extension "Erweiterungen/"
#define ext_light "Lichtsensor/"
#define ext_sensor "BME-280/"
#define ext_ubext "UB-Ext/"

#define SEALEVELPRESSURE_HPA (1013.25)

struct SENSOR
{
    String bme;
    boolean bme_temperature;
    boolean bme_humidity;
    boolean bme_pressure;
    boolean bme_high;
    boolean bme_gas_ohm;
    boolean bme_gas_score;
    boolean bme_gas_text;
    boolean light;
    boolean ubext;
    
};
extern SENSOR sensor;

void bme_config();
void bme_refresh();
void air_quality(String topic);
void light_refresh();
void ubext_refresh();

String webserver_call_sensor(const String &var);
void webserver_triger_sensor(String name, String msg);

void load_conf_sensor(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_sensor(StaticJsonDocument<1024> doc);