#include "network-mqtt.hpp"

void mqtt_filter(String topic, String msg)
{
    boolean msg_bool;
    if (msg == "false")
    {
        msg_bool = false;
    }
    if (msg == "true")
    {
        msg_bool = true;
    }
    Serial.print("das Topic : ");
    Serial.print(topic);
    Serial.print(" || die Message : ");
    Serial.println(msg);

    if (topic == config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_control + "Melder_Finden" ||
        topic == config.mqtt_topic_base + "/" + group_control + config.detector_group + "/" + "Melder_Finden")
    {
        if (seri_status == 0)
        {
            seri_status = 1;
            if (msg_bool == true)
            {
                serial_send("070020"); // Melder- Finden An
            }
            else
            {
                serial_send("070040"); // Melder- Finden Aus
            }
        }
    }
    if (topic == config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_control + "Testalarm-Funk" ||
        topic == config.mqtt_topic_base + "/" + group_control + config.detector_group + "/" + "Testalarm")
    {
        if (seri_status == 0)
        {
            seri_status = 1;
            if (msg_bool == true)
            {
                serial_send("030080"); // Test- Alarm
            }
            else
            {
                serial_send("030200"); // Reset Funk-Alarm + Test-Alarm
            }
        }
    }
    if (topic == config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_control + "Alarm-Funk" || 
    topic == config.mqtt_topic_base + "/" + group_control + config.detector_group + "/" + "Alarm")
    {
        if (seri_status == 0)
        {
            seri_status = 1;
            if (msg_bool == true)
            {
                if (timer_alarm < millis())
                {
                    timer_alarm = millis() + 300000;
                    serial_send("030210"); // Alarm Funk
                }
            }
            else
            {
                serial_send("030200"); // Reset Funk-Alarm + Test-Alarm
            }
        }
    }

    if (topic == config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_control + "Reset_Test/Funk_Alarme" &&
        seri_status == 0)
    {
        seri_status = 1;
        if (msg_bool == true)
        {
            serial_send("030000");
            client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_control + "Reset_Test/Funk_Alarme").c_str(), "false");
        }
    }

    if (topic == config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_control + "Das_ist_ein_Test" &&
        seri_status == 0)
    {
        seri_status = 1;
        if (msg == "1")
        {
            serial_send("030080"); // Wireless Testalarm über Funkmodul (Q), RM sendet dann 82 20 80 00 00 F4
        }
        if (msg == "2")
        {
            serial_send("030000"); // Alarm deaktivieren, auch Testalarm
        }
        if (msg == "3")
        {
            serial_send("030210"); // Wireless Alarm (über Funk), RM sendet dann 82 20 10 00 00 ED
        }
        if (msg == "4")
        {
            serial_send("030280"); // Wireless Testalarm (über Funk), RM sendet dann 82 20 80 00 00 F4
        }
        if (msg == "5")
        {
            serial_send("030200"); // Wireless Alarm / Testalarm beendet
            Serial2.write(0x02);
            Serial2.write("03021026");
            Serial2.write(0x03);
            seri_status = 0;
        }
        if (msg == "6")
        {
            serial_send("030200"); // Wireless Alarm / Testalarm beendet
            seri_status = 0;
        }
        client.publish((config.mqtt_topic_base + "/" + config.mqtt_topic_define + "/" + detector_control + "Das_ist_ein_Test").c_str(), "0");
    }
}
