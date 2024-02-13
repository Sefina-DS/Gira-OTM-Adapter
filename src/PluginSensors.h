#pragma once
#define BME280_ADDR 0x76 // BME280 I2C address
#define BME680_ADDR 0x77 // BME680 I2C address

#define SEALEVELPRESSURE_HPA (1013.25)

struct SENSORS
{
    bool bme                = false;
    bool bme_configured     = false;
    String bme_art          = "";
    boolean bme_temperature = false;
    boolean bme_humidity    = false;
    boolean bme_pressure    = false;
    boolean bme_high        = false;
    boolean bme_gas_ohm     = false;
    boolean bme_gas_score   = false;
    boolean bme_gas_text    = false;
    boolean light           = false;
    boolean ubext           = false;
    unsigned long timer     = 0;
};
extern SENSORS sensors;
extern Adafruit_Sensor *sensor;

void sensor_data();
String data_ubext();
String data_light();
void bme_setup();
bool bme_check(byte address);
String data_bme();
void data_bme_280(Adafruit_BME280 *sensor, StaticJsonDocument<1024> &json);
void data_bme_680(Adafruit_BME680 *sensor, StaticJsonDocument<1024> &json);

float bme_680_airquality_calculate();
String bme_680_airquality_text(float air_quality_score);

String web_request_sensor(const String &var);
void web_response_sensor(String name, String msg);

void load_conf_sensor(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_sensor(StaticJsonDocument<1024> doc);
