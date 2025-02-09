#pragma once

#include "esphome/core/component.h"
#include "wr3223.h"

#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif

#ifdef USE_TEXT_SENSOR
#include "esphome/components/text_sensor/text_sensor.h"
#endif

namespace esphome {
namespace wr3223 {

class WR3223ErrorComponent : public PollingComponent {
public:
  WR3223ErrorComponent(WR3223 *parent, uint32_t update_interval)
      : PollingComponent(update_interval), parent_(parent) {}

  void setup() override;
  void update() override;

#ifdef USE_BINARY_SENSOR
  void register_status_sensor(binary_sensor::BinarySensor *sensor);
#endif

#ifdef USE_TEXT_SENSOR
  void register_text_sensor(text_sensor::TextSensor *sensor);
#endif

private:
  WR3223 *parent_;

#ifdef USE_BINARY_SENSOR
  binary_sensor::BinarySensor *errorStatus = nullptr;
#endif

#ifdef USE_TEXT_SENSOR
  text_sensor::TextSensor *errorText = nullptr;
#endif
};

} // namespace wr3223
} // namespace esphome
