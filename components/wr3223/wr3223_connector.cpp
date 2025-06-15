#include "wr3223_connector.h"
#include "esphome/core/log.h"
#include <vector>

namespace esphome
{
  namespace wr3223
  {

    static const char *TAG = "wr3223_connector";

    // Definition der globalen Variablen (eine einzige Definition in dieser Datei)
    /// @brief Gibt an, ob ein frischer Start nach einem Stromausfall erfolgt
    bool freshStart = true;

    /// @brief Wird über den RelaisDecoder aktualisiert
    bool bedienteilAktiv = true;

    static std::string bytes_to_hex(const uint8_t *data, size_t len)
    {
      char buf[4];
      std::string out;
      for (size_t i = 0; i < len; i++)
      {
        snprintf(buf, sizeof(buf), "%02X ", data[i]);
        out += buf;
      }
      return out;
    }

    /// @brief Berechnet die Checksumme einer Antwort.
    /// @param answer Die Antwort als Byte-Array
    /// @param start Startposition
    /// @param end Endposition
    /// @return Checksumme als Integer oder `-1` bei ungültigen Parametern.
    int WR3223Connector::buildCheckSum(const char *answer, int start, int end)
    {
      // Sicherheitsprüfung: Ist der Bereich gültig?
      if (start < 0 || end < start)
      {
        ESP_LOGE("WR3223", "Ungültige Checksum-Berechnung! Start: %d, End: %d",
                 start, end);
        return -1;
      }

      // Startwert der Checksumme setzen (als uint8_t für Sicherheit)
      uint8_t chkSum = static_cast<uint8_t>(answer[start]);

      // XOR-Berechnung über den gesamten Bereich
      for (int i = start + 1; i <= end; i++)
      {
        chkSum ^= static_cast<uint8_t>(answer[i]);
      }

      return chkSum;
    }

    /// @brief Liest eine Antwort vom Gerät bis zum ETX-Zeichen und speichert die
    /// Checksumme.
    /// @param buffer Der Buffer zum Speichern der gelesenen Daten
    /// @param len Maximale Länge des Buffers
    /// @return Anzahl der gelesenen Zeichen (inkl. ETX & Checksumme), oder `-1` bei
    /// Fehlern
    int WR3223Connector::readAnswer(char *buffer, int len)
    {
      int pos = 0;
      uint32_t start_time = millis(); // Timeout-Überwachung

      bool start_detected = false;

      while (millis() - start_time < 500)
      { // Timeout nach 500ms
        if (!available())
        {
          continue; // Warten, bis Daten verfügbar sind
        }

        int readch = read();

        if (readch < 0)
        {
          continue; // Falls kein gültiges Zeichen, nochmal versuchen
        }

        // ESP_LOGD(TAG, "Empfangen: Zeichen [%d]: 0x%02X", pos, readch); //
        // Debugging

        if (!start_detected)
        {
          if (readch == MessageControl::STX)
          {
            start_detected = true;
            buffer[pos++] = readch; // <STX> speichern
          }
          continue;
        }

        // Falls Nachricht zu lang wird, abbrechen
        if (pos >= len - 2)
        { // -2 damit noch Platz für \0 ist
          ESP_LOGE(TAG, "Antwort ist zu lang! Abbruch.");
          buffer[pos] = '\0';
          return -1;
        }

        buffer[pos++] = readch;

        // Falls wir <ETX> erreichen, lesen wir noch die Checksumme
        if (readch == MessageControl::ETX)
        {
          if (!available())
          {
            ESP_LOGE(TAG,
                     "Antwort endet mit <ETX>, aber keine Checksumme gefunden!");
            buffer[pos] = '\0'; // Nullterminierung
            return -1;
          }

          buffer[pos++] = read(); // Letztes Zeichen = Checksumme
          buffer[pos] = '\0';     // Null-Terminator für Sicherheit
          return pos;             // Erfolgreich
        }
      }

      ESP_LOGE(TAG, "Timeout beim Lesen der Antwort!");
      return -1; // Falls Timeout erreicht wurde
    }

    void WR3223Connector::send_request(const char *command, std::function<void(char *answer, bool success)> callback)
    {
      std::string cmd_key(command); // `std::string` aus `char *` erstellen

      // Prüfen, ob das Kommando bereits existiert
      if (request_map_.count(cmd_key))
      {
        ESP_LOGW(TAG,
                 "Kommando %s ist bereits in der Queue – kein erneutes Hinzufügen.",
                 command);
        return;
      }

      // Neues Callback-Info-Objekt zur Map hinzufügen
      CallbackInfo info{};
      info.last_sent = millis();
      info.callback = callback;
      info.is_write = false;
      info.data = "";
      request_map_[cmd_key] = info;

      ESP_LOGD(TAG, "Neues Kommando %s zur Queue hinzugefügt (gesamt %u).", command,
               (unsigned)request_map_.size());
    }

    void WR3223Connector::send_write_request(const char *command, const std::string &data, std::function<void(char *answer, bool success)> callback)
    {
      std::string cmd_key(command);
      if (request_map_.count(cmd_key))
      {
        ESP_LOGW(TAG, "Kommando %s ist bereits in der Queue – kein erneutes Hinzufügen.", command);
        return;
      }
      CallbackInfo info{};
      info.last_sent = millis();
      info.callback = callback;
      info.is_write = true;
      info.data = data;
      request_map_[cmd_key] = info;
      ESP_LOGD(TAG,
               "Neues Schreibkommando %s=%s zur Queue hinzugefügt (gesamt %u).",
               command, data.c_str(), (unsigned)request_map_.size());
    }

    void WR3223Connector::setup()
    {
      ESP_LOGI(TAG, "WR3223Connector wurde als Komponente registriert.");
    }

    void WR3223Connector::loop()
    {
      bool has_data = available();
      ESP_LOGV(TAG, "Loop: cmd=%s available=%d queue=%u", current_command_.c_str(),
               has_data, (unsigned)request_map_.size());

      if (!has_data)
      {
        // Falls ein Kommando zu lange keine Antwort bekam, zurücksetzen
        if (!current_command_.empty() &&
            millis() - request_map_[current_command_].last_sent > 1000)
        {
          ESP_LOGW(TAG, "Timeout für %s!", current_command_.c_str());
          error_count_map_[current_command_]++;
          current_command_.clear();
        }

        // Falls kein aktives Kommando mehr läuft, das nächste senden
        if (current_command_.empty() && !request_map_.empty())
        {
          send_next_request();
        }
        return;
      }

      bool is_write = false;
      auto it_req = request_map_.find(current_command_);
      if (it_req != request_map_.end())
        is_write = it_req->second.is_write;

      if (is_write)
      {
        // Bei Schreibkommandos gibt es nur ein einzelnes ACK/NAK-Byte
        int resp_byte = read();
        ESP_LOGD(TAG, "Empfangenes ACK/NAK-Byte: 0x%02X", resp_byte);
        const char *resp = (resp_byte == MessageControl::ACK) ? "ACK" : "NAK";
        it_req->second.callback((char *)resp, resp_byte == MessageControl::ACK);
        ESP_LOGD(TAG, "Schreibantwort fuer %s: %s", current_command_.c_str(), resp);
        request_map_.erase(it_req);
        error_count_map_[current_command_] = 0;
        current_command_.clear();
        while (available())
          read();
        return;
      }

      char answer_buffer[16]; // Temporärer Buffer
      int answer_length = readAnswer(answer_buffer, sizeof(answer_buffer));

      if (answer_length < 3)
      {
        ESP_LOGW(TAG, "Ungültige Antwort empfangen - wird ignoriert.");
        error_count_map_[current_command_]++;
        current_command_.clear();
        return;
      }

      // Checksumme aus dem Buffer berechnen
      int calculated_checksum = buildCheckSum(answer_buffer, 1, answer_length - 2);
      int received_checksum =
          answer_buffer[answer_length - 1]; // Letztes Zeichen ist die Checksumme

      if (received_checksum != calculated_checksum)
      {
        ESP_LOGE(TAG, "Checksumme fehlerhaft! Erwartet %x, erhalten %x.", calculated_checksum, received_checksum);
        error_count_map_[current_command_]++;
        current_command_.clear();
        return;
      }

      // Kommando aus der Antwort extrahieren
      std::string received_cmd(answer_buffer + 1, 2);

      // Prüfen, ob ein passender Request existiert
      if (request_map_.count(received_cmd))
      {

        // Nur den Datenbereich extrahieren
        char *data = answer_buffer + 3;      // Daten beginnen nach C1 und C2
        int data_length = answer_length - 5; // Daten bis vor ETX (-5 durch STX, C1, C2, ETX, Checksumme)

        if (data_length > 0)
        {
          data[data_length] = '\0'; // Null-Terminator als Ende setzen (Checksumme und ETX brauchen wir nicht)
        }
        else
        {
          data = (char *)""; // Falls keine Daten, leere Zeichenkette nutzen
        }

        // Callback nur mit Datenbereich aufrufen
        request_map_[received_cmd].callback(data, true);

        ESP_LOGD(TAG, "Antwort für %s verarbeitet: %s", received_cmd.c_str(), data);

        // Anfrage aus Map entfernen, da sie verarbeitet wurde
        request_map_.erase(received_cmd);
        error_count_map_[received_cmd] = 0;
      }
      else
      {
        ESP_LOGW(TAG, "Antwort für %s erhalten, aber kein passender Request gefunden!", received_cmd.c_str());
      }
      // Jetzt das nächste Kommando senden
      current_command_.clear();
    }

    void WR3223Connector::send_next_request()
    {
      if (!current_command_.empty())
      {
        ESP_LOGD(TAG, "Noch auf Antwort für %s warten, kein neues Kommando senden.",
                 current_command_.c_str());
        return;
      }

      for (auto it = request_map_.begin(); it != request_map_.end();)
      {
        auto &req = *it;
        // Prüfen, ob die letzte Anfrage vor mindestens 500 ms gesendet wurde
        if (millis() - req.second.last_sent < 500)
        {
          ++it;
          continue;
        }

        // Prüfen, ob dieses Kommando zu oft fehlgeschlagen ist
        if (error_count_map_[req.first] >= 3)
        {
          error_count_map_[req.first] = 0;
          ESP_LOGW(TAG,
                   "Kommando %s wurde zu oft nicht beantwortet und wird entfernt.",
                   req.first.c_str());

          // Sicherstellen, dass wir den Callback nach dem Entfernen weiterhin
          // aufrufen koennen
          auto cb = req.second.callback;
          it = request_map_.erase(it);
          
          if (cb)
            cb((char *)"", false);

          continue;
        }

        req.second.last_sent = millis(); // Zeitstempel aktualisieren
        current_command_ = req.first;    // Jetzt läuft dieses Kommando

        ESP_LOGD(TAG, "Sende Kommando %s (write=%d)", current_command_.c_str(),
                 req.second.is_write);

        if (req.second.is_write)
        {
          // Aufbau: EOT + ADR + STX + C1 + C2 + DATA... + ETX + CHK
          size_t len = req.second.data.size();
          std::vector<uint8_t> msg(10 + len); // 5 Header + STX + 2 Cmd + data + ETX + CHK

          // Adressierung und STX
          msg[0] = MessageControl::EOT;
          msg[1] = 0x30; // Zehnerstelle ("0")
          msg[2] = 0x30; // Zehnerstelle wiederholen
          msg[3] = 0x31; // Einerstelle ("1")
          msg[4] = 0x31; // Einerstelle wiederholen
          msg[5] = MessageControl::STX;

          // Kommando
          msg[6] = req.first[0];
          msg[7] = req.first[1];

          for (size_t i = 0; i < len; i++)
          {
            msg[8 + i] = req.second.data[i];
          }
          // Abschluss
          msg[8 + len] = MessageControl::ETX;
          uint8_t chk = buildCheckSum((char *)msg.data(), 6, 8 + len);
          msg[9 + len] = chk;

          write_array(msg.data(), msg.size());
          flush();
          ESP_LOGD(TAG, "Schreibkommando %s gesendet: %s", req.first.c_str(),
                   bytes_to_hex(msg.data(), msg.size()).c_str());
        }
        else
        {
          // Standard-ENQ Nachricht vorbereiten
          static uint8_t message[8] = {MessageControl::EOT,
                                       0x30,
                                       0x30, // Zehnerstelle doppelt (0x30 = '0')
                                       0x31,
                                       0x31, // Einerstelle doppelt (0x31 = '1')
                                       0x00,
                                       0x00, // Platzhalter für das Kommando
                                       MessageControl::ENQ};

          // Nur Kommando-Bytes aktualisieren
          message[5] = req.first[0];
          message[6] = req.first[1];

          write_array(message, 8);
          flush();
          ESP_LOGD(TAG, "Kommando %s gesendet: %s", req.first.c_str(),
                   bytes_to_hex(message, sizeof(message)).c_str());
        }
        return; // Nur ein Kommando senden, dann beenden
      }
    }

  } // namespace wr3223
} // namespace esphome
