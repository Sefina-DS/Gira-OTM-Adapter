#pragma once

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



void led_flash_timer(int timer_on,int timer_off, int number);
