#include "SysHeaders.h"

SYSTEM_FUNKTION system_funktion;

void system_timer()
{
    system_funktion.timer = millis() + 10000 ;

    if ( WiFi.isConnected() && mqtt.aktiv && !client.connected() ) mqtt_connect();
    if (!WiFi.isConnected() && !AP_Mode ) wlan_connect();
        
    led_flash_timer(100 ,0 ,1);
}

void version_check()
{
    if (WiFi.isConnected() == true )
    {
        Serial.println("Version- Update wird geprüfft ... ");
        HTTPClient http;
        http.begin( firmware_path() + "firmware/version.txt" );
        int httpCode = http.GET();
        if(httpCode > 0) 
        {
            if(httpCode == HTTP_CODE_OK) 
            {
                system_funktion.version_new = http.getString() ;
            }
        } else {
            system_funktion.version_new = system_funktion.version_old ;
        }
        http.end();
        if ( system_funktion.version_old != system_funktion.version_new )
        {
            system_funktion.new_version = true;
        } else {
            system_funktion.new_version = false;
        }
        Serial.print("alte Version : ");
        Serial.print(system_funktion.version_old);
        Serial.print(" // neue Version : ");
        Serial.print(system_funktion.version_new);
        Serial.println();
        if ( system_funktion.new_version )
        {
            Serial.println("Eine neue Version steht zum download bereit.");
        } else {
            Serial.println("Kein Update vorhanden.");
        }
    }
}

void update_webpage()
{
    if (WiFi.isConnected() == true )
    {
        SPIFFS.remove("/config.html");
        File file_html = SPIFFS.open("/config.html", "w");
        if (file_html) 
        {
            HTTPClient http;
            http.begin( firmware_path() + "data/config.html" );
            int httpCode = http.GET();
            if(httpCode > 0) 
            {
                if(httpCode == HTTP_CODE_OK) 
                {
                    file_html.print( http.getString()) ;
                    file_html.close();
                }
            } 
            http.end();
        }
        SPIFFS.remove("/config.css");
        File file_css = SPIFFS.open("/config.css", "w");
        if (file_css) 
        {
            HTTPClient http;
            http.begin( firmware_path() + "data/config.css" );
            int httpCode = http.GET();
            if(httpCode > 0) 
            {
                if(httpCode == HTTP_CODE_OK) 
                {
                    file_css.print( http.getString()) ;
                    file_css.close();
                }
            } 
            http.end();
        }
    }
}

void firmwareupdate_http()
// https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/main/firmware/firmware.bin
{
    if (WiFi.isConnected() &&
        system_funktion.new_version )
    {
        Serial.println(firmware_path());
        update_webpage();
        t_httpUpdate_return ret = ESPhttpUpdate.update( firmware_path() + "firmware/firmware.bin" );
        switch(ret) {
            case HTTP_UPDATE_FAILED:
                Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                break;
            case HTTP_UPDATE_NO_UPDATES:
                Serial.println("HTTP_UPDATE_NO_UPDATES");
                break;
            case HTTP_UPDATE_OK:
                Serial.println("HTTP_UPDATE_OK");
                break;
        }
    }
}

void led_flash_timer(int timer_on,int timer_off, int number)
{
    for (int i = 0; i < number; i++)
    {
        digitalWrite(output_led_detector, HIGH);
        digitalWrite(output_led_esp, HIGH);
        delay(timer_on);
        digitalWrite(output_led_detector, LOW);
        digitalWrite(output_led_esp, LOW);
        delay(timer_off);
    }
}

void speicher_diagnose()
{
    StaticJsonDocument<1024> temp_json;
    String temp_string;
  
    temp_json["getHeapSize"]        = ESP.getHeapSize();
    temp_json["getPsramSize"]       = ESP.getPsramSize();
    temp_json["getFreeHeap"]        = ESP.getFreeHeap();
    temp_json["getFreePsram"]       = ESP.getFreePsram();
    temp_json["getMinFreeHeap"]     = ESP.getMinFreeHeap();
    temp_json["getMinFreePsram"]    = ESP.getMinFreePsram();
    temp_json["getMaxAllocHeap"]    = ESP.getMaxAllocHeap();
    temp_json["getMaxAllocPsram"]   = ESP.getMaxAllocPsram();

    serializeJson(temp_json, temp_string);
    mqtt_publish(mqtt.topic_base + "/" + mqtt.topic_define + "/ESP/-Speicher-Status-", temp_string, "mqtt_esp_status");
}

void load_conf_sys(StaticJsonDocument<1024> doc)
{
    Serial.println("... Sys- Variablen ...");
    
    system_funktion.fw_art = doc["fw"] | "main";
}

StaticJsonDocument<1024> safe_conf_sys(StaticJsonDocument<1024> doc)
{
    Serial.println("... Sys- Variablen ...");
    
    doc["fw"] = system_funktion.fw_art;

    return doc;
}

/*String firmware_path ()
{
    String temp ;
    temp += "https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/" ;
    temp += system_funktion.fw_art ;
    temp += "/" ;

    return temp;
}*/

const String& firmware_path() {
    static String temp;
    temp = "https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/";
    temp += system_funktion.fw_art;
    temp += "/";
    Serial.println(temp);
    return temp;
}

String web_request_sys(const String &var) {
    if          (var == "ph_sys_fwtyp") {
        return (system_funktion.fw_art == "main" )  ? "'main' selected='main'>main</option><option value='beta'>beta" 
                                                    : "'beta' selected='beta'>beta</option><option value='main'>main";
    } else if   (var == "ph_sys_fwold") {
        return system_funktion.version_old ;
    } else if   (var == "ph_sys_fwnew") {
        return system_funktion.version_new ;
    }
    return String();
}
void web_response_sys(String name, String msg) {
    if (msg != "")
    {
        if (name == "fw_build")         { system_funktion.fw_art = msg; version_check(); }
        if (name == "Firmwareupdate")   { firmwareupdate_http(); }
    }
}