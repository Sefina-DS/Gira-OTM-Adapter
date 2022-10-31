#pragma once

struct COMSERIAL
{
    int com_status = 0; 
    boolean aktiv;
    boolean gestartet;
    unsigned long timer_alarm;
    
    
};
extern COMSERIAL comserial;

extern boolean seri_run;


void serial_status();
void serial_send(String msg_funktion, int status);
void serial_read();

