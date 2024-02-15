#pragma once

struct COMSERIAL
{
    int com_status = 0;
    HardwareSerial com = Serial2; 
    boolean aktiv;
    boolean gestartet;
    unsigned long timer_alarm;

    String message_receive = "";   
};
extern COMSERIAL comserial;

void load_conf_serial(StaticJsonDocument<1024> doc);
StaticJsonDocument<1024> safe_conf_serial(StaticJsonDocument<1024> doc);

void serial_setup();

void serial_receive();
bool serial_transceive(String message);
bool serial_receive_check();
byte calculate_checksum(String message);
void serial_receive_filter(const char* message, int length); 

#define MESSAGE_START 0x02 // ASCII-Wert von '␂' (Startzeichen für eine Nachricht)
#define MESSAGE_END 0x03 // ASCII-Wert von '␃' (Endzeichen für eine Nachricht)
#define ACK_SUCCESS 0x06 // Bestätigung für Erfolg
#define ACK_FAILURE 0x15 // Bestätigung für Misserfolg
#define ACK_TIMEOUT_MS 2000
#define ACK_MAX_RETRIES 5 // Maximale Anzahl von Wiederholungen