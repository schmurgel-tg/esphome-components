#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "wr3223_connector.h"

namespace esphome {
namespace wr3223 {

class WR3223 : public PollingComponent {
public:
    WR3223(uart::UARTComponent *parent, uint32_t update_interval = 5000)
        : PollingComponent(update_interval), connector_(parent) {}

    void setup() override;
    void update() override;
    void dump_config() override;

    WR3223Connector connector_;
};

}  // namespace wr3223
}  // namespace esphome
