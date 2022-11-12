#include "SysHeaders.h"

COMSERIAL comserial;

void load_conf_serial(StaticJsonDocument<1024> doc)
{
    Serial.println("... Serial- Variablen ...");

    String temp;
    
    comserial.aktiv                = doc["serial"] | true;
}

StaticJsonDocument<1024> safe_conf_serial(StaticJsonDocument<1024> doc)
{
    Serial.println("... Serial- Variablen ...");
    
    doc["serial"]                   = comserial.aktiv;

    return doc;
}

void serial_send(String msg_funktion = "", int status = 1)
{
    if ( comserial.com_status != status ) comserial.com_status = status;
    if (!comserial.gestartet)
    {
        return;
    }
    static String msg_speicher = "";
    if (msg_funktion != "")
    {
        msg_speicher = msg_funktion;
    }
    int deci = 0;
    char hex[8] = {0};
    int number = 0;
    byte msg[20];
    int length = msg_speicher.length();
    if (comserial.com_status % 2 != 0) // muss ungerade sein !
    {
        if (comserial.com_status < 7)
        {
            comserial.com_status++;
            //      String -> Byte || Buchstaben in decimal und Addieren
            for (byte n = 0; n < msg_speicher.length(); n++)
            {
                msg[n] = msg_speicher[n];
                deci += msg[n];
            }
            // Serial.print("Checksumme in Deci : ");
            // Serial.println(deci);
            //       decimal umwandeln in Hexadecimal
            while (deci > 0)
            {
                int i = deci % 16;
                deci = deci / 16;
                hex[number] = i + '0';
                if (i >= 10)
                    hex[number] += 7;
                number++;
            }
            //      Checksumme (Hex) an die Massage anhängen
            msg[length] = hex[1];
            msg[length + 1] = hex[0];
            number = 0;
            //      Message an Serial 2 Ausgeben
            Serial2.write(0x02);
            while (number < length + 2)
            {
                Serial2.write(msg[number]);
                number++;
            }
            Serial2.write(0x03);
            //      Massage komplet ausgeben über Serial 1 (Debug)
            Serial.print("Die Nachricht : ");
            number = 0;
            while (number < length)
            {
                Serial.print(msg[number], byte());
                number++;
            }
            Serial.print("-");
            Serial.print(msg[length], byte());
            Serial.print(msg[length + 1], byte());
            Serial.println(" / wird gesendet (gestartet)");
        }
        else
        {
            Serial.println("Senden- Serial abgebrochen");
            comserial.com_status = 0;
            msg_speicher = "";
        }
    }
}

void serial_read()
{
    if (Serial2.available() > 0)
    {
        String temp_receive = "";
        int deci = 0;
        char hex[8] = {0};
        int number = 0;
        int begin = 0;
        int end = 0;
        const int size = 20;
        byte buff[size];
        char letter;

        Serial.println("Nachricht wird Empfangen !");
        int rlen = Serial2.readBytes(buff, size);
        for (int i = 0; i < rlen; i++)
        {
            if (buff[i] == 0x02)
            {
                begin = i + 1;
            }
            if (buff[i] == 0x03)
            {
                end = i - 2;
            }
        }
        if (buff[0] == 0x06)
        {
            Serial.println("Senden- Serial erfolgreich");
            comserial.com_status = 0;
            if (buff[2] != 0x02)
            {
                return;
            }
        }
        if (buff[0] == 0x15)
        {
            Serial.println("Senden- Serial nicht erfolgreich");
            comserial.com_status++;
            if (buff[2] != 0x02)
            {
                return;
            }
        }
        //          Checksumme
        for (int i = begin; i < end; i++)
        {
            deci += buff[i];
        }
        number = 0;
        while (deci > 0)
        {
            int i = deci % 16;
            deci = deci / 16;
            hex[number] = i + '0';
            if (i >= 10)
                hex[number] += 7;
            number++;
        }
        if (buff[end + 1] == hex[0] && buff[end] == hex[1])
        {
            //          Erfolgreich
            int ssize = 0;
            byte shex[10];
            Serial2.write(0x06);
            for (int i = begin; i < end; i++)
            {
                letter = buff[i];
                temp_receive += letter;
                shex[ssize] = letter;
                ssize++;
            }
            Serial.println("Nachricht wurde erfolgreich empfangen : " + temp_receive);
            filter(shex, ssize);
        }
        else
        {
            //          Fehlgeschlagen
            Serial2.write(0x15);
            for (int i = begin; i < end + 2; i++)
            {
                letter = buff[i];
                temp_receive += letter;
            }
            Serial.println("Nachricht wurde abgelehnt : " + temp_receive);
        }
    }
}

void serial_status()
{
    // Output aktivieren/deaktivieren
    if (comserial.aktiv == true)
    {
        digitalWrite(output_comport_activ, LOW);
    }
    else
    {
        digitalWrite(output_comport_activ, HIGH);
    }
    
    // abfragen Seriele komunikation + Variable anpassen
    if (digitalRead(input_comport_activ) == 0)
    {
        comserial.gestartet = true;
    }
    else
    {
        comserial.gestartet = false;
    }
}
