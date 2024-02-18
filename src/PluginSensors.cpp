#include "SysHeaders.h"

SENSORS sensors;

Adafruit_Sensor *sensor = NULL;

Adafruit_BME280 bme280;
Adafruit_BME680 bme680;

#define DEBUG_SERIAL_SENSOR

void sensor_data() {
    if (millis() <= sensors.timer) return;
    sensors.timer = millis() + 10000;
    if (sensors.ubext) mqtt_publish("Erweiterungen/UB-Ext", data_ubext());
    if (sensors.light) mqtt_publish("Erweiterungen/Light", data_light());
    if (sensors.bme_configured) mqtt_publish("Erweiterungen/BME", data_bme());
}

String data_light() {
    StaticJsonDocument<1024> temp_json;
    String temp_string;
    static int temp_light = 0;
    int input_light_reading = analogRead(input_light);

    // Berechnung der Differenz zwischen aktuellen und vorherigen Lichtwerten
    int light_difference = abs(input_light_reading - temp_light);

    // Aktualisierung des vorherigen Lichtwerts
    temp_light = input_light_reading;

    // Formatieren der Lichtwerte für die Ausgabe
    temp_json["Lichtzahl"] = String(input_light_reading);
    temp_json["Lichtwert"] = String(map(input_light_reading, 0, 4095, 100, 0));

    serializeJson(temp_json, temp_string);
    return temp_string;
}

String data_ubext() {
    StaticJsonDocument<1024> temp_json;
    String temp_string;
    int R1 = 10490;
    int R2 = 2140;
    int faktor = 1026;
    float temp_roh = analogRead(input_ubext);
    float temp_ubext = ((temp_roh * 100000) / faktor) / ((100000 * R2) / (R1 + R2));
    temp_json["UB-Ext"] = String(temp_ubext);
    serializeJson(temp_json, temp_string);
    return temp_string;
}

void bme_setup() {
    Wire.begin();

    bool bme280_found = bme_check(BME280_ADDR);
    bool bme680_found = bme_check(BME680_ADDR);

    if (!bme280_found && !bme680_found) {
        sensors.bme_art = "keiner";
        #ifdef DEBUG_SERIAL_SENSOR
            Serial.println("Es wurde kein Sensor gefunden.");
        #endif
        while (1);
    }

    if (bme280_found) {
        if (!bme280.begin(BME280_ADDR)) {
            #ifdef DEBUG_SERIAL_SENSOR
                Serial.println("Es konnte kein BME-280 gefunden werden // Überprüfung der Kabel!");
            #endif
            while (1);
        }
        sensors.bme_art = "BME-280";
        #ifdef DEBUG_SERIAL_SENSOR
            Serial.println("BME-280 Sensor wurde gefunden.");
        #endif
    } else {
        if (!bme680.begin(BME680_ADDR)) {
            #ifdef DEBUG_SERIAL_SENSOR
                Serial.println("Es konnte kein BME-680 gefunden werden // Überprüfung der Kabel!");
            #endif
            while (1);
        }
        sensors.bme_art = "BME-680";
        #ifdef DEBUG_SERIAL_SENSOR
            Serial.println("BME-680 Sensor wurde gefunden.");
        #endif
    }
    sensors.bme_configured = true;
}

bool bme_check(byte address) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();
    return (error == 0);
}

void data_bme_280(Adafruit_BME280 *sensor, StaticJsonDocument<1024> &json) {
    json["BME"] = "BME-280";
    if (sensors.bme_temperature) json["Temperatur"] = String(sensor->readTemperature(), 1);
    if (sensors.bme_pressure) json["Druck"] = String(sensor->readPressure() / 100.0F, 0);
    if (sensors.bme_humidity) json["Feuchte"] = String(sensor->readHumidity(), 1);
    if (sensors.bme_high) json["Höhe"] = String(sensor->readAltitude(SEALEVELPRESSURE_HPA), 1);
}

void data_bme_680(Adafruit_BME680 *sensor, StaticJsonDocument<1024> &json) {
    json["BME"] = "BME-680";
    if (sensors.bme_temperature) json["Temperatur"] = String(sensor->readTemperature(), 1);
    if (sensors.bme_pressure) json["Druck"] = String(sensor->readPressure() / 100.0F, 0);
    if (sensors.bme_humidity) json["Feuchte"] = String(sensor->readHumidity(), 1);
    if (sensors.bme_high) json["Höhe"] = String(sensor->readAltitude(SEALEVELPRESSURE_HPA), 1);
    if (sensors.bme_gas_ohm) json["Gas_KOhm"] = String(sensor->gas_resistance / 1000.0, 1);
    if (sensors.bme_gas_score) json["Gas_Score"] = String(bme_680_airquality_calculate());
    if (sensors.bme_gas_text) json["Gas_Text"] = String(bme_680_airquality_text(bme_680_airquality_calculate()));
}

String data_bme() {
    StaticJsonDocument<1024> temp_json;
    String temp_string;
    if (sensors.bme_art == "BME-280") {
        data_bme_280(&bme280, temp_json);
    } else if (sensors.bme_art == "BME-680") {
        data_bme_680(&bme680, temp_json);
    } else {
        temp_json["BME"] = "keiner angeschlossen oder Defekt";
    }
    serializeJson(temp_json, temp_string);
    return temp_string;
}

float bme_680_airquality_calculate() {
    float temp_humidity = bme680.readHumidity();
    float hum_score;
    if (temp_humidity >= 38 && temp_humidity <= 42)
        hum_score = 0.25 * 100; // Humidity +/-5% around optimum
    else {
        if (temp_humidity < 38)
            hum_score = 0.25 / 38 * temp_humidity * 100;
        else
            hum_score = ((-0.25 / (100 - 38) * temp_humidity) + 0.416666) * 100;
    }

    float gas_lower_limit = 5000; // Bad air quality limit
    float gas_upper_limit = 50000; // Good air quality limit
    float gas_reference = bme680.readGas();
    if (gas_reference > gas_upper_limit) gas_reference = gas_upper_limit;
    if (gas_reference < gas_lower_limit) gas_reference = gas_lower_limit;
    float gas_score = (0.75 / (gas_upper_limit - gas_lower_limit) * gas_reference - (gas_lower_limit * (0.75 / (gas_upper_limit - gas_lower_limit)))) * 100;

    float air_quality_score = hum_score + gas_score;
    air_quality_score = (100 - air_quality_score) * 5;
    return air_quality_score;
}

String bme_680_airquality_text(float air_quality_score) {
    if (air_quality_score >= 301)
        return "Gefährlich";
    else if (air_quality_score >= 201)
        return "Sehr ungesund";
    else if (air_quality_score >= 176)
        return "Ungesund";
    else if (air_quality_score >= 151)
        return "Ungesund für Sensible";
    else if (air_quality_score >= 51)
        return "Angenehm";
    else
        return "Gut";
}

String web_request_sensor(const String &var) {
    if          (var == "button_sensors_bme")       { return (sensors.bme)              ? "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'>deaktiviert</option>"
                                                                                        : "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'>aktiviert</option>";
    } else if   (var == "display_sensors_bme")      { return (sensors.bme)              ? ""
                                                                                        : "style= 'display: none'";
    } else if   (var == "button_sensors_temp")      { return (sensors.bme_temperature)  ? "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                        : "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   (var == "button_sensors_humidity")  { return (sensors.bme_humidity)     ? "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                        : "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   (var == "button_sensors_pressure")  { return (sensors.bme_pressure)     ? "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                        : "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   (var == "button_sensors_high")      { return (sensors.bme_high)         ? "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                        : "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   (var == "button_sensors_gasohm")    { return (sensors.bme_gas_ohm)      ? "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                        : "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   (var == "button_sensors_gasscore")  { return (sensors.bme_gas_score)    ? "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                        : "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   (var == "button_sensors_gastext")   { return (sensors.bme_gas_text)     ? "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                        : "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   (var == "button_sensors_light")     { return (sensors.light)            ? "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                        : "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   (var == "button_sensors_ubext")     { return (sensors.ubext)            ? "<option value='aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                        : "<option value='deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    }
    return String();
}

void web_response_sensor(String name, String msg) {
    if (msg == "aktiviert") {
        if (name == "bme") sensors.bme = true;
        else if (name == "bme-temperature") sensors.bme_temperature = true;
        else if (name == "bme-humidity") sensors.bme_humidity = true;
        else if (name == "bme-pressure") sensors.bme_pressure = true;
        else if (name == "bme-high") sensors.bme_high = true;
        else if (name == "bme-gas-ohm") sensors.bme_gas_ohm = true;
        else if (name == "bme-gas-score") sensors.bme_gas_score = true;
        else if (name == "bme-gas-text") sensors.bme_gas_text = true;
        else if (name == "light") sensors.light = true;
        else if (name == "ubext") sensors.ubext = true;
    } else if (msg == "deaktiviert") {
        if (name == "bme") sensors.bme = false;
        else if (name == "bme-temperature") sensors.bme_temperature = false;
        else if (name == "bme-humidity") sensors.bme_humidity = false;
        else if (name == "bme-pressure") sensors.bme_pressure = false;
        else if (name == "bme-high") sensors.bme_high = false;
        else if (name == "bme-gas-ohm") sensors.bme_gas_ohm = false;
        else if (name == "bme-gas-score") sensors.bme_gas_score = false;
        else if (name == "bme-gas-text") sensors.bme_gas_text = false;
        else if (name == "light") sensors.light = false;
        else if (name == "ubext") sensors.ubext = false;
    }
}

void load_conf_sensor(StaticJsonDocument<1024> doc) {
    String bme_str;
    #ifdef DEBUG_SERIAL_SENSOR
        Serial.println("... Sensor- Variablen ...");
    #endif
    sensors.bme = doc["bme"] | false;
    sensors.bme_temperature = doc["bme_temperature"] | false;
    sensors.bme_humidity = doc["bme_humidity"] | false;
    sensors.bme_pressure = doc["bme_pressure"] | false;
    sensors.bme_high = doc["bme_high"] | false;
    sensors.bme_gas_ohm = doc["bme_gas_ohm"] | false;
    sensors.bme_gas_score = doc["bme_gas_score"] | false;
    sensors.bme_gas_text = doc["bme_gas_text"] | false;
    sensors.light = doc["light"] | false;
    sensors.ubext = doc["ubext"] | false;
}

StaticJsonDocument<1024> safe_conf_sensor(StaticJsonDocument<1024> doc) {
    #ifdef DEBUG_SERIAL_SENSOR
        Serial.println("... Sensor- Variablen ...");
    #endif
    doc["bme"] = sensors.bme;
    doc["bme_temperature"] = sensors.bme_temperature;
    doc["bme_humidity"] = sensors.bme_humidity;
    doc["bme_pressure"] = sensors.bme_pressure;
    doc["bme_high"] = sensors.bme_high;
    doc["bme_gas_ohm"] = sensors.bme_gas_ohm;
    doc["bme_gas_score"] = sensors.bme_gas_score;
    doc["bme_gas_text"] = sensors.bme_gas_text;
    doc["light"] = sensors.light;
    doc["ubext"] = sensors.ubext;

    return doc;
}
