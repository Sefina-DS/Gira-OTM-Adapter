#pragma once

struct DETECTORDIAG
{
    unsigned long timer = 0;
    int status = 0;
    String nr_talarm_k; // Anzahl - Test - Alarm - Kabel / Funk / Lokal
    String nr_talarm_f;
    String nr_talarm_l;
    String nr_alarm_k;
    String nr_alarm_f;
    String nr_alarm_l_opti;
    String nr_alarm_l_temp;
    String temp_1;
    String temp_2;
    String time;
    String ub_ext;
    String ub_batterie;
    String serial_nr;
    String optical_dirt;
    String optical_smoke;
};
extern DETECTORDIAG detectordiag;

void filter(byte msg[10], int size);
void mqtt_link(String topic, String msg);

void detector_serial_timer();