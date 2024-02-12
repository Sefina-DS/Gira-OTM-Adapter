#include "SysHeaders.h"

SENSORS sensors;

Adafruit_Sensor *sensor = NULL;


Adafruit_BME280 bme280;
Adafruit_BME680 bme680;
bool bme280_run;
bool bme680_run;

float hum_score, gas_score;
float gas_reference = 250000;
float hum_reference = 40;
int   getgasreference_count = 0;

void sensor_data(){
    if ( millis() <= sensors.timer ) return;
    sensors.timer = millis() + 10000;
    if ( sensors.ubext ) mqtt_publish("Erweiterungen/UB-Ext", data_ubext());
    if ( sensors.light ) mqtt_publish("Erweiterungen/Light", data_light());
    if ( sensors.bme_configured ) data_bme();
}


void bme_config()
{
    if ( true) 
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
    if (true)
    {
        temp_topic += "280/";
        bme280_run = bme280.begin(0x76);
        if (true)
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
            if (sensors.bme_temperature)     temp_json["Temperatur"]     =   String(bme280.readTemperature(), 1);
            if (sensors.bme_pressure)        temp_json["Druck"]          =   String(bme280.readPressure() / 100.0F, 0);
            if (sensors.bme_humidity)        temp_json["Feuchte"]        =   String(bme280.readHumidity(), 1);
            if (sensors.bme_high)            temp_json["Höhe"]           =   String(bme280.readAltitude(SEALEVELPRESSURE_HPA), 1);
            light_refresh(temp_json);
        }
    }
    // BME - 680
    if ( true )
    {
        temp_topic += "680/";
        bme680_run = bme680.begin();
        if (true)
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
            if (sensors.bme_temperature)     temp_json["Temperatur"]     =   String(bme680.readTemperature(), 1);
            if (sensors.bme_pressure)        temp_json["Druck"]          =   String(bme680.readPressure() / 100.0F, 0);
            if (sensors.bme_humidity)        temp_json["Feuchte"]        =   String(bme680.readHumidity(), 1);
            if (sensors.bme_high)            temp_json["Höhe"]           =   String(bme680.readAltitude(SEALEVELPRESSURE_HPA), 1);
            if (sensors.bme_gas_ohm)         temp_json["Gas_KOhm"]       =   String(bme680.gas_resistance / 1000.0, 1);
            if (sensors.bme_gas_score || sensors.bme_gas_text)    air_quality(temp_json);
            if (!sensors.bme_gas_score && !sensors.bme_gas_text)  light_refresh(temp_json);
            //sensor_mqtt_send(temp_json);
        }
    }
    if ( true ) light_refresh(temp_json);
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

void bme_setup(){
    Wire.begin();
    if (bme_check(BME280_ADDR)) {
        if (!bme280.begin(BME280_ADDR)) {
            sensors.bme_art = "keiner";
            #ifdef DEBUG_SERIAL_OUTPUT
                Serial.println("Es konnte kein BME-280 gefunden werden // überprüfung der Kabel!");
            #endif
            while (1);
        }
        sensors.bme_art = "BME-280";
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.println("BME-280 Sensor wurde gefunden.");
        #endif
    } else if (bme_check(BME680_ADDR)) {
        if (!bme680.begin(BME680_ADDR)) {
            sensors.bme_art = "keiner";
            #ifdef DEBUG_SERIAL_OUTPUT
                Serial.println("Es konnte kein BME-680 gefunden werden // überprüfung der Kabel!");
                
            #endif
            while (1);
        }
        sensors.bme_art = "BME-680";
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.println("BME-680 Sensor wurde gefunden.");
            mqtt_publish("Erweiterung/BME", "BME-680");
        #endif
    } else {
        sensors.bme_art = "keiner";
        #ifdef DEBUG_SERIAL_OUTPUT
            Serial.println("Es wurde kein Sensor gefunden.");
        #endif
        while (1);
    }
    sensors.bme_configured = true;
}

bool bme_check(byte address) {
  Wire.beginTransmission(address);
  byte error = Wire.endTransmission();
  return (error == 0);
}

void data_bme(){
    mqtt_publish("Erweiterung/BME", sensors.bme_art);
    /*Serial.println("BME wair gelesen.");
    if ( sensors.bme_art = "BME-280" ) {
        //bme280_run = bme280.begin(0x76);
        Serial.print("Temperature (BME280): ");
        Serial.print(bme280.readTemperature());
        Serial.println(" °C");
    }*/
    /*if (bme_check(BME280_ADDR)) {
    bme280_run = bme280.begin(0x76);
    Serial.println("280er wird gestartet");
    if (!bme280_run){
    Serial.print("Temperature (BME280): ");
    Serial.print(bme280.readTemperature());
    Serial.println(" °C");
    
    Serial.print("Humidity (BME280): ");
    Serial.print(bme280.readHumidity());
    Serial.println(" %");
    
    Serial.print("Pressure (BME280): ");
    Serial.print(bme280.readPressure() / 100.0F);
    Serial.println(" hPa");
    }
  }
  
  if (bme_check(BME680_ADDR)) {
    Serial.print("Temperature (BME680): ");
    Serial.print(bme680.temperature);
    Serial.println(" °C");
    
    Serial.print("Humidity (BME680): ");
    Serial.print(bme680.humidity);
    Serial.println(" %");
    
    Serial.print("Pressure (BME680): ");
    Serial.print(bme680.pressure / 100.0F);
    Serial.println(" hPa");
    
    Serial.print("Gas Resistance (BME680): ");
    Serial.print(bme680.readGas());
    Serial.println(" ohms");
    
    Serial.print("IAQ (BME680): ");
    Serial.println("bme680.Iaq");
    
    Serial.print("Gas Equivalent (BME680): ");
    Serial.println(bme680.gas_resistance);
  }*/
}

String web_request_sensor(const String &var) {
    if          ( var == "ph_sensor_bme")           { return ( sensors.bme )                 ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_bmedisplay")    { return ( !sensors.bme )                ? "display: none; " 
                                                                                            : "";
    } else if   ( var == "ph_sensor_temp")          { return ( sensors.bme_temperature )     ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_humidity")      { return ( sensors.bme_humidity )        ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_pressure")      { return ( sensors.bme_pressure )        ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_high")          { return ( sensors.bme_high )            ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_gasohm")        { return ( sensors.bme_gas_ohm )         ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_gasscore")      { return ( sensors.bme_gas_score )       ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_gastext")       { return ( sensors.bme_gas_text )        ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_light")         { return ( sensors.light )               ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    } else if   ( var == "ph_sensor_ubext")         { return ( sensors.ubext )               ? "'aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert"
                                                                                            : "'deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert";
    }

    return String();
}

void web_response_sensor(String name, String msg) {
    if          (msg == "aktiviert") {
        if      (name == "bme")                 sensors.bme = true;
        else if (name == "bme-temperature")     sensors.bme_temperature = true;
        else if (name == "bme-humidity")        sensors.bme_humidity = true;
        else if (name == "bme-pressure")        sensors.bme_pressure = true;
        else if (name == "bme-high")            sensors.bme_high = true;
        else if (name == "bme-gas-ohm")         sensors.bme_gas_ohm = true;
        else if (name == "bme-gas-score")       sensors.bme_gas_score = true;
        else if (name == "bme-gas-text")        sensors.bme_gas_text = true;
        else if (name == "light")               sensors.light = true;
        else if (name == "ubext")               sensors.ubext = true;
    } else if   (msg == "deaktiviert") {
        if      (name == "bme")                 sensors.bme = false;
        else if (name == "bme-temperature")     sensors.bme_temperature = false;
        else if (name == "bme-humidity")        sensors.bme_humidity = false;
        else if (name == "bme-pressure")        sensors.bme_pressure = false;
        else if (name == "bme-high")            sensors.bme_high = false;
        else if (name == "bme-gas-ohm")         sensors.bme_gas_ohm = false;
        else if (name == "bme-gas-score")       sensors.bme_gas_score = false;
        else if (name == "bme-gas-text")        sensors.bme_gas_text = false;
        else if (name == "light")               sensors.light = false;
        else if (name == "ubext")               sensors.ubext = false;
    }
}

void load_conf_sensor(StaticJsonDocument<1024> doc)
{
    String bme_str;
    #ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("... Sensor- Variablen ...");
    #endif
    bme_str                  = doc["bme"] | false;
    sensors.bme_temperature      = doc["bme_temperature"] | false;
    sensors.bme_humidity         = doc["bme_humidity"] | false;
    sensors.bme_pressure         = doc["bme_pressure"] | false;
    sensors.bme_high             = doc["bme_high"] | false;
    sensors.bme_gas_ohm          = doc["bme_gas_ohm"] | false;
    sensors.bme_gas_score        = doc["bme_gas_score"] | false;
    sensors.bme_gas_text         = doc["bme_gas_text"] | false;
    sensors.light                = doc["light"] | false;
    sensors.ubext                = doc["ubext"] | false;

    if ( bme_str == "false" || bme_str == "keiner vorhanden" ) {
        sensors.bme = false;
    } else {
        sensors.bme = true;
    }
}

StaticJsonDocument<1024> safe_conf_sensor(StaticJsonDocument<1024> doc)
{
    #ifdef DEBUG_SERIAL_OUTPUT
        Serial.println("... Sensor- Variablen ...");
    #endif
    doc["bme"]              = sensors.bme;
    doc["bme_temperature"]  = sensors.bme_temperature;
    doc["bme_humidity"]     = sensors.bme_humidity;
    doc["bme_pressure"]     = sensors.bme_pressure;
    doc["bme_high"]         = sensors.bme_high;
    doc["bme_gas_ohm"]      = sensors.bme_gas_ohm;
    doc["bme_gas_score"]    = sensors.bme_gas_score;
    doc["bme_gas_text"]     = sensors.bme_gas_text;
    doc["light"]            = sensors.light;
    doc["ubext"]            = sensors.ubext;

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
    
    if (sensors.bme_gas_score)         temp_json["Gas in Score"] =   String(air_quality_score, 0);
    if (sensors.bme_gas_score)         temp_json["Gas in Text"]  =   air_text;

    light_refresh(temp_json);
}

void sensor_mqtt_send(StaticJsonDocument<1024> temp_json)
{
    String temp_string;
    serializeJson(temp_json, temp_string);
    mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/" + "Extension" + "/" + "Sensoren", temp_string, "sensor_mqtt_send");
}
