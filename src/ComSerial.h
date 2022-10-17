#pragma once

struct COMSERIAL
{
    int com_status;
    boolean aktiv;
    boolean gestartet;
    unsigned long timer_alarm;
    
    
};
extern COMSERIAL comserial;

extern boolean seri_run;


void serial_status();
void serial_send(String msg_funktion);
void serial_read();

