#pragma once

#include "esphome/core/component.h"
#include "wr3223.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace wr3223 {

class WR3223ErrorComponent : public PollingComponent {
public:
  WR3223ErrorComponent(WR3223 *parent, uint32_t update_interval)
      : PollingComponent(update_interval), parent_(parent) {}

  void setup() override;
  void update() override;

  void register_status_sensor(binary_sensor::BinarySensor *sensor);
  void register_text_sensor(text_sensor::TextSensor *sensor);

private:
  WR3223 *parent_;

  binary_sensor::BinarySensor *errorStatus = nullptr;
  text_sensor::TextSensor *errorText = nullptr;

  void process_response(char *response);
};

} // namespace wr3223
} // namespace esphome
