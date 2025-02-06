#pragma once

#include "esphome/components/uart/uart.h"
#include "wr3223_constants.h"
#include "wr3223_types.h"

namespace esphome {
namespace wr3223 {

/// @brief nach einem stromausfall oder ähnlichem, verhalten wir uns anders im
/// ersten Update
extern bool freshStart;
/// @brief Wird über den RelaisDecoder aktualisiert
extern bool bedienteilAktiv;

class WR3223Connector : public uart::UARTDevice {
public:
  WR3223Connector(uart::UARTComponent *parent);

  /// @brief Liest die Antwort eines Kommandos in den übergebenen Buffer und
  /// liefert die Anzahl der Datenbits.
  /// @param answer Der Buffer für die Antwort
  /// @param max_line_length Die maximale Länge der Antwort (des Buffers)
  /// @param pCmd Das Kommando, das gesendet werden soll
  /// @return Anzahl der Datenbits (nicht die Position, Datenbits starten nicht
  /// bei 0)
  int readLine(char *answer, int max_line_length, CommandPair pCmd);

  /// @brief Sendet einen Befehl mit den angegebenen Daten an das Gerät.
  /// @param cmd Das zu sendende Kommando
  /// @param data Die zu sendenden Daten (maximal 6 Zeichen)
  /// @return `true`, wenn das Schreiben erfolgreich war, sonst `false`
  bool write(CommandPair cmd, const char *data);

private:
  bool waitUntilDataAvailable(int dataCount, long timeout);
  int readAnswer(char *buffer, int len);
  void setCommandToMessage(uint8_t *message, char commandBit, char command,
                           int offset);
  void setControllerAddressToMessage(uint8_t *message, int addr, int offset);
  std::string toString(char *answer, int start, int end);
  int buildCheckSum(const char *answer, int start, int end);
  int buildCheckSum(uint8_t *msg, int start, int end);
  int get_data_length(char *data, int maxLength);
};

} // namespace wr3223
} // namespace esphome
