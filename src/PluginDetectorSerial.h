#pragma once

struct DETECTORDIAG
{
    unsigned long timer = 10000;
    int status = 0;
    int counter = 0;
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

void serial_transceive_diagnose();
void serial_receive_diagnose(String msg);
