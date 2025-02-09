#pragma once

#include <string>
#include "esphome/core/log.h"

#ifdef USE_SENSOR
#include "esphome/components/sensor/sensor.h"
#endif

#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif

#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

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
    #ifdef USE_SENSOR
    sensor::Sensor *sensor = nullptr;
    #endif

    #ifdef USE_BINARY_SENSOR
    binary_sensor::BinarySensor *binarySensor = nullptr;
    #endif

    #ifdef USE_TEXT_SENSOR
    text_sensor::TextSensor *textSensor = nullptr;
    #endif    
    std::function<bool(const char*)> publisherFunc = nullptr; 

public:
    #ifdef USE_SENSOR
    CommandPair(const char *pCmd, sensor::Sensor *sensor) 
        : cmd(pCmd), sensor(sensor), resultType(ECommandResultType::Number) {}
    #endif

    #ifdef USE_BINARY_SENSOR
    CommandPair(const char *pCmd, binary_sensor::BinarySensor *binarySensor) 
        : cmd(pCmd), binarySensor(binarySensor), resultType(ECommandResultType::Bool) {}
    #endif

    #ifdef USE_TEXT_SENSOR
    CommandPair(const char *pCmd, text_sensor::TextSensor *textSensor) 
        : cmd(pCmd), textSensor(textSensor), resultType(ECommandResultType::Text) {}
    #endif

    CommandPair(const char *pCmd, ECommandResultType pResultType) 
        : cmd(pCmd), resultType(pResultType) {}

    CommandPair(const char *pCmd, std::function<bool(const char*)> function)
        : cmd(pCmd), publisherFunc(function) {}

    const char* cmd;
    char GetCommandBit() { return cmd[0]; }
    char GetCommand() { return cmd[1]; }
    ECommandResultType resultType;

    bool publish_data(const std::string &data) {        
            #ifdef USE_SENSOR
            if (resultType == ECommandResultType::Number && sensor != nullptr) {
                sensor->publish_state(WR3223Helper::to_float(data));
                return true;
            }
            #endif

            #ifdef USE_BINARY_SENSOR
            if (resultType == ECommandResultType::Bool && binarySensor != nullptr) {
                binarySensor->publish_state(data != "0");
                return true;
            }
            #endif

            #ifdef USE_TEXT_SENSOR
            if (resultType == ECommandResultType::Text && textSensor != nullptr) {
                textSensor->publish_state(data);
                return true;
            }
            #endif

            if (publisherFunc != nullptr) {
                return publisherFunc(data.c_str());
            }
        
            ESP_LOGD("COMMAND", "Cmd %s tried to publish_data: %s", cmd, data.c_str());            
            return false;        
    }
};

}  // namespace wr3223
}  // namespace esphome
