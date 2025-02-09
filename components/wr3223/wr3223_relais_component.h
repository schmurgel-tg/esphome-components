#pragma once

#include "esphome/core/component.h"
#include "wr3223.h"
#include "wr3223_constants.h"
#include <map>

#ifdef USE_BINARY_SENSOR  // Nur laden, wenn binary_sensor genutzt wird
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif


namespace esphome {
namespace wr3223 {

class WR3223RelaisComponent : public PollingComponent {
public:
  WR3223RelaisComponent(WR3223 *parent, uint32_t update_interval)
      : PollingComponent(update_interval), parent_(parent) {}

  void setup() override;
  void update() override;

  #ifdef USE_BINARY_SENSOR
  void register_relais_sensor(int flag, binary_sensor::BinarySensor *sensor);
  #endif

private:
  WR3223 *parent_;

  #ifdef USE_BINARY_SENSOR
  std::map<int, binary_sensor::BinarySensor *> relais_sensors_;
  #endif
};

} // namespace WR3223
} // namespace esphome
