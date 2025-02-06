#pragma once

#include <string>
#include "esphome/core/log.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "wr3223_helper.h"

namespace esphome {
namespace wr3223 {

/// @brief Die Art des Ergebnisses, welches beim Lesen eines Befehls zu erwarten ist
enum class ECommandResultType {
    Number,
    Text,
    Bool,
    Relais,
    Error,
    Status,
    Modus
};

class CommandPair {
    sensor::Sensor *sensor = nullptr;
    binary_sensor::BinarySensor *binarySensor = nullptr;
    text_sensor::TextSensor *textSensor = nullptr;
    bool (*publisherFunc)(const char*) = nullptr;

public:
    CommandPair(const char *pCmd, sensor::Sensor *sensor) 
        : cmd(pCmd), sensor(sensor), resultType(ECommandResultType::Number) {}

    CommandPair(const char *pCmd, binary_sensor::BinarySensor *binarySensor) 
        : cmd(pCmd), binarySensor(binarySensor), resultType(ECommandResultType::Bool) {}

    CommandPair(const char *pCmd, text_sensor::TextSensor *textSensor) 
        : cmd(pCmd), textSensor(textSensor), resultType(ECommandResultType::Text) {}

    CommandPair(const char *pCmd, ECommandResultType pResultType) 
        : cmd(pCmd), resultType(pResultType) {}

    CommandPair(const char *pCmd, bool (*function)(const char*)) 
        : cmd(pCmd), publisherFunc(function) {}

    const char* cmd;
    char GetCommandBit() { return cmd[0]; }
    char GetCommand() { return cmd[1]; }
    ECommandResultType resultType;

    bool publish_data(const std::string &data) {
        try {
            if (resultType == ECommandResultType::Number && sensor != nullptr) {
                sensor->publish_state(WR3223Helper::to_float(data));
                return true;
            }
            if (resultType == ECommandResultType::Bool && binarySensor != nullptr) {
                binarySensor->publish_state(data != "0");
                return true;
            }
            if (resultType == ECommandResultType::Text && textSensor != nullptr) {
                textSensor->publish_state(data);
                return true;
            }
            if (publisherFunc != nullptr) {
                return publisherFunc(data.c_str());
            }
            return false;
        } catch (const std::exception &e) {
            ESP_LOGD("COMMAND", "Cmd %s tried to publish_data: %s", cmd, data.c_str());
            ESP_LOGD("COMMAND", "Exception occurred: %s", e.what());
            return false;
        }
    }
};

}  // namespace wr3223
}  // namespace esphome
