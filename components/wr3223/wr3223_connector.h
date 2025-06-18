#pragma once

#include "esphome/components/uart/uart.h"
#include "wr3223_constants.h"
#include "wr3223_helper.h"
#include <functional>
#include <unordered_map>

namespace esphome
{
  namespace wr3223
  {
    class WR3223Connector : public Component, public uart::UARTDevice
    {
    public:
      WR3223Connector(uart::UARTComponent *parent) : UARTDevice(parent) {}

      void setup() override;
      void loop() override;

      void send_request(const char *command, std::function<void(char *answer, bool success)> callback);
      void send_write_request(const char *command, const std::string &data, std::function<void(char *answer, bool success)> callback);

    private:
      int readAnswer(char *buffer, int len);
      int buildCheckSum(const char *answer, int start, int end);
      void send_next_request(); // Versendet die nächste Anfrage in der Queue

      struct CallbackInfo
      {
        uint32_t last_sent;                                       // Zeitpunkt der Anfrage
        std::function<void(char *answer, bool success)> callback; // Callback bei Antwort
        bool is_write = false;                                    // Schreibkommando?
        std::string data;                                         // Daten zum Schreiben
      };

      std::unordered_map<std::string, CallbackInfo> request_map_; // Dictionary für Requests
      std::unordered_map<std::string, int> error_count_map_;      // Fehler-Tracking
      std::string current_command_ = "";                          // Das aktuelle Kommando
    };

  } // namespace wr3223
} // namespace esphome
