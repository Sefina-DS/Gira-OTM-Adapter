#pragma once

struct SYSTEM_FUNKTION
{
    unsigned long timer = 0;
    String version_old = "20240207";
    String github_path = "https://raw.githubusercontent.com/Sefina-DS/Gira-OTM-Adapter/Firmwareupdate";
    String version_new ;
    String fw_art ;
    boolean new_version ;
};
extern SYSTEM_FUNKTION system_funktion;

//      Inputs
#define RXD2 17
#define TXD2 16
#define input_light 36
#define input_ubext 39
#define input_comport_activ 15
#define input_webportal 13
#define input_reset 25

#define output_led_esp 19
#define output_led_detector 27
#define output_comport_activ 23


void system_timer();
void version_check();
//String firmware_path();
const String& firmware_path();

void firmwareupdate_http();
void update_webpage();
void led_flash_timer(int timer_on,int timer_off, int number);
void speicher_diagnose();

void load_conf_sys(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_sys(StaticJsonDocument<1024> doc);
