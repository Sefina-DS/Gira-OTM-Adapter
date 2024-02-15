#include "SysHeaders.h"

COMSERIAL comserial;

void load_conf_serial(StaticJsonDocument<1024> doc)
{
    #ifdef DEBUG_SERIAL_DETECTOR
        Serial.println("... Serial- Variablen ...");
    #endif
    String temp;
    
    comserial.aktiv                = doc["serial"] | true;
}

StaticJsonDocument<1024> safe_conf_serial(StaticJsonDocument<1024> doc)
{
    #ifdef DEBUG_SERIAL_DETECTOR
        Serial.println("... Serial- Variablen ...");
    #endif
    doc["serial"]                   = comserial.aktiv;

    return doc;
}

void serial_setup()
{
    if ( comserial.aktiv ) {
        comserial.com.begin(9600, SERIAL_8N1, RXD2, TXD2);
        digitalWrite(output_comport_activ, LOW);
    }   else {
        digitalWrite(output_comport_activ, HIGH);
    }
}

void serial_receive() {
  //static String message_receive; // Variable zum Speichern der empfangenen Nachricht
  // Überprüfe, ob Daten verfügbar sind, bevor du sie liest
  while (comserial.com.available() > 0) {
    // Lies das nächste verfügbare Byte
    char receivedByte = comserial.com.read();
    // Überprüfe, ob das empfangene Byte das Startzeichen für eine Nachricht ist
    if (receivedByte == MESSAGE_START) {
      // Setze die Variable für die nächste Nachricht zurück
      comserial.message_receive = "";
    } else if (receivedByte == MESSAGE_END) {
      // Wenn ja, ist die Nachricht abgeschlossen
      // Überprüfe, ob die empfangene Nachricht eine gültige Nachricht ist
      if (serial_receive_check()) {
        // Wenn die Nachricht gültig ist, sende ein ACK_SUCCESS
        comserial.com.write(ACK_SUCCESS);
        // Verarbeite die empfangene Nachricht
        #ifdef DEBUG_SERIAL_DETECTOR
          Serial.print("Empfangene Nachricht: ");
          Serial.println(comserial.message_receive);
        #endif
      } else {
        // Wenn die Nachricht ungültig ist, sende ein ACK_FAILURE
        comserial.com.write(ACK_FAILURE);
      }
    } else {
      // Füge das empfangene Byte zur aktuellen Nachricht hinzu
      comserial.message_receive += receivedByte;
    }
  }
}

bool serial_receive_check() {
    // Überprüfen, ob die empfangene Nachricht leer ist
    if (comserial.message_receive.length() == 0) {
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.println("Keine empfangene Nachricht vorhanden");
        #endif
        return false;
    }

    // Größe der empfangenen Nachricht
    int message_length = comserial.message_receive.length();

    // Checksumme in der Nachricht
    byte received_checksum = strtol(comserial.message_receive.substring(message_length - 2).c_str(), NULL, 16);

    // Prüfsumme berechnen
    byte checksum = calculate_checksum(comserial.message_receive.substring(0, message_length - 2));

    // Erfolgreiche Nachrichten verarbeiten
    if (received_checksum == checksum) {
        // Erfolgreich
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.println("Nachricht wurde erfolgreich empfangen : " + comserial.message_receive.substring(0, message_length - 2));
        #endif
        serial_receive_filter(comserial.message_receive.substring(0, message_length - 2).c_str(), message_length - 2);
        return true;
    } else {
        // Fehlgeschlagen
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.println("Nachricht wurde abgelehnt : " + comserial.message_receive);
        #endif
        return false;
    }
}

byte calculate_checksum(String message) {
    int checksum = 0;
    for (int i = 0; i < message.length(); i++) {
        checksum += message[i];
    }

    byte checksum_byte = checksum & 0xFF;
    byte checksum_high = (checksum_byte >> 4) & 0x0F; // Die ersten vier Bits
    byte checksum_low = checksum_byte & 0x0F; // Die letzten vier Bits
    
    return checksum_byte;
}

bool serial_transceive(String message) {
    byte checksum = calculate_checksum(message);
    
    // Prüfung und Anpassung der Checksumme, um sicherzustellen, dass sie zwei Stellen hat
    String checksumHex = String(checksum, HEX); // Hier wird die Variable aktualisiert
    if (checksumHex.length() == 1) {
        checksumHex = "0" + checksumHex; // Führende Null hinzufügen
    }

    // Debug-Ausgabe, falls aktiviert
    #ifdef DEBUG_SERIAL_DETECTOR
        Serial.print("Nachricht wird versucht zu senden : ");
        Serial.print(message);
        Serial.print(" + ");
        checksumHex.toUpperCase(); // Prüfsumme in Großbuchstaben umwandeln
        Serial.println(checksumHex);
    #endif

    // Fügen Sie die Prüfsumme am Ende der Nachricht hinzu und hinzufügen von dem Start und End Byte
    String message_complet = String((char)MESSAGE_START) + message + checksumHex + String((char)MESSAGE_END);

    // Hier senden Sie die Nachricht über die serielle Verbindung
    for (int attempt = 0; attempt < ACK_MAX_RETRIES; attempt++) {
        // Nachricht senden
        comserial.com.write(message_complet.c_str());
        
        // Warten auf ACK
        unsigned long start_time = millis();
        while (millis() - start_time < ACK_TIMEOUT_MS) {
            if (comserial.com.available() > 0) {
                char ack = comserial.com.read();
                if (ack == ACK_SUCCESS) {
                    #ifdef DEBUG_SERIAL_DETECTOR
                        Serial.println("ACK erhalten");
                    #endif
                    return true; // ACK empfangen, Funktion verlassen
                }
            }
        }

        // Timeout für ACK erreicht
        #ifdef DEBUG_SERIAL_DETECTOR
            Serial.println("Timeout für ACK erreicht, Nachricht wird erneut gesendet");
        #endif
    }

    // Maximale Anzahl von Wiederholungen erreicht
    #ifdef DEBUG_SERIAL_DETECTOR
        Serial.println("Maximale Anzahl von Wiederholungen erreicht, Nachricht konnte nicht gesendet werden");
    #endif
    return false;
}

void serial_receive_filter(const char* message, int length) {
    // Hier kannst du den Code einfügen, um die Nachricht zu filtern oder zu verarbeiten
    // Beispiel: Ausgabe der empfangenen Nachricht
    serial_receive_diagnose(message);
    #ifdef DEBUG_SERIAL_DETECTOR
        Serial.print("Filtered Message: ");
        Serial.println(message);
    #endif
}