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
        temp += F("<tr><td>BME Art :</td>");
        temp += F("<td><select name='bme'><option value='");
        temp += sensor.bme;
        temp += F("' selected>");
        temp += sensor.bme;
        temp += F("</option>");
        temp += F("<option value='keiner vorhanden'>keiner vorhanden</option>");
        temp += F("<option value='BME - 280'>BME - 280</option>");
        temp += F("<option value='BME - 680'>BME - 680</option>");
        temp += F("</select>");
        temp += F("</td></tr>");
        temp += F("</table>");
        // Verdeckte Einstellungen (BME 280 Aktiv)
        temp += F("<table ");
        if (sensor.bme == "keiner vorhanden")
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
            if (sensor.bme_temperature)
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
            if (sensor.bme_humidity)
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
            if (sensor.bme_pressure)
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
            if (sensor.bme_high)
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
            // Verdeckte Einstellungen (BME 680 Aktiv)
            temp += F("<table ");
            if (sensor.bme != "BME - 680")
            {
                temp += F("style='display: none'>");
            }
            else
            {
                temp += F(">");
            }
                // Gas Ohm
                temp += F("<tr><td>Gas in KOhm :</td>");
                temp += F("<td><select name='bme-gas-ohm'><option value='");
                if (sensor.bme_gas_ohm)
                {
                    temp += F("aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert");
                }
                else
                {
                    temp += F("deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert");
                }
                temp += F("</select>");
                temp += F("</td></tr>");
                // Gas Score
                temp += F("<tr><td>Raumklimabewertung im Score :</td>");
                temp += F("<td><select name='bme-gas-score'><option value='");
                if (sensor.bme_gas_score)
                {
                    temp += F("aktiviert' selected>aktiviert</option><option value='deaktiviert'</option>deaktiviert");
                }
                else
                {
                    temp += F("deaktiviert' selected>deaktiviert</option><option value='aktiviert'</option>aktiviert");
                }
                temp += F("</select>");
                temp += F("</td></tr>");
                // Gas Text
                temp += F("<tr><td>Raumklimebewertung im Text :</td>");
                temp += F("<td><select name='bme-gas-text'><option value='");
                if (sensor.bme_gas_text)
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
    if (name == "bme" && msg != "")     sensor.bme = msg;
    if (name == "bme-temperature"   && msg == "aktiviert")      sensor.bme_temperature = true;
    if (name == "bme-temperature"   && msg == "deaktiviert")    sensor.bme_temperature = false;
    if (name == "bme-humidity"   && msg == "aktiviert")      sensor.bme_humidity = true;
    if (name == "bme-humidity"   && msg == "deaktiviert")    sensor.bme_humidity = false;
    if (name == "bme-pressure"   && msg == "aktiviert")      sensor.bme_pressure = true;
    if (name == "bme-pressure"   && msg == "deaktiviert")    sensor.bme_pressure = false;
    if (name == "bme-high"   && msg == "aktiviert")      sensor.bme_high = true;
    if (name == "bme-high"   && msg == "deaktiviert")    sensor.bme_high = false;
    if (name == "bme-gas-ohm"   && msg == "aktiviert")      sensor.bme_gas_ohm = true;
    if (name == "bme-gas-ohm"   && msg == "deaktiviert")    sensor.bme_gas_ohm = false;
    if (name == "bme-gas-score"   && msg == "aktiviert")      sensor.bme_gas_score = true;
    if (name == "bme-gas-score"   && msg == "deaktiviert")    sensor.bme_gas_score = false;
    if (name == "bme-gas-text"   && msg == "aktiviert")      sensor.bme_gas_text = true;
    if (name == "bme-gas-text"   && msg == "deaktiviert")    sensor.bme_gas_text = false;
    if (name == "light"   && msg == "aktiviert")      sensor.light = true;
    if (name == "light"   && msg == "deaktiviert")    sensor.light = false;
    if (name == "ubext"   && msg == "aktiviert")      sensor.ubext = true;
    if (name == "ubext"   && msg == "deaktiviert")    sensor.ubext = false;
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
