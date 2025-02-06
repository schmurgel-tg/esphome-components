#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "wr3223_connector.h"

namespace esphome {
namespace wr3223 {

class WR3223SensorPollingComponent;

class WR3223 : public PollingComponent {
public:
    WR3223(uart::UARTComponent *parent, uint32_t update_interval = 5000)
        : PollingComponent(update_interval), connector_(parent) {}

    void setup() override;
    void update() override;
    void dump_config() override;

    WR3223Connector connector_;

    void set_sensor_polling_component(WR3223SensorPollingComponent* component) {
        this->sensor_polling_component = component;
    }

    WR3223SensorPollingComponent* get_sensor_polling_component() {
        return this->sensor_polling_component;
    }

 private:
    WR3223SensorPollingComponent* sensor_polling_component;
};

}  // namespace wr3223
}  // namespace esphome
