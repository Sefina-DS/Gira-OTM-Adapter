#pragma once

#define extension "Erweiterungen/"
#define ext_light "Lichtsensor/"
#define ext_sensor "BME-280/"
#define ext_ubext "UB-Ext/"

#define BME280_ADDR 0x76 // BME280 I2C address
#define BME680_ADDR 0x77 // BME680 I2C address

#define SEALEVELPRESSURE_HPA (1013.25)

struct SENSORS
{
    bool bme;
    bool bme_configured = false;
    String bme_art;
    boolean bme_temperature;
    boolean bme_humidity;
    boolean bme_pressure;
    boolean bme_high;
    boolean bme_gas_ohm;
    boolean bme_gas_score;
    boolean bme_gas_text;
    boolean light;
    boolean ubext;
    unsigned long timer = 0;
    
};
extern SENSORS sensors;
extern Adafruit_Sensor *sensor;

void sensor_data();
String data_ubext();
String data_light();
void bme_setup();
bool bme_check(byte address);
void data_bme();

void bme_config();
void bme_refresh();
void air_quality(StaticJsonDocument<1024> temp_json);
void light_refresh(StaticJsonDocument<1024> temp_json);
void ubext_refresh(StaticJsonDocument<1024> temp_json);

String web_request_sensor(const String &var);
void web_response_sensor(String name, String msg);

void load_conf_sensor(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_sensor(StaticJsonDocument<1024> doc);

void sensor_mqtt_send(StaticJsonDocument<1024> temp_json);