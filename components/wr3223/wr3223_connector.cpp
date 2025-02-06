#include "wr3223_connector.h"
#include "esphome/core/log.h"

namespace esphome {
namespace wr3223 {

// Definition der globalen Variablen (eine einzige Definition in dieser Datei)
/// @brief Gibt an, ob ein frischer Start nach einem Stromausfall erfolgt
bool freshStart = true;

/// @brief Wird über den RelaisDecoder aktualisiert
bool bedienteilAktiv = true;

WR3223Connector::WR3223Connector(uart::UARTComponent *parent)
    : UARTDevice(parent) {}

/// @brief Wartet darauf, dass eine bestimmte Anzahl von Daten verfügbar ist
/// oder das Timeout abläuft.
/// @param dataCount Erwartete Anzahl der verfügbaren Bytes
/// @param timeout Maximale Wartezeit in Millisekunden
/// @return `true`, wenn genügend Daten empfangen wurden, sonst `false`
bool WR3223Connector::waitUntilDataAvailable(int dataCount, long timeout) {
  uint32_t startTime = millis();
  while (millis() - startTime < timeout) {
        if (available() >= dataCount) {
            return true;  // Genug Daten empfangen
        }

        // WDT verhindern:
        yield();  // Gibt die Kontrolle zurück, damit andere Tasks laufen können
        delay(10); // Kurze Pause, um nicht 100% CPU-Last zu verursachen
        
    }

  ESP_LOGE("PUBLISHER", "Fehler beim Warten auf die Daten.");
  return false;
}

/// @brief Liest eine Antwort vom Gerät bis zum ETX-Zeichen.
/// @param buffer Der Buffer zum Speichern der gelesenen Daten
/// @param len Maximale Länge des Buffers
/// @return Anzahl der gelesenen Zeichen, oder `-1`, wenn kein Abschluss-Flag
/// gefunden wurde
int WR3223Connector::readAnswer(char *buffer, int len) {
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

/// @brief Setzt das Kommando in die Nachricht ein.
/// @param message Das Nachrichtenarray
/// @param commandBit Das erste Kommando-Bit
/// @param command Das zweite Kommando-Bit
/// @param offset Position im Nachrichtenarray
void WR3223Connector::setCommandToMessage(uint8_t *message, char commandBit,
                                          char command, int offset) {
  message[offset] = commandBit;
  message[offset + 1] = command;
}

/// @brief Setzt die Controller-Adresse in die Nachricht ein.
/// @param message Das Nachrichtenarray
/// @param addr Die Adresse (muss zwischen 1 und 99 liegen)
/// @param offset Position im Nachrichtenarray
void WR3223Connector::setControllerAddressToMessage(uint8_t *message, int addr,
                                                    int offset) {
  if (addr > 99) {
    ESP_LOGE("WR3223_Connector",
             "Fehler: Die Adresse muss zwischen 1 und 99 liegen.");
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

/// @brief Wandelt eine Antwort in einen String um.
/// @param answer Die Antwort als Byte-Array
/// @param start Startposition
/// @param end Endposition
/// @return Antwort als String
std::string WR3223Connector::toString(char *answer, int start, int end) {
  std::string data = "";
  for (int ii = start; ii <= end; ii++) {
    char ch = (char)answer[ii];
    data = data + ch;
  }
  return data;
}

/// @brief Berechnet die Checksumme einer Antwort.
/// @param answer Die Antwort als Byte-Array
/// @param start Startposition
/// @param end Endposition
/// @return Checksumme als Integer
int WR3223Connector::buildCheckSum(const char *answer, int start, int end) {
  int chkSum = (int)answer[start];
  for (int i = start + 1; i <= end; i++) {
    chkSum = chkSum ^ (int)answer[i];
  }
  return chkSum;
}

int WR3223Connector::buildCheckSum(uint8_t *msg, int start, int end) {
  int chkSum = (int)msg[start];
  for (int i = start + 1; i <= end; i++) {
    chkSum = chkSum ^ (int)msg[i];
  }
  return chkSum;
}

int WR3223Connector::get_data_length(char *data, int maxLength) {
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

  ESP_LOGD("WR3223_Connector",
           "Kein Abschluss-Flag gefunden, so können wir die Datenlänge nicht "
           "bestimmen. (counter: %i)",
           counter);
  return 0;
}

/// @brief Liest eine Antwort vom Gerät für ein bestimmtes Kommando.
/// @param answer Der Buffer für die Antwort
/// @param max_line_length Maximale Länge des Buffers
/// @param pCmd Das Kommando
/// @return Anzahl der Datenbits (nicht die Position!)
int WR3223Connector::readLine(char *answer, int max_line_length,
                              CommandPair pCmd) {
  static uint8_t message[8] = {
      MessageControl::EOT, 0x30, 0x30, 0x31, 0x31, 0x00, 0x00,
      MessageControl::ENQ};
  setControllerAddressToMessage(message, 1, 1);
  setCommandToMessage(message, pCmd.GetCommandBit(), pCmd.GetCommand(), 5);

  write_array(message, 8);
  flush();

  if (waitUntilDataAvailable(10, 10) == false)
    return 0;

  int answerLength = readAnswer(answer, max_line_length);
  int chkSum = read();

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

/// @brief Sendet ein Kommando mit den angegebenen Daten.
/// @param cmd Das Kommando
/// @param data Die zu sendenden Daten
/// @return `true`, wenn das Schreiben erfolgreich war, sonst `false`
bool WR3223Connector::write(CommandPair cmd, const char *data) {
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

} // namespace wr3223
} // namespace esphome
