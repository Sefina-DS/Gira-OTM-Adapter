#include "SysHeaders.h"

void alarm_group_diagnose(String msg)
{
    String number_str = "";
    int number = 999999;
    int size = 0;
    boolean mg = false;

    for (int i = 0; msg[i] != 0; i++)
    {
        if (msg[i] > 47 &&
            msg[i] < 58)
        {
            if (number == 999999)
            {
                number_str = String(msg[i] - 48);
                number = msg[i] - 48;
            }
            else
            {
                number_str += String(msg[i] - 48);
            }
        }
        else
        {
            if (number >= 0 &&
                number != 999999)
            {
                if (msg[i + 1] != 0)
                {
                    config.detector_alarm_group_int[size] = number_str.toInt();
                    if (config.detector_group == number_str)
                    {
                        mg = true;
                    }
                    size++;
                    number = 999999;
                }
            }
        }
    }
    config.detector_alarm_group_int[size] = number_str.toInt();
    if (mg == false &&
        config.detector_group != number_str)
    {
        size++;
        config.detector_alarm_group_int[size] = config.detector_group.toInt();
    }
    config.detector_alarm_group_size = size;
    for (int i = 0; i < config.detector_alarm_group_size + 1; i++)
    {
        Serial.print("Alarmierungsgruppen = Array- Nummer ");
        Serial.print(i);
        Serial.print(" : ");
        Serial.println(config.detector_alarm_group_int[i]);
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