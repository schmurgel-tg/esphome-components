#pragma once

#include "esphome/components/uart/uart.h"
#include "wr3223_constants.h"
#include "wr3223_helper.h"
#include <functional>
#include <unordered_map>


namespace esphome {
namespace wr3223 {

/// @brief nach einem stromausfall oder ähnlichem, verhalten wir uns anders im
/// ersten Update
extern bool freshStart;
/// @brief Wird über den RelaisDecoder aktualisiert
extern bool bedienteilAktiv;

class WR3223Connector : public Component, public uart::UARTDevice {
public:
  WR3223Connector(uart::UARTComponent *parent) : UARTDevice(parent) {}

  void send_request(const char *command, std::function<void(char *answer)> callback);
  void loop() override;

private:
  int readAnswer(char *buffer, int len);
  int buildCheckSum(const char *answer, int start, int end);
  void send_next_request(); // Versendet die nächste Anfrage in der Queue

  struct CallbackInfo {    
    uint32_t last_sent; // Zeitpunkt der Anfrage
    std::function<void(char *answer)> callback;  // Callback bei Antwort
  };

  std::unordered_map<std::string, CallbackInfo> request_map_;  // Dictionary für Requests 
};

} // namespace wr3223
} // namespace esphome
