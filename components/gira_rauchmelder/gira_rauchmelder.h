#pragma once
//
// Gira Rauchmelder – UART-Anbindung für ESPHome
// ================================================
// Implementiert das Protokoll aus "Gira_Rauchmelder_URART_Dokumentation-alt.md"
// (Reverse-Engineering-Stand 2013-04-25, Korrektur 82).
//
// Zusätzlich zur reinen Protokollumsetzung enthält diese Komponente an mehreren
// Stellen gezielte Log-Ausgaben zu den in der Dokumentation als "offen" markierten
// Punkten (Abschnitt 17), damit sich deren Bedeutung durch Beobachtung im echten
// Betrieb (Home-Assistant-Log / ESPHome-Log, ggf. mit Zeitstempel gegen eigene
// Aktionen am Melder abgeglichen) nachträglich klären lässt:
//   - Byte 3 der Status-Antwort (02/82)               -> log_unknown_bits_()
//   - Bit 2, Bit 6, Bit 7 in Byte 1 (Flags)            -> log_unknown_bits_()
//   - Bit 1 in Byte 2 (Status)                         -> log_unknown_bits_()
//   - Bit 0,1,3,5,6,7 in Byte 4 (Defekt)                -> log_unknown_bits_()
//   - Kommando 08 (komplett unbekannt)                 -> handle_unknown_08_()
//   - Kommando 0F (nur grob mit Batteriespannung        -> handle_unknown_0f_()
//     korrelierbar)
//   - Kommando 86 (nur teilweise rekonstruiert)         -> handle_unknown_86_()
//   - jedes nicht in der Dokumentation gelistete
//     Antwort-Kommando                                  -> handle_unhandled_()
//
// Die Werte werden dabei sowohl geloggt als auch als Text-Sensoren (Rohwert-Hex)
// bereitgestellt, damit sie in Home Assistant historisiert und mit anderen
// Ereignissen (Homeassistant-Automationen, eigene Notizen etc.) korreliert
// werden können.

#include <cstdio>
#include <deque>
#include <string>
#include <vector>

#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace gira_rauchmelder {

static const char *const TAG = "gira_rauchmelder";

// Master -> Rauchmelder Abfragekommandos (Abschnitt 4 der Dokumentation)
enum Cmd : uint8_t {
  CMD_STATUS = 0x02,
  CMD_SERIAL = 0x04,
  CMD_UNKNOWN_08 = 0x08,
  CMD_UPTIME = 0x09,
  CMD_SMOKE_CHAMBER = 0x0B,
  CMD_BATTERY_TEMP = 0x0C,
  CMD_ALARM_COUNT = 0x0D,
  CMD_REMOTE_TEST_COUNT = 0x0E,
  CMD_UNKNOWN_0F = 0x0F,
};

class GiraRauchmelderComponent : public uart::UARTDevice, public PollingComponent {
 public:
  GiraRauchmelderComponent() = default;
  explicit GiraRauchmelderComponent(uart::UARTComponent *parent) : uart::UARTDevice(parent) {}

  // ---- Sensor-Setter (werden aus der YAML/den custom-Plattformen aufgerufen) ----
  void set_battery_voltage_sensor(sensor::Sensor *s) { battery_voltage_sensor_ = s; }
  void set_temperature1_sensor(sensor::Sensor *s) { temperature1_sensor_ = s; }
  void set_temperature2_sensor(sensor::Sensor *s) { temperature2_sensor_ = s; }
  void set_smoke_chamber_value_sensor(sensor::Sensor *s) { smoke_chamber_value_sensor_ = s; }
  void set_smoke_chamber_pollution_sensor(sensor::Sensor *s) { smoke_chamber_pollution_sensor_ = s; }
  void set_smoke_chamber_alarm_count_sensor(sensor::Sensor *s) { smoke_chamber_alarm_count_sensor_ = s; }
  void set_uptime_hours_sensor(sensor::Sensor *s) { uptime_hours_sensor_ = s; }
  void set_alarm_count_local_temp_sensor(sensor::Sensor *s) { alarm_count_local_temp_sensor_ = s; }
  void set_alarm_count_local_test_sensor(sensor::Sensor *s) { alarm_count_local_test_sensor_ = s; }
  void set_alarm_count_wired_sensor(sensor::Sensor *s) { alarm_count_wired_sensor_ = s; }
  void set_alarm_count_wireless_sensor(sensor::Sensor *s) { alarm_count_wireless_sensor_ = s; }
  void set_remote_test_count_wired_sensor(sensor::Sensor *s) { remote_test_count_wired_sensor_ = s; }
  void set_remote_test_count_wireless_sensor(sensor::Sensor *s) { remote_test_count_wireless_sensor_ = s; }

  void set_battery_low_binary_sensor(binary_sensor::BinarySensor *s) { battery_low_bs_ = s; }
  void set_battery_powered_binary_sensor(binary_sensor::BinarySensor *s) { battery_powered_bs_ = s; }
  void set_button_pressed_binary_sensor(binary_sensor::BinarySensor *s) { button_pressed_bs_ = s; }
  void set_defect_active_binary_sensor(binary_sensor::BinarySensor *s) { defect_active_bs_ = s; }
  void set_defect_temp1_binary_sensor(binary_sensor::BinarySensor *s) { defect_temp1_bs_ = s; }
  void set_defect_temp2_binary_sensor(binary_sensor::BinarySensor *s) { defect_temp2_bs_ = s; }
  void set_smoke_alarm_binary_sensor(binary_sensor::BinarySensor *s) { smoke_alarm_bs_ = s; }
  void set_wired_alarm_binary_sensor(binary_sensor::BinarySensor *s) { wired_alarm_bs_ = s; }
  void set_wireless_alarm_binary_sensor(binary_sensor::BinarySensor *s) { wireless_alarm_bs_ = s; }
  void set_local_test_alarm_binary_sensor(binary_sensor::BinarySensor *s) { local_test_alarm_bs_ = s; }
  void set_wired_test_alarm_binary_sensor(binary_sensor::BinarySensor *s) { wired_test_alarm_bs_ = s; }
  void set_wireless_test_alarm_binary_sensor(binary_sensor::BinarySensor *s) { wireless_test_alarm_bs_ = s; }

  void set_serial_number_text_sensor(text_sensor::TextSensor *s) { serial_number_ts_ = s; }
  void set_raw_status_byte3_text_sensor(text_sensor::TextSensor *s) { raw_status_byte3_ts_ = s; }
  void set_raw_cmd08_text_sensor(text_sensor::TextSensor *s) { raw_cmd08_ts_ = s; }
  void set_raw_cmd0f_text_sensor(text_sensor::TextSensor *s) { raw_cmd0f_ts_ = s; }
  void set_raw_cmd86_text_sensor(text_sensor::TextSensor *s) { raw_cmd86_ts_ = s; }
  void set_last_unhandled_frame_text_sensor(text_sensor::TextSensor *s) { last_unhandled_frame_ts_ = s; }

  // ---- ESPHome-Lifecycle ----
  void setup() override {
    ESP_LOGI(TAG, "Gira-Rauchmelder UART-Anbindung gestartet (9600 8N1).");
    ESP_LOGI(TAG, "Offene Punkte laut Dokumentation (Abschnitt 17) – werden per Log beobachtet:");
    ESP_LOGI(TAG, "  - Status Byte 3 (02/82): Bedeutung unbekannt");
    ESP_LOGI(TAG, "  - Byte1 Bit2/Bit6/Bit7, Byte2 Bit1, Byte4 Bit0/1/3/5/6/7: unbekannt");
    ESP_LOGI(TAG, "  - Kommando 08: komplett unbekannt (Registerwerte)");
    ESP_LOGI(TAG, "  - Kommando 0F: nur grob mit Batteriespannung korrelierbar");
    ESP_LOGI(TAG, "  - Kommando 86: nur teilweise rekonstruiert");
  }

  void update() override {
    // Einmal pro update_interval alle bekannten Abfragekommandos neu einreihen,
    // sofern die Warteschlange bereits abgearbeitet ist (verhindert unbegrenztes
    // Anwachsen, falls der Melder gerade nicht antwortet).
    if (this->poll_queue_.empty()) {
      this->poll_queue_.push_back(CMD_STATUS);
      this->poll_queue_.push_back(CMD_SERIAL);
      this->poll_queue_.push_back(CMD_UPTIME);
      this->poll_queue_.push_back(CMD_SMOKE_CHAMBER);
      this->poll_queue_.push_back(CMD_BATTERY_TEMP);
      this->poll_queue_.push_back(CMD_ALARM_COUNT);
      this->poll_queue_.push_back(CMD_REMOTE_TEST_COUNT);
      this->poll_queue_.push_back(CMD_UNKNOWN_08);
      this->poll_queue_.push_back(CMD_UNKNOWN_0F);
    }
  }

  void loop() override {
    // 1) eingehende Bytes verarbeiten (ACK/NAK/Frames) – jederzeit möglich,
    //    auch unaufgefordert gesendete 82-/86-Meldungen.
    uint8_t c;
    while (this->available()) {
      if (!this->read_byte(&c))
        break;
      this->process_byte_(c);
    }

    const uint32_t now = millis();

    // 2) Timeout-/Retry-Behandlung für laufende Abfrage
    if (this->tx_state_ != TxState::IDLE) {
      if (now - this->last_action_ms_ > this->response_timeout_ms_) {
        if (this->attempts_left_ > 0) {
          ESP_LOGD(TAG, "Kommando 0x%02X: keine/ungültige Antwort, Wiederholung (%d Versuch(e) übrig)",
                   this->current_cmd_, this->attempts_left_);
          this->do_send_(this->current_cmd_);
        } else {
          ESP_LOGW(TAG,
                   "Kommando 0x%02X: nach 3 Sendeversuchen keine gültige Antwort erhalten "
                   "(Abschnitt 3, Retry-Regel) – wird übersprungen.",
                   this->current_cmd_);
          this->tx_state_ = TxState::IDLE;
          this->last_action_ms_ = now;
        }
      }
      return;
    }

    // 3) nächstes Kommando aus der Warteschlange senden, mit kleinem Abstand
    if (!this->poll_queue_.empty() && (now - this->last_action_ms_ > this->inter_command_delay_ms_)) {
      uint8_t cmd = this->poll_queue_.front();
      this->poll_queue_.pop_front();
      this->current_cmd_ = cmd;
      this->attempts_left_ = 3;
      this->do_send_(cmd);
    }
  }

  float get_setup_priority() const override { return setup_priority::DATA; }

  // ---- Öffentliche Aktionen (experimentell, siehe README) ----
  // Diese Kommandos sind laut Dokumentation eigentlich "Funkmodul -> Rauchmelder"
  // (Abschnitt 6/8). Ob der Rauchmelder sie auch vom Master (diesem ESP) akzeptiert,
  // ist NICHT verifiziert. Bitte vorsichtig testen und das Ergebnis (Log!) zur
  // Dokumentation beitragen.
  void alarm_deactivate() { this->send_raw_command_(0x03, {0x00, 0x00}); }
  void alarm_trigger_wireless_test() { this->send_raw_command_(0x03, {0x00, 0x80}); }
  void finder_beep_on() { this->send_raw_command_(0x07, {0x00, 0x20}); }
  void finder_beep_off() { this->send_raw_command_(0x07, {0x00, 0x40}); }

  // Für gezieltes Debugging: sofort EIN einzelnes Status-Kommando (02) einreihen,
  // ohne auf den kompletten Abfragezyklus zu warten (an den Anfang der Warteschlange).
  void request_status() { this->poll_queue_.push_front(CMD_STATUS); }

  // ---- Getter für die "custom platform" Sensor-Arrays (siehe YAML-Beispiel) ----
  std::vector<sensor::Sensor *> get_sensors() {
    return {battery_voltage_sensor_,
            temperature1_sensor_,
            temperature2_sensor_,
            smoke_chamber_value_sensor_,
            smoke_chamber_pollution_sensor_,
            smoke_chamber_alarm_count_sensor_,
            uptime_hours_sensor_,
            alarm_count_local_temp_sensor_,
            alarm_count_local_test_sensor_,
            alarm_count_wired_sensor_,
            alarm_count_wireless_sensor_,
            remote_test_count_wired_sensor_,
            remote_test_count_wireless_sensor_};
  }

  std::vector<binary_sensor::BinarySensor *> get_binary_sensors() {
    return {battery_low_bs_,       battery_powered_bs_,     button_pressed_bs_,
            defect_active_bs_,     defect_temp1_bs_,        defect_temp2_bs_,
            smoke_alarm_bs_,       wired_alarm_bs_,         wireless_alarm_bs_,
            local_test_alarm_bs_,  wired_test_alarm_bs_,    wireless_test_alarm_bs_};
  }

  std::vector<text_sensor::TextSensor *> get_text_sensors() {
    return {serial_number_ts_,     raw_status_byte3_ts_, raw_cmd08_ts_,
            raw_cmd0f_ts_,         raw_cmd86_ts_,         last_unhandled_frame_ts_};
  }

 protected:
  // ================= Low-Level Framing =================
  static std::string to_hex_byte_(uint8_t b) {
    char buf[3];
    snprintf(buf, sizeof(buf), "%02X", b);
    return std::string(buf);
  }

  static std::string to_hex_(const std::vector<uint8_t> &bytes) {
    std::string out;
    out.reserve(bytes.size() * 2);
    for (uint8_t b : bytes)
      out += to_hex_byte_(b);
    return out;
  }

  static std::vector<uint8_t> from_hex_(const std::string &hex) {
    std::vector<uint8_t> out;
    out.reserve(hex.size() / 2);
    for (size_t i = 0; i + 1 < hex.size(); i += 2) {
      out.push_back((uint8_t) strtol(hex.substr(i, 2).c_str(), nullptr, 16));
    }
    return out;
  }

  // Prüfsumme laut Abschnitt 2: ASCII-Summe der Hex-Zeichen des Payloads & 0xFF
  static uint8_t checksum_(const std::string &hex_payload) {
    uint32_t sum = 0;
    for (char ch : hex_payload)
      sum += (uint8_t) ch;
    return (uint8_t) (sum & 0xFF);
  }

  void send_ack_() {
    this->write_byte(0x06);
    ESP_LOGI(TAG, "ESP32 -> Rauchmelder: ACK");
  }
  void send_nak_() {
    this->write_byte(0x15);
    ESP_LOGI(TAG, "ESP32 -> Rauchmelder: NAK");
  }

  // Sendet <STX> hex(cmd) [weitere Datenbytes als Hex] Checksumme <ETX>
  void send_raw_command_(uint8_t cmd, const std::vector<uint8_t> &extra_data = {}) {
    std::vector<uint8_t> data{cmd};
    data.insert(data.end(), extra_data.begin(), extra_data.end());
    std::string hex_payload = to_hex_(data);
    std::string cs = to_hex_byte_(checksum_(hex_payload));

    this->write_byte(0x02);  // STX
    for (char ch : hex_payload)
      this->write_byte((uint8_t) ch);
    for (char ch : cs)
      this->write_byte((uint8_t) ch);
    this->write_byte(0x03);  // ETX

    ESP_LOGI(TAG, "ESP32 -> Rauchmelder: <STX>%s%s<ETX>", hex_payload.c_str(), cs.c_str());
  }

  void do_send_(uint8_t cmd) {
    this->send_raw_command_(cmd);
    this->tx_state_ = TxState::WAIT_ACK;
    this->last_action_ms_ = millis();
    if (this->attempts_left_ > 0)
      this->attempts_left_--;
  }

  // ================= Empfang =================
  enum class RxState { IDLE, IN_FRAME };
  enum class TxState { IDLE, WAIT_ACK, WAIT_RESPONSE };

  void process_byte_(uint8_t c) {
    if (this->rx_state_ == RxState::IDLE) {
      if (c == 0x06) {  // ACK
        ESP_LOGI(TAG, "Rauchmelder -> ESP32: ACK");
        this->handle_ack_();
      } else if (c == 0x15) {  // NAK
        ESP_LOGI(TAG, "Rauchmelder -> ESP32: NAK");
        this->handle_nak_();
      } else if (c == 0x02) {  // STX -> neuer Frame beginnt
        this->rx_buffer_.clear();
        this->rx_state_ = RxState::IN_FRAME;
      } else if (c == 0x00) {
        // Füllbyte zwischen ACK und Folgenachricht (Abschnitt 3) – ignorieren
      } else {
        ESP_LOGW(TAG, "[Doku-Lücke] Rauchmelder -> ESP32: unerwartetes Byte 0x%02X außerhalb eines Frames", c);
      }
      return;
    }

    // RxState::IN_FRAME
    if (c == 0x03) {  // ETX -> Frame komplett
      this->handle_frame_(this->rx_buffer_);
      this->rx_state_ = RxState::IDLE;
    } else if (c == 0x02) {
      // Laut Abschnitt 3 kann robustes Parsen nötig sein – im Zweifel neu beginnen
      ESP_LOGW(TAG, "Erneutes STX mitten im Frame – vorherige Daten verworfen: %s",
               this->rx_buffer_.c_str());
      this->rx_buffer_.clear();
    } else {
      this->rx_buffer_ += (char) c;
    }
  }

  void handle_ack_() {
    if (this->tx_state_ == TxState::WAIT_ACK) {
      this->tx_state_ = TxState::WAIT_RESPONSE;
      this->last_action_ms_ = millis();
    }
    // ACK auf ein von uns gesendetes ACK/Antwortpaar wird ignoriert (kommt nicht vor)
  }

  void handle_nak_() {
    if (this->tx_state_ == TxState::WAIT_ACK || this->tx_state_ == TxState::WAIT_RESPONSE) {
      ESP_LOGD(TAG, "NAK für Kommando 0x%02X erhalten", this->current_cmd_);
      if (this->attempts_left_ > 0) {
        this->do_send_(this->current_cmd_);
      } else {
        this->tx_state_ = TxState::IDLE;
      }
    }
  }

  void handle_frame_(const std::string &hex_string) {
    ESP_LOGI(TAG, "Rauchmelder -> ESP32: <STX>%s<ETX>", hex_string.c_str());

    if (hex_string.size() < 4 || hex_string.size() % 2 != 0) {
      ESP_LOGW(TAG, "Ungültiger Frame (Länge %d): %s – NAK gesendet", (int) hex_string.size(), hex_string.c_str());
      this->send_nak_();
      return;
    }

    std::string body_hex = hex_string.substr(0, hex_string.size() - 2);
    std::string cs_hex = hex_string.substr(hex_string.size() - 2, 2);
    uint8_t given_cs = (uint8_t) strtol(cs_hex.c_str(), nullptr, 16);
    uint8_t calc_cs = checksum_(body_hex);

    if (given_cs != calc_cs) {
      ESP_LOGW(TAG, "Prüfsummenfehler: Frame=%s erwartet=%02X berechnet=%02X – NAK gesendet", hex_string.c_str(),
               given_cs, calc_cs);
      this->send_nak_();
      return;
    }

    // gültiger Frame -> bestätigen
    this->send_ack_();

    std::vector<uint8_t> body = from_hex_(body_hex);
    uint8_t cmd = body[0];
    std::vector<uint8_t> data(body.begin() + 1, body.end());

    ESP_LOGI(TAG, "Rauchmelder -> ESP32: Kommando=0x%02X Daten=%s (dekodiert)", cmd, to_hex_(data).c_str());

    switch (cmd) {
      case 0xC2:
        this->handle_status_(data, false);
        break;
      case 0x82:
        this->handle_status_(data, true);
        break;
      case 0xC4:
        this->handle_serial_(data);
        break;
      case 0xC9:
        this->handle_uptime_(data);
        break;
      case 0xCB:
        this->handle_smoke_chamber_(data);
        break;
      case 0xCC:
        this->handle_battery_temp_(data);
        break;
      case 0xCD:
        this->handle_alarm_count_(data);
        break;
      case 0xCE:
        this->handle_remote_test_count_(data);
        break;
      case 0xC8:
        this->handle_unknown_08_(data);
        break;
      case 0xCF:
        this->handle_unknown_0f_(data);
        break;
      case 0x86:
        this->handle_unknown_86_(data);
        break;
      default:
        this->handle_unhandled_(cmd, data);
        break;
    }

    // Falls das die erwartete Antwort auf unser laufendes Kommando war -> weiter
    if (this->tx_state_ == TxState::WAIT_RESPONSE && cmd == (this->current_cmd_ | 0xC0)) {
      this->tx_state_ = TxState::IDLE;
      this->last_action_ms_ = millis();
    }
  }

  // ================= Kommando-Handler =================
  void handle_status_(const std::vector<uint8_t> &data, bool unsolicited) {
    if (data.size() != 4) {
      ESP_LOGW(TAG, "Status-Antwort: unerwartete Länge %d (erwartet 4) – Rohdaten=%s", (int) data.size(),
               to_hex_(data).c_str());
      return;
    }
    uint8_t flags = data[0], status = data[1], byte3 = data[2], defekt = data[3];
    ESP_LOGD(TAG, "Status (%s): Flags=%02X Status=%02X Byte3=%02X Defekt=%02X", unsolicited ? "82 unaufgefordert" : "02-Antwort",
             flags, status, byte3, defekt);

    this->publish_bs_(this->battery_powered_bs_, flags & 0x20);
    this->publish_bs_(this->button_pressed_bs_, flags & 0x08);
    this->publish_bs_(this->defect_active_bs_, flags & 0x02);

    this->publish_bs_(this->battery_low_bs_, status & 0x01);
    this->publish_bs_(this->smoke_alarm_bs_, status & 0x04);
    this->publish_bs_(this->wired_alarm_bs_, status & 0x08);
    this->publish_bs_(this->wireless_alarm_bs_, status & 0x10);
    this->publish_bs_(this->local_test_alarm_bs_, status & 0x20);
    this->publish_bs_(this->wired_test_alarm_bs_, status & 0x40);
    this->publish_bs_(this->wireless_test_alarm_bs_, status & 0x80);

    this->publish_bs_(this->defect_temp1_bs_, defekt & 0x04);
    this->publish_bs_(this->defect_temp2_bs_, defekt & 0x10);

    this->log_unknown_bits_(flags, status, byte3, defekt);
  }

  // "Offene Punkte"-Logger: meldet nur bei Änderung, um das Log nicht zu fluten.
  void log_unknown_bits_(uint8_t flags, uint8_t status, uint8_t byte3, uint8_t defekt) {
    if (!this->have_last_status_) {
      this->have_last_status_ = true;
      this->last_flags_ = flags;
      this->last_status_ = status;
      this->last_byte3_ = byte3;
      this->last_defekt_ = defekt;
    }

    if (byte3 != this->last_byte3_) {
      ESP_LOGW(TAG, "[Doku-Lücke, Abschnitt 17] Status Byte3 (Bedeutung unbekannt) geändert: %02X -> %02X", this->last_byte3_,
               byte3);
      if (this->raw_status_byte3_ts_ != nullptr)
        this->raw_status_byte3_ts_->publish_state(to_hex_byte_(byte3));
    }

    uint8_t unknown_flags_mask = 0x04 | 0x40 | 0x80;  // Bit2, Bit6, Bit7 von Byte1
    if ((flags & unknown_flags_mask) != (this->last_flags_ & unknown_flags_mask)) {
      ESP_LOGW(TAG, "[Doku-Lücke, Abschnitt 17] Byte1 (Flags) unbekannte Bits geändert: %02X -> %02X (Maske %02X)",
               this->last_flags_, flags, unknown_flags_mask);
    }

    uint8_t unknown_status_mask = 0x02;  // Bit1 von Byte2
    if ((status & unknown_status_mask) != (this->last_status_ & unknown_status_mask)) {
      ESP_LOGW(TAG, "[Doku-Lücke, Abschnitt 17] Byte2 (Status) unbekanntes Bit1 geändert: %02X -> %02X", this->last_status_,
               status);
    }

    uint8_t unknown_defekt_mask = 0x01 | 0x02 | 0x08 | 0x20 | 0x40 | 0x80;
    if ((defekt & unknown_defekt_mask) != (this->last_defekt_ & unknown_defekt_mask)) {
      ESP_LOGW(TAG, "[Doku-Lücke, Abschnitt 17] Byte4 (Defekt) unbekannte Bits geändert: %02X -> %02X (Maske %02X)",
               this->last_defekt_, defekt, unknown_defekt_mask);
    }

    this->last_flags_ = flags;
    this->last_status_ = status;
    this->last_byte3_ = byte3;
    this->last_defekt_ = defekt;
  }

  void handle_serial_(const std::vector<uint8_t> &data) {
    if (data.size() != 4) {
      ESP_LOGW(TAG, "Seriennummer-Antwort: unerwartete Länge %d", (int) data.size());
      return;
    }
    std::string serial = to_hex_(data);
    ESP_LOGD(TAG, "Seriennummer: %s", serial.c_str());
    if (this->serial_number_ts_ != nullptr)
      this->serial_number_ts_->publish_state(serial);
  }

  void handle_uptime_(const std::vector<uint8_t> &data) {
    if (data.size() != 4) {
      ESP_LOGW(TAG, "Betriebszeit-Antwort: unerwartete Länge %d", (int) data.size());
      return;
    }
    uint32_t quarters = ((uint32_t) data[0] << 24) | ((uint32_t) data[1] << 16) | ((uint32_t) data[2] << 8) | data[3];
    float hours = quarters / 4.0f / 3600.0f;
    ESP_LOGD(TAG, "Betriebszeit: %.2f h (%u x 1/4s)", hours, quarters);
    if (this->uptime_hours_sensor_ != nullptr)
      this->uptime_hours_sensor_->publish_state(hours);
  }

  void handle_smoke_chamber_(const std::vector<uint8_t> &data) {
    if (data.size() != 4) {
      ESP_LOGW(TAG, "Rauchkammer-Antwort: unerwartete Länge %d", (int) data.size());
      return;
    }
    uint16_t raw = ((uint16_t) data[0] << 8) | data[1];
    float scaled = raw * 0.003223f;
    uint8_t alarm_count = data[2];
    uint8_t pollution = data[3];
    ESP_LOGD(TAG, "Rauchkammer: roh=%u skaliert=%.4f Alarme=%u Verschmutzung=%u", raw, scaled, alarm_count, pollution);
    if (this->smoke_chamber_value_sensor_ != nullptr)
      this->smoke_chamber_value_sensor_->publish_state(scaled);
    if (this->smoke_chamber_alarm_count_sensor_ != nullptr)
      this->smoke_chamber_alarm_count_sensor_->publish_state(alarm_count);
    if (this->smoke_chamber_pollution_sensor_ != nullptr)
      this->smoke_chamber_pollution_sensor_->publish_state(pollution);
  }

  void handle_battery_temp_(const std::vector<uint8_t> &data) {
    if (data.size() != 4) {
      ESP_LOGW(TAG, "Batterie/Temperatur-Antwort: unerwartete Länge %d", (int) data.size());
      return;
    }
    uint16_t raw_v = ((uint16_t) data[0] << 8) | data[1];
    // Umrechnung laut Abschnitt 12 – Einheit (V vs. cV) im Original nicht zweifelsfrei
    // belegt, hier als Volt direkt interpretiert (x * 0,018368).
    float voltage = raw_v * 0.018368f;
    float temp1 = data[2] / 2.0f - 20.0f;
    float temp2 = data[3] / 2.0f - 20.0f;
    ESP_LOGD(TAG, "Batterie/Temperatur: U=%.3fV T1=%.1f°C T2=%.1f°C (roh U=%u)", voltage, temp1, temp2, raw_v);
    this->last_known_voltage_ = voltage;
    this->have_last_known_voltage_ = true;
    if (this->battery_voltage_sensor_ != nullptr)
      this->battery_voltage_sensor_->publish_state(voltage);
    if (this->temperature1_sensor_ != nullptr)
      this->temperature1_sensor_->publish_state(temp1);
    if (this->temperature2_sensor_ != nullptr)
      this->temperature2_sensor_->publish_state(temp2);
  }

  void handle_alarm_count_(const std::vector<uint8_t> &data) {
    if (data.size() < 4) {
      ESP_LOGW(TAG, "Alarmzähler-Antwort: unerwartete Länge %d", (int) data.size());
      return;
    }
    ESP_LOGD(TAG, "Alarmzähler: lok.Temp=%u lok.Test=%u Kabel=%u Funk=%u", data[0], data[1], data[2], data[3]);
    if (this->alarm_count_local_temp_sensor_ != nullptr)
      this->alarm_count_local_temp_sensor_->publish_state(data[0]);
    if (this->alarm_count_local_test_sensor_ != nullptr)
      this->alarm_count_local_test_sensor_->publish_state(data[1]);
    if (this->alarm_count_wired_sensor_ != nullptr)
      this->alarm_count_wired_sensor_->publish_state(data[2]);
    if (this->alarm_count_wireless_sensor_ != nullptr)
      this->alarm_count_wireless_sensor_->publish_state(data[3]);
  }

  void handle_remote_test_count_(const std::vector<uint8_t> &data) {
    if (data.size() < 2) {
      ESP_LOGW(TAG, "Remote-Test-Alarm-Antwort: unerwartete Länge %d", (int) data.size());
      return;
    }
    ESP_LOGD(TAG, "Remote-Test-Alarme: Kabel=%u Funk=%u", data[0], data[1]);
    if (this->remote_test_count_wired_sensor_ != nullptr)
      this->remote_test_count_wired_sensor_->publish_state(data[0]);
    if (this->remote_test_count_wireless_sensor_ != nullptr)
      this->remote_test_count_wireless_sensor_->publish_state(data[1]);
  }

  void handle_unknown_08_(const std::vector<uint8_t> &data) {
    std::string hex = to_hex_(data);
    if (hex != this->last_cmd08_hex_) {
      ESP_LOGW(TAG, "[Doku-Lücke, Abschnitt 17] Kommando 08 (komplett unbekannt) Rohwert=%s (vorher %s)", hex.c_str(),
               this->last_cmd08_hex_.c_str());
      this->last_cmd08_hex_ = hex;
      if (this->raw_cmd08_ts_ != nullptr)
        this->raw_cmd08_ts_->publish_state(hex);
    }
  }

  void handle_unknown_0f_(const std::vector<uint8_t> &data) {
    std::string hex = to_hex_(data);
    if (hex != this->last_cmd0f_hex_) {
      if (this->have_last_known_voltage_) {
        ESP_LOGW(TAG,
                 "[Doku-Lücke, Abschnitt 15/17] Kommando 0F Rohwert=%s (vorher %s) – zuletzt bekannte "
                 "Batteriespannung=%.3fV, bitte korrelieren",
                 hex.c_str(), this->last_cmd0f_hex_.c_str(), this->last_known_voltage_);
      } else {
        ESP_LOGW(TAG, "[Doku-Lücke, Abschnitt 15/17] Kommando 0F Rohwert=%s (vorher %s), Batteriespannung noch unbekannt",
                 hex.c_str(), this->last_cmd0f_hex_.c_str());
      }
      this->last_cmd0f_hex_ = hex;
      if (this->raw_cmd0f_ts_ != nullptr)
        this->raw_cmd0f_ts_->publish_state(hex);
    }
  }

  void handle_unknown_86_(const std::vector<uint8_t> &data) {
    std::string hex = to_hex_(data);
    ESP_LOGW(TAG,
             "[Doku-Lücke, Abschnitt 16] Kommando 86 empfangen, Rohdaten=%s – laut Dokumentation bisher nur bei "
             "'Taste 1s gedrückt nach Batteriewechsel unter Stützspannung' beobachtet",
             hex.c_str());
    if (this->raw_cmd86_ts_ != nullptr)
      this->raw_cmd86_ts_->publish_state(hex);
  }

  void handle_unhandled_(uint8_t cmd, const std::vector<uint8_t> &data) {
    std::string hex = to_hex_(data);
    ESP_LOGW(TAG,
             "[Doku-Lücke, Abschnitt 4] Unbekanntes/undokumentiertes Antwort-Kommando 0x%02X empfangen, Daten=%s – "
             "bitte für die Dokumentation notieren",
             cmd, hex.c_str());
    if (this->last_unhandled_frame_ts_ != nullptr) {
      char buf[8];
      snprintf(buf, sizeof(buf), "%02X:", cmd);
      this->last_unhandled_frame_ts_->publish_state(std::string(buf) + hex);
    }
  }

  static void publish_bs_(binary_sensor::BinarySensor *bs, bool value) {
    if (bs != nullptr)
      bs->publish_state(value);
  }

  // ================= Zustand =================
  RxState rx_state_{RxState::IDLE};
  std::string rx_buffer_;

  TxState tx_state_{TxState::IDLE};
  std::deque<uint8_t> poll_queue_;
  uint8_t current_cmd_{0};
  uint8_t attempts_left_{0};
  uint32_t last_action_ms_{0};
  const uint32_t response_timeout_ms_{800};
  const uint32_t inter_command_delay_ms_{150};

  bool have_last_status_{false};
  uint8_t last_flags_{0}, last_status_{0}, last_byte3_{0}, last_defekt_{0};
  std::string last_cmd08_hex_;
  std::string last_cmd0f_hex_;
  bool have_last_known_voltage_{false};
  float last_known_voltage_{0};

  // ---- Sensor-Pointer ----
  sensor::Sensor *battery_voltage_sensor_{nullptr};
  sensor::Sensor *temperature1_sensor_{nullptr};
  sensor::Sensor *temperature2_sensor_{nullptr};
  sensor::Sensor *smoke_chamber_value_sensor_{nullptr};
  sensor::Sensor *smoke_chamber_pollution_sensor_{nullptr};
  sensor::Sensor *smoke_chamber_alarm_count_sensor_{nullptr};
  sensor::Sensor *uptime_hours_sensor_{nullptr};
  sensor::Sensor *alarm_count_local_temp_sensor_{nullptr};
  sensor::Sensor *alarm_count_local_test_sensor_{nullptr};
  sensor::Sensor *alarm_count_wired_sensor_{nullptr};
  sensor::Sensor *alarm_count_wireless_sensor_{nullptr};
  sensor::Sensor *remote_test_count_wired_sensor_{nullptr};
  sensor::Sensor *remote_test_count_wireless_sensor_{nullptr};

  binary_sensor::BinarySensor *battery_low_bs_{nullptr};
  binary_sensor::BinarySensor *battery_powered_bs_{nullptr};
  binary_sensor::BinarySensor *button_pressed_bs_{nullptr};
  binary_sensor::BinarySensor *defect_active_bs_{nullptr};
  binary_sensor::BinarySensor *defect_temp1_bs_{nullptr};
  binary_sensor::BinarySensor *defect_temp2_bs_{nullptr};
  binary_sensor::BinarySensor *smoke_alarm_bs_{nullptr};
  binary_sensor::BinarySensor *wired_alarm_bs_{nullptr};
  binary_sensor::BinarySensor *wireless_alarm_bs_{nullptr};
  binary_sensor::BinarySensor *local_test_alarm_bs_{nullptr};
  binary_sensor::BinarySensor *wired_test_alarm_bs_{nullptr};
  binary_sensor::BinarySensor *wireless_test_alarm_bs_{nullptr};

  text_sensor::TextSensor *serial_number_ts_{nullptr};
  text_sensor::TextSensor *raw_status_byte3_ts_{nullptr};
  text_sensor::TextSensor *raw_cmd08_ts_{nullptr};
  text_sensor::TextSensor *raw_cmd0f_ts_{nullptr};
  text_sensor::TextSensor *raw_cmd86_ts_{nullptr};
  text_sensor::TextSensor *last_unhandled_frame_ts_{nullptr};
};

}  // namespace gira_rauchmelder
}  // namespace esphome
