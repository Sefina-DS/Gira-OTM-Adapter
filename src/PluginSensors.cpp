#include "SysHeaders.h"

SENSOR sensor;
Adafruit_BME280 bme280;
Adafruit_BME680 bme680;
bool bme280_run;
bool bme680_run;

float hum_score, gas_score;
float gas_reference = 250000;
float hum_reference = 40;
int   getgasreference_count = 0;

void sensor_data(){
    if ( millis() <= sensor.timer ) return;
    sensor.timer = millis() + 10000;
    if ( sensor.ubext ) mqtt_publish("Erweiterungen/UB-Ext", data_ubext());
    if ( sensor.light ) mqtt_publish("Erweiterungen/Light", data_light());
}


void bme_config()
{
    if ( sensor.bme == "BME - 680" && !bme680.begin()) 
    {
        while (1);
        bme680.setHumidityOversampling(BME680_OS_2X);
        bme680.setPressureOversampling(BME680_OS_4X);
        bme680.setIIRFilterSize(BME680_FILTER_SIZE_3);
        bme680.setGasHeater(320, 150); // 320*C for 150 ms
    }
}

void bme_refresh()
{
    StaticJsonDocument<1024> temp_json;
    String temp_topic = mqtt.topic_base + "/" + mqtt.topic_define + "/Erweiterungen/BME-";
    // BME - 280
    if (sensor.bme == "BME - 280")
    {
        temp_topic += "280/";
        bme280_run = bme280.begin(0x76);
        if (!bme280_run)
        {
            temp_json["BME-Status"]              = "Störung";
            light_refresh(temp_json);
            #ifdef DEBUG_SERIAL_OUTPUT
                Serial.println("BME-280 ist in Störung");
            #endif
        }
        else
        {
            temp_json["BME-Status"]              = "Läuft";
            if (sensor.bme_temperature)     temp_json["Temperatur"]     =   String(bme280.readTemperature(), 1);
            if (sensor.bme_pressure)        temp_json["Druck"]          =   String(bme280.readPressure() / 100.0F, 0);
            if (sensor.bme_humidity)        temp_json["Feuchte"]        =   String(bme280.readHumidity(), 1);
            if (sensor.bme_high)            temp_json["Höhe"]           =   String(bme280.readAltitude(SEALEVELPRESSURE_HPA), 1);
            light_refresh(temp_json);
        }
    }
    // BME - 680
    if ( sensor.bme == "BME - 680")
    {
        temp_topic += "680/";
        bme680_run = bme680.begin();
        if (!bme680_run)
        {
            #ifdef DEBUG_SERIAL_OUTPUT
                Serial.println("BME-680 ist in Störung");
            #endif
            temp_json["BME-Status"]              = "Störung";
            light_refresh(temp_json);
        }
        else
        {
            temp_json["BME-Status"]              = "Läuft";
            if (sensor.bme_temperature)     temp_json["Temperatur"]     =   String(bme680.readTemperature(), 1);
            if (sensor.bme_pressure)        temp_json["Druck"]          =   String(bme680.readPressure() / 100.0F, 0);
            if (sensor.bme_humidity)        temp_json["Feuchte"]        =   String(bme680.readHumidity(), 1);
            if (sensor.bme_high)            temp_json["Höhe"]           =   String(bme680.readAltitude(SEALEVELPRESSURE_HPA), 1);
            if (sensor.bme_gas_ohm)         temp_json["Gas_KOhm"]       =   String(bme680.gas_resistance / 1000.0, 1);
            if (sensor.bme_gas_score || sensor.bme_gas_text)    air_quality(temp_json);
            if (!sensor.bme_gas_score && !sensor.bme_gas_text)  light_refresh(temp_json);
            //sensor_mqtt_send(temp_json);
        }
    }
    if ( sensor.bme == "keiner vorhanden") light_refresh(temp_json);
}

void light_refresh(StaticJsonDocument<1024> temp_json)
{
    if (sensor.light)
    {
        String topic;
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
        temp_light = analogRead(input_light);
        temp_json["Lichtzahl"]  =   String(temp_light);
        temp = temp_light - 4095;
        if (temp != 0)
        {
            temp = -temp;
        }
        temp = (temp * 100) / 4095;
        temp_json["Lichtwert"]  =   String(temp);
    }

    ubext_refresh(temp_json);
}

String data_light(){
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

void ubext_refresh(StaticJsonDocument<1024> temp_json)
{
    if (sensor.ubext)
    {
        int R1 = 10490;
        int R2 = 2140;
        int faktor = 1026;
        float temp_roh = analogRead(input_ubext);
        float temp_ubext = ((temp_roh * 100000) / faktor) / ((100000 * R2) / (R1 + R2));
        temp_json["UB-Ext"]  =   String(temp_ubext);
    }

    sensor_mqtt_send(temp_json);

}
String data_ubext(){
    StaticJsonDocument<1024> temp_json;
    String temp_string;
    int R1 = 10490;
    int R2 = 2140;
    int faktor = 1026;
    float temp_roh = analogRead(input_ubext);
    float temp_ubext = ((temp_roh * 100000) / faktor) / ((100000 * R2) / (R1 + R2));
    temp_json["UB-Ext"]  =   String(temp_ubext);
    serializeJson(temp_json, temp_string);
    return temp_string;
}

String web_request_sensor(const String &var) {
    if          ( var == "ph_sensor_bme")           { return sensor.bme;
    } else if   ( var == "ph_sensor_bmedisplay")    { return (sensor.bme == "keiner vorhanden")         ? "display: none; " 
                                                                                                        : "";
    } else if   ( var == "ph_sensor_temp")          { return ( sensor.bme_temperature )     ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_humidity")      { return ( sensor.bme_humidity )        ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_pressure")      { return ( sensor.bme_pressure )        ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_high")          { return ( sensor.bme_high )            ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_bme680")        { return ( sensor.bme != "BME - 680" )  ? "display: none; " 
                                                                                            : "";
    } else if   ( var == "ph_sensor_gasohm")        { return ( sensor.bme_gas_ohm )         ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_gasscore")      { return ( sensor.bme_gas_score )       ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_gastext")       { return ( sensor.bme_gas_text )        ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_light")         { return ( sensor.light )               ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_ubext")         { return ( sensor.ubext )               ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    }

    return String();
}

void web_response_sensor(String name, String msg) {
    if          (msg == "aktiviert") {
        if      (name == "bme")                 sensor.bme = msg;
        else if (name == "bme-temperature")     sensor.bme_temperature = true;
        else if (name == "bme-humidity")        sensor.bme_humidity = true;
        else if (name == "bme-pressure")        sensor.bme_pressure = true;
        else if (name == "bme-high")            sensor.bme_high = true;
        else if (name == "bme-gas-ohm")         sensor.bme_gas_ohm = true;
        else if (name == "bme-gas-score")       sensor.bme_gas_score = true;
        else if (name == "bme-gas-text")        sensor.bme_gas_text = true;
        else if (name == "light")               sensor.light = true;
        else if (name == "ubext")               sensor.ubext = true;
    } else if   (msg == "deaktiviert") {
        if      (name == "bme")                 sensor.bme = msg;
        else if (name == "bme-temperature")     sensor.bme_temperature = false;
        else if (name == "bme-humidity")        sensor.bme_humidity = false;
        else if (name == "bme-pressure")        sensor.bme_pressure = false;
        else if (name == "bme-high")            sensor.bme_high = false;
        else if (name == "bme-gas-ohm")         sensor.bme_gas_ohm = false;
        else if (name == "bme-gas-score")       sensor.bme_gas_score = false;
        else if (name == "bme-gas-text")        sensor.bme_gas_text = false;
        else if (name == "light")               sensor.light = false;
        else if (name == "ubext")               sensor.ubext = false;
    }
}

void load_conf_sensor(StaticJsonDocument<1024> doc)
{
    #ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("... Sensor- Variablen ...");
    #endif
    sensor.bme                  = doc["bme"] | "keiner vorhanden";
    sensor.bme_temperature      = doc["bme_temperature"] | false;
    sensor.bme_humidity         = doc["bme_humidity"] | false;
    sensor.bme_pressure         = doc["bme_pressure"] | false;
    sensor.bme_high             = doc["bme_high"] | false;
    sensor.bme_gas_ohm          = doc["bme_gas_ohm"] | false;
    sensor.bme_gas_score        = doc["bme_gas_score"] | false;
    sensor.bme_gas_text         = doc["bme_gas_text"] | false;
    sensor.light                = doc["light"] | false;
    sensor.ubext                = doc["ubext"] | false;
}

StaticJsonDocument<1024> safe_conf_sensor(StaticJsonDocument<1024> doc)
{
    #ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("... Sensor- Variablen ...");
    #endif
    doc["bme"]              = sensor.bme;
    doc["bme_temperature"]  = sensor.bme_temperature;
    doc["bme_humidity"]     = sensor.bme_humidity;
    doc["bme_pressure"]     = sensor.bme_pressure;
    doc["bme_high"]         = sensor.bme_high;
    doc["bme_gas_ohm"]      = sensor.bme_gas_ohm;
    doc["bme_gas_score"]    = sensor.bme_gas_score;
    doc["bme_gas_text"]     = sensor.bme_gas_text;
    doc["light"]            = sensor.light;
    doc["ubext"]            = sensor.ubext;

    return doc;
}

void air_quality(StaticJsonDocument<1024> temp_json) 
{
    float temp_humidity = bme680.readHumidity();
    if (temp_humidity >= 38 && temp_humidity <= 42)     hum_score = 0.25*100; // Humidity +/-5% around optimum 
    else
    { //sub-optimal
        if (temp_humidity < 38)                         hum_score = 0.25/hum_reference*temp_humidity*100;
        else
        {
            hum_score = ((-0.25/(100-hum_reference)*temp_humidity)+0.416666)*100;
        }
    }
    //Calculate gas contribution to IAQ index
    float gas_lower_limit = 5000;   // Bad air quality limit
    float gas_upper_limit = 50000;  // Good air quality limit 
    if (gas_reference > gas_upper_limit)    gas_reference = gas_upper_limit; 
    if (gas_reference < gas_lower_limit)    gas_reference = gas_lower_limit;
    gas_score = (0.75/(gas_upper_limit-gas_lower_limit)*gas_reference -(gas_lower_limit*(0.75/(gas_upper_limit-gas_lower_limit))))*100;
    float air_quality_score = hum_score + gas_score;
    if ((getgasreference_count++)%10==0) 
    {
        int readings = 10;
        for (int i = 1; i <= readings; i++)
        {
            gas_reference += bme680.readGas();
        }
        gas_reference = gas_reference / readings;
    }
    String air_text;
    air_quality_score = (100 - air_quality_score) * 5;
    if (air_quality_score >= 301)                               air_text = "Gefährlich";
    if (air_quality_score >= 201 && air_quality_score <= 300 )  air_text = "sehr Ungesund";
    if (air_quality_score >= 176 && air_quality_score <= 200 )  air_text = "Ungesund";
    if (air_quality_score >= 151 && air_quality_score <= 175 )  air_text = "Ungesund für Sensiebelchen";
    if (air_quality_score >=  51 && air_quality_score <= 150 )  air_text = "Angenehm";
    if (air_quality_score >=  00 && air_quality_score <=  50 )  air_text = "Gut";
    
    if (sensor.bme_gas_score)         temp_json["Gas in Score"] =   String(air_quality_score, 0);
    if (sensor.bme_gas_score)         temp_json["Gas in Text"]  =   air_text;

    light_refresh(temp_json);
}

void sensor_mqtt_send(StaticJsonDocument<1024> temp_json)
{
    String temp_string;
    serializeJson(temp_json, temp_string);
    mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + "Extension" + "/" + "Sensoren", temp_string, "sensor_mqtt_send");
}
