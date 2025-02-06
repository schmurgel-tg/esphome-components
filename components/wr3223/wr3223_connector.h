#pragma once

#include "esphome/components/uart/uart.h"
#include "wr3223_constants.h"
#include "wr3223_types.h"

namespace esphome {
namespace wr3223 {

/// @brief nach einem stromausfall oder ähnlichem, verhalten wir uns anders im
/// ersten Update
bool freshStart = true;
/// @brief Wird über den RelaisDecoder aktualisiert
bool bedienteilAktiv = true;

class WR3223Connector : public uart::UARTDevice {
  /**
   * Wait until the specified timeout of data are available or the timout is
   * reached.
   *
   * @param dataCount
   * @param timeout
   * @throws IOException
   */
  bool waitUntilDataAvailable(int dataCount, long timeout) {
    uint32_t startTime = millis();
    long duration = 0;
    delay(100);
    while (available() < dataCount && duration < timeout) {
      ESP_LOGD("Reader", "Warte auf Antwort von WR3223. Timeout in %ims.",
               timeout - duration);
      delay(100);
      duration = (millis() - startTime);
    }
    return available() >= dataCount;

    ESP_LOGE("PUBLISHER", "Fehler beim Warten auf die Daten.");
    return false;
  }

  /// @brief Liest den Stream bis zum ETX
  /// @param buffer Der buffer in welchen gelesen werden soll
  /// @param len Länge des buffers
  /// @return liefert die Anzahl der gelesenen Zeichen
  int readAnswer(char *buffer, int len) {
    int pos = 0;
    while (available()) {
      int readch = read();
      if (readch > 0) {
        switch (readch) {
        case MessageControl::ETX: // Ende der Nachricht
          buffer[pos++] = readch;
          // buffer[pos+1] = read(); // Checksumme noch mit dranhängen
          return pos;
        default:
          if (pos < len - 1) {
            buffer[pos++] = readch;
            buffer[pos] = 0;
          }
        }
      }
    }
    // No end of line has been found, so return -1.
    return -1;
  }

  /**
   * Set the command to the message.
   *
   * @param message
   * @param command
   * @param offset
   */
  void setCommandToMessage(uint8_t *message, char commandBit, char command,
                           int offset) {
    message[offset] = commandBit;
    message[offset + 1] = command;
  }

  /**
   * Set the controller address to the message.
   *
   * @param message
   * @param addr
   * @param offset
   */
  void setControllerAddressToMessage(uint8_t *message, int addr, int offset) {
    if (addr > 99) {
      ESP_LOGE("WR3223", "Fehler: Die Adresse muss zwischen 1 und 99 liegen.");
      return;
    }

    std::string addrStr = std::to_string(addr);
    if (addrStr.length() == 1) {
      addrStr = "0" + addrStr;
    }
    char const *pchar = addrStr.c_str();
    message[offset] = pchar[0];
    message[offset + 1] = pchar[0];
    message[offset + 2] = pchar[1];
    message[offset + 3] = pchar[1];
  }

  /**
   * Answer to string
   *
   * @param answer as bytes
   * @param start
   * @param end
   * @return the answer as string value.
   */
  std::string toString(char *answer, int start, int end) {
    std::string data = "";
    for (int ii = start; ii <= end; ii++) {
      char ch = (char)answer[ii];
      data = data + ch;
    }
    return data;
  }

  /**
   * Build the checksum.
   *
   * @param answer
   * @param start
   * @param end
   * @return checksum of the answer byte array.
   */
  int buildCheckSum(const char *answer, int start, int end) {
    int chkSum = (int)answer[start];
    for (int i = start + 1; i <= end; i++) {
      chkSum = chkSum ^ (int)answer[i];
    }
    return chkSum;
  }

  int buildCheckSum(uint8_t *msg, int start, int end) {
    int chkSum = (int)msg[start];
    for (int i = start + 1; i <= end; i++) {
      chkSum = chkSum ^ (int)msg[i];
    }
    return chkSum;
  }

  int get_data_length(char *data, int maxLength) {
    int counter = 0;
    if (maxLength < DATA_START)
      return 0;

    // ESP_LOGD("get_data_length", "Data MaxLength: %i", maxLength);

    for (int i = DATA_START; i < maxLength + DATA_START; i++) {
      // ESP_LOGD("get_data_length", "Data Position: %i, Data Char: %c",i,
      // data[i]);
      if (uint8_t(data[i]) == MessageControl::ETX)
        return counter;
      else
        counter++;
    }

    ESP_LOGD("get_data_length",
             "Kein Abschluss-Flag gefunden, so können wir die Datenlänge nicht "
             "bestimmen. (counter: %i)",
             counter);
    return 0;
  }

public:
  WR3223Connector(uart::UARTComponent *parent) : UARTDevice(parent) {}

  /// @brief Liest die ANtwort eines Kommandos n den übergeben buffer und
  /// Liefert die Anzahl der Datenbits
  /// @param answer Der buffer für die Antwort
  /// @param max_line_length Die maximale Länger der Antwort (des buffers)
  /// @param pCmd Das Kommandos, welches abgesetzt werden soll
  /// @return Liefert die Anazahl der Datenbits (Achtung, nicht die Position!!,
  /// die Datenbits fangen nicht bei 0 an!!)
  int readLine(char *answer, int max_line_length, CommandPair pCmd) {
    // Write read message to the controller
    static uint8_t message[8] = {
        MessageControl::EOT, 0x30, 0x30, 0x31, 0x31, 0x00, 0x00,
        MessageControl::ENQ};
    setControllerAddressToMessage(message, 1, 1);
    setCommandToMessage(message, pCmd.GetCommandBit(), pCmd.GetCommand(), 5);

    write_array(message, 8);
    flush();

    if (waitUntilDataAvailable(10, 5000) == false)
      return 0;

    // Read answer from controller
    int answerLength = readAnswer(answer, max_line_length);
    int chkSum = read(); // und die Checksumme der Antwort

    // ESP_LOGD("Reader", "Read data: %s with checksum %x.", answer, chkSum);
    //  Check answer1
    if ((uint8_t)answer[0] != MessageControl::STX ||
        (uint8_t)answer[answerLength - 1] != MessageControl::ETX) {
      ESP_LOGE("Reader", "Start/end of the connector answer is wrong.");
      return 0;
    }

    if (chkSum != buildCheckSum(answer, 1, answerLength - 1)) {
      ESP_LOGE("Reader", "Fehler: Checksum error. Expected %x but got %x.",
               chkSum, buildCheckSum(answer, 1, answerLength - 1));
      return 0;
    }

    if (pCmd.GetCommandBit() != answer[1] || pCmd.GetCommand() != answer[2]) {
      ESP_LOGE("Reader",
               "Fehler: Wrong command received. Expected %s but got %c%c.",
               pCmd.cmd, answer[1], answer[2]);
    }
    return get_data_length(answer, answerLength);
  }

  bool write(CommandPair cmd, const char *data) {
    int dataLength = strlen(data);
    // Check if the provided data not longer then 6 characters.
    if (data == 0 || dataLength > 6 || dataLength == 0) {
      ESP_LOGE("WRITER", "Not valid data format.");
      return false;
    }

    ESP_LOGE("WRITER", "WRITE: %s", data);
    ESP_LOGE("WRITER", "Data Length: %i", dataLength);
    ESP_LOGE("WRITER", "COMMAND: %S", cmd.cmd);

    int msgLength = 10 + dataLength;
    // Write command to the controller
    uint8_t message[msgLength];
    message[0] = MessageControl::EOT;
    setControllerAddressToMessage(message, 1, 1);
    message[5] = MessageControl::STX;
    setCommandToMessage(message, cmd.GetCommandBit(), cmd.GetCommand(), 6);

    // byte[] d = data.getBytes();

    for (int ii = 0; ii < dataLength; ii++) {
      message[8 + ii] = data[ii];
    }

    // ESP_LOGE("WRITER", "Data in Hex: %s", dataInHex.c_str());
    // ESP_LOGE("WRITER", "Data Length: %i", dataLength);

    message[msgLength - 2] = MessageControl::ETX;
    uint8_t chkSum = (uint8_t)buildCheckSum(message, 6, dataLength + 2 + 6);
    message[msgLength - 1] = chkSum;

    ESP_LOGE("WRITER", "Data to Write: %s",
             WR3223Helper::to_hex_string(message, msgLength).c_str());

    if (bedienteilAktiv) {

      ESP_LOGE("WRITER", "Bedienteil ist aktiv, kein schreiben möglich");
      return false;
    }

    write_array(message, msgLength);
    flush();
    // Read controller answer
    if (waitUntilDataAvailable(1, 5000l)) {
      const static int max_length = 10;
      static char answer[max_length];

      int answerLength = readAnswer(answer, max_length);
      if (answerLength == 0) {
        ESP_LOGE("WRITER", "Keine Antwort erhalten!");
        return false;
      }

      ESP_LOGD("WRITER", "Antwort erhalten: %s",
               WR3223Helper::to_hex_string(answer, answerLength).c_str());
      if (answer[0] == MessageControl::ACK) {
        ESP_LOGI("WRITER", "Antwort POSITIV");
        return true;
      } else {
        ESP_LOGE("WRITER", "Antwort NEGATIV");
        return false;
      }
    } else
      ESP_LOGE("WRITER", "Timeout - keine Antwort erhalten!");
    return false;
  }
};
} // namespace wr3223
} // namespace esphome